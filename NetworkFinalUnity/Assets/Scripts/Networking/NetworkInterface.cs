using System;
using System.Collections.Generic;
using System.IO;
using MLAPI;
using MLAPI.Configuration;
using MLAPI.Connection;
using MLAPI.Messaging;
using MLAPI.Serialization;
using MLAPI.Transports;
using UnityEngine;
using UnityEngine.UI;

[Flags]
public enum PlayerInput
{
    E = 0, // empty
    W = 1,
    A = 2,
    S = 4,
    D = 8
}

public enum MessageType
{
	PLAYER_INPUT,	// sending a controllers last input
	PLAYER_SPATIAL, // sending an objects position
	MAP_EVENT,		// sending a map event (cell change)
	PLAYER_COLOR,	// sending a controllers color
	REQUEST_COLORS,	// requesting/sending all colors
	GAME_START,     // server telling client the game started
	GAME_END		// server telling client the game ended
}

public class NetworkInterface : MonoBehaviour
{
	public static NetworkInterface Instance;

	private NetworkConfig _config;
	private NetworkedGameManager _manager;
	private Text _messageLog;

	private void Awake()
	{
		Instance = this;
	}

	public void Start()
	{
		_config = NetworkManager.Singleton.NetworkConfig;
		_manager = GameObject.FindGameObjectWithTag("GameManager").GetComponent<NetworkedGameManager>();

		_messageLog = GameObject.FindWithTag("MessageLog").GetComponent<Text>();
		CustomMessagingManager.OnUnnamedMessage += ProcessUnnamedMessage;
	}

	// Update is called once per frame
	void Update()
	{

	}

	private void ProcessUnnamedMessage(ulong clientid, Stream stream)
	{
		//Debug.Log("Unnamed Message received (" + stream.Length + " bytes) from " + clientid);

		NetworkReader reader = new NetworkReader(stream);
		// get message type
		MessageType type = (MessageType)reader.ReadNibble();

		// process message based on size
		switch (type)
		{
			case MessageType.PLAYER_INPUT:
				ReceivePlayerInput(clientid, reader);
				break;
			case MessageType.PLAYER_SPATIAL:
				ReceivePlayerSpatial(clientid, reader);
				break;
			case MessageType.MAP_EVENT:
				ReceiveMapEvent(clientid, reader);
				break;
			case MessageType.PLAYER_COLOR:
				ReceivePlayerColor(clientid, reader);
				break;
			case MessageType.REQUEST_COLORS:
				HandleColorRequest(clientid, reader);
				break;
			case MessageType.GAME_START:
				HandleGameStartMessage(clientid, reader);
				break;
			case MessageType.GAME_END:
				ReceiveEndGame(clientid, reader);
				break;
		}
	}

	// - handlers if message type is interpreted differently by server v client - //
	// handle color request response
	private void HandleColorRequest(ulong clientid, NetworkReader stream)
	{
		// if color request message received by server, send colors to sender
		if (NetworkManager.Singleton.IsServer)
		{
			ReceiveAllColorRequest(clientid, stream);
		}
		else // otherwise, process the recieved colors
		{
			ReceiveAllColors(clientid, stream);
		}
	}
	// handle game start message response 
	private void HandleGameStartMessage(ulong clientid, NetworkReader stream)
	{
		// if color request message received by server, send colors to sender
		if (NetworkManager.Singleton.IsServer)
		{
			_manager.TryStartGame();
		}
		else // otherwise, process the recieved colors
		{
			_manager.StartGame();
		}
	}


	/* - Broadcast data to all connected clients - */
	public void BroadcastMapEvent(ulong netObjId, int cell)
	{
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);

		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.MAP_EVENT);
		// pack index => 2 bits
		writer.WriteNibble((byte)netObjId);
		// cell => 8 bits
		writer.WriteByte((byte)cell);

		// get all of the client ids
		List<NetworkClient> clients = NetworkManager.Singleton.ConnectedClientsList;

		// turn to list of client ids
		List<ulong> clientIds = new List<ulong>();
		foreach (NetworkClient client in clients)
			clientIds.Add(client.ClientId);

		//Debug.Log("Broadcasting input for " + netObjId + " (" + cell + ") to " + clientIds[0] + " clients");

		// send message to all connected clients
		CustomMessagingManager.SendUnnamedMessage(clientIds, buffer, NetworkChannel.DefaultMessage);
	}

	public void BroadcastPlayerInput(ulong netObjId, PlayerInput input)
	{
		// pack all of the data into the buffer
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);
		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.PLAYER_INPUT);
		// index => 4 bits
		writer.WriteNibble((byte)netObjId);
		// input flags => 4 bits
		writer.WriteNibble((byte)input);

		// get all of the client ids
		List<NetworkClient> clients = NetworkManager.Singleton.ConnectedClientsList;

		// turn to list of client ids
		List<ulong> clientIds = new List<ulong>();
		foreach (NetworkClient client in clients)
			clientIds.Add(client.ClientId);

		//Debug.Log("Broadcasting input for " + netObjId + " (" + input + ") to " + clientIds[0] + " clients");

		// send message to all connected clients
		CustomMessagingManager.SendUnnamedMessage(clientIds, buffer, NetworkChannel.DefaultMessage);
	}
	// (server) broadcast player spatial data to all clients
	public void BroadcastPlayerSpatial(ulong netObjId, Transform t)
	{
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);

		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.PLAYER_SPATIAL);
		// net obj id => 4 bits
		writer.WriteNibble((byte)netObjId);
		// pos => 3*13 bits
		Vector3 pos = t.position;
		writer.WriteBits(CompressPositionValue(pos.x, 13), 13);
		writer.WriteBits(CompressPositionValue(pos.y, 13), 13);
		writer.WriteBits(CompressPositionValue(pos.z, 13), 13);
		// rot => 3*13 bits
		Vector3 rot = t.rotation.eulerAngles;
		writer.WriteBits(CompressRotationValue(rot.x, 13), 13);
		writer.WriteBits(CompressRotationValue(rot.y, 13), 13);
		writer.WriteBits(CompressRotationValue(rot.z, 13), 13);

		// get all of the client ids
		List<NetworkClient> clients = NetworkManager.Singleton.ConnectedClientsList;

		// turn to list of client ids
		List<ulong> clientIds = new List<ulong>();
		foreach (NetworkClient client in clients)
			clientIds.Add(client.ClientId);

		// send message to all connected clients
		CustomMessagingManager.SendUnnamedMessage(clientIds, buffer, NetworkChannel.DefaultMessage);

		//_messageLog.text = "Pos: " + pos.ToString("F4") + "\nRot: " + t.rotation.ToString("F4");
	}
	// (server) broadcast a player's color to all clients
	public void BroadcastPlayerColor(ulong netObjId, Color color)
	{
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);

		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.PLAYER_COLOR);
		// net obj id => 4 bits
		writer.WriteNibble((byte)netObjId);
		// color => 3*13 bits
		writer.WriteByte((byte)CompressColorValue(color.r, 8));
		writer.WriteByte((byte)CompressColorValue(color.g, 8));
		writer.WriteByte((byte)CompressColorValue(color.b, 8));

		// get all of the client ids
		List<NetworkClient> clients = NetworkManager.Singleton.ConnectedClientsList;

		// turn to list of client ids
		List<ulong> clientIds = new List<ulong>();
		foreach (NetworkClient client in clients)
			clientIds.Add(client.ClientId);

		// send message to all connected clients
		CustomMessagingManager.SendUnnamedMessage(clientIds, buffer, NetworkChannel.DefaultMessage);

		_messageLog.text = color.ToString();
	}
	// (server) broadcast game start message to clients
	public void BroadcastStartMessage()
	{
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);

		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.GAME_START);

		// get all of the client ids
		List<NetworkClient> clients = NetworkManager.Singleton.ConnectedClientsList;

		// turn to list of client ids
		List<ulong> clientIds = new List<ulong>();
		foreach (NetworkClient client in clients)
			clientIds.Add(client.ClientId);

		// send message to all connected clients
		CustomMessagingManager.SendUnnamedMessage(clientIds, buffer, NetworkChannel.DefaultMessage);
	}
	// (server) broadcast game end message to clients
	public void BroadcastEndMessage(List<ulong> netObjs, List<ulong> scores)
	{
		// - package the data - //
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);
		
		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.GAME_END);
		// number of scores to expect => 4 bits
		writer.WriteNibble((byte)netObjs.Count);

		// loop thru adding all clients
		for (int i = 0; i < netObjs.Count; i++)
		{
			// net obj id => 4 bits
			writer.WriteNibble((byte)netObjs[i]);
			// score => 8 bits (max score of 256)
			writer.WriteByte((byte)scores[i]);
		}

		// get all of the client ids
		List<NetworkClient> clients = NetworkManager.Singleton.ConnectedClientsList;

		// turn to list of client ids
		List<ulong> clientIds = new List<ulong>();
		foreach (NetworkClient client in clients)
			clientIds.Add(client.ClientId);

		Debug.Log("broadcasting scores to " + clients.Count);

		// broadcast to everyone
		CustomMessagingManager.SendUnnamedMessage(clientIds, buffer, NetworkChannel.DefaultMessage);
	}


	/* - private handlers for when messages are received - */
	// receiving a controller'sinput (either direction)
	private void ReceivePlayerInput(ulong clientid, NetworkReader stream)
	{
		//NetworkReader reader = new NetworkReader(stream);

		ulong netObjID = stream.ReadNibble();
		PlayerInput pInput = (PlayerInput)stream.ReadNibble();

		_manager.UpdateClientInput(netObjID, pInput);

		//Debug.Log("Recieved input from p " + pIndex + ": " + pInput);
		//_messageLog.text = "Input from " + clientid + ": " + pInput;
	}

	// handles recieving spatial data from server
	private void ReceivePlayerSpatial(ulong clientid, NetworkReader stream)
	{
		// read in player index
		ulong netObjID = stream.ReadNibble();

		// read in position
		int compPosVal = (int)stream.ReadBits(13);
		float posX = DecompressPositionValue(compPosVal, 13);
		compPosVal = (int)stream.ReadBits(13);
		float posY = DecompressPositionValue(compPosVal, 13);
		compPosVal = (int)stream.ReadBits(13);
		float posZ = DecompressPositionValue(compPosVal, 13);
		Vector3 pos = new Vector3(posX, posY, posZ);

		// read in rotation
		int compRotVal = (int)stream.ReadBits(13);
		float rotX = DecompressRotationValue(compRotVal, 13);
		compRotVal = (int)stream.ReadBits(13);
		float rotY = DecompressRotationValue(compRotVal, 13);
		compRotVal = (int)stream.ReadBits(13);
		float rotZ = DecompressRotationValue(compRotVal, 13);
		Quaternion rot = Quaternion.Euler(rotX, rotY, rotZ);

		_manager.UpdateClientSpatial(netObjID, pos, rot);

		//Debug.Log("Spatial from " + clientid + ": " + pos);
		//_messageLog.text = "Pos: " + pos.ToString("F4") + "\nRot: " + rot.ToString("F4");
	}
	private void ReceiveMapEvent(ulong clientid, NetworkReader stream)
	{
		//NetworkReader reader = new NetworkReader(stream);
		// read in player index
		ulong netObjID = stream.ReadNibble();

		int cell = (int)stream.ReadByte();

		_manager.UpdateClientMap(netObjID, cell);

		//Debug.Log("Recieved input from p " + cell + ": " + netObjID);
		//_messageLog.text = "Map evt: " + cell;
	}
	// recieve specific controller color
	private void ReceivePlayerColor(ulong clientid, NetworkReader stream)
	{
		// read in player index
		ulong netObjID = stream.ReadNibble();

		// read in position
		int compColorVal = (int)stream.ReadByte();
		float colR = DecompressColorValue(compColorVal, 8);
		compColorVal = (int)stream.ReadByte();
		float colG = DecompressColorValue(compColorVal, 8);
		compColorVal = (int)stream.ReadByte();
		float colB = DecompressColorValue(compColorVal, 8);
		Color plColor = new Color(colR, colG, colB);

		_manager.UpdateClientColor(netObjID, plColor);

		//Debug.Log("Spatial from " + clientid + ": " + pos);
		_messageLog.text = plColor.ToString();
	}
	// (server) receive color request
	private void ReceiveAllColorRequest(ulong clientid, NetworkReader stream)
	{
		List<ulong> clients = new List<ulong>();
		List<Color> colors = new List<Color>();
		// get the info from game manager
		_manager.RetrieveClientColors(ref clients, ref colors);

		// - package the data - //
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);
		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.REQUEST_COLORS);
		// number of colors to expect => 4 bits
		writer.WriteNibble((byte)clients.Count);

		// loop thru adding all clients
		for (int i = 0; i < clients.Count; i++)
		{
			// net obj id => 4 bits
			writer.WriteNibble((byte)clients[i]);
			// color => 3*13 bits
			writer.WriteByte((byte)CompressColorValue(colors[i].r, 8));
			writer.WriteByte((byte)CompressColorValue(colors[i].g, 8));
			writer.WriteByte((byte)CompressColorValue(colors[i].b, 8));
		}

		// send off to requestee
		CustomMessagingManager.SendUnnamedMessage(clientid, buffer, NetworkChannel.DefaultMessage);
	}
	// (client) receiving all colors
	private void ReceiveAllColors(ulong clientid, NetworkReader stream)
	{
		// lists to hold the data in
		List<ulong> clients = new List<ulong>();
		List<Color> colors = new List<Color>();

		// number of colors to expect => 4 bits
		int numColors = stream.ReadNibble();

		// loop thru adding all clients
		for (int i = 0; i < numColors; i++)
		{
			// get the net obj id
			clients.Add(stream.ReadNibble());
			// get the color
			int compColorVal = (int)stream.ReadByte();
			float colR = DecompressColorValue(compColorVal, 8);
			compColorVal = (int)stream.ReadByte();
			float colG = DecompressColorValue(compColorVal, 8);
			compColorVal = (int)stream.ReadByte();
			float colB = DecompressColorValue(compColorVal, 8);
			Color plColor = new Color(colR, colG, colB);
			colors.Add(plColor);
		}

		// send data to manager to update
		_manager.UpdateClientColors(clients, colors);
	}
	// (client) receiving end game scores
	private void ReceiveEndGame(ulong clientid, NetworkReader stream)
	{
		// lists to hold the data in
		List<ulong> clients = new List<ulong>();
		List<ulong> scores = new List<ulong>();

		// number of colors to expect => 4 bits
		int numScores = stream.ReadNibble();

		// loop thru adding all clients
		for (int i = 0; i < numScores; i++)
		{
			// get the net obj id
			clients.Add(stream.ReadNibble());
			// get the score
			scores.Add((ulong)stream.ReadByte());
		}

		// send data to manager to update
		_manager.DisplayFinalScores(clients, scores);
	}


	/* - public functions for players to send info to server - */
	public void SendPlayerInputToServer(ulong target, ulong netObjId, PlayerInput input)
	{
		//_messageLog.text = "Sending input to " + target;

		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);
		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.PLAYER_INPUT);
		// net obj id => 4 bits
		writer.WriteNibble((byte)netObjId);
		// input flags => 4 bits
		writer.WriteNibble((byte)input);

		CustomMessagingManager.SendUnnamedMessage(target, buffer, NetworkChannel.DefaultMessage);
	}

	// client requesting all controller colors
	public void SendColorRequest(ulong target)
	{
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);

		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.REQUEST_COLORS);

		// no other data required ...

		CustomMessagingManager.SendUnnamedMessage(target, buffer, NetworkChannel.DefaultMessage);
	}
	// (client) requesting the game to start
	public void SendStartRequestToServer(ulong target)
	{
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);

		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.GAME_START);

		// no other data required ...

		CustomMessagingManager.SendUnnamedMessage(target, buffer, NetworkChannel.DefaultMessage);
	}
	

	/* - Compression/quantization helpers - */
	const float posMin = -15f;
	const float posMax = 25f;
	const float posRange = posMax - posMin;
	private ulong CompressPositionValue(float val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1;
		float serial = (val - posMin); // get make so min value is 0
		float ratio = serial / posRange; // divide by whole range
		ulong compressed = (ulong)Mathf.RoundToInt(ratio * (float)maxVal); // convert to integer
		return compressed;
	}
	private float DecompressPositionValue(int val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1; // bitshift to get max value in provided number of bits
		float ratio = (float)val / (float)maxVal; // get the ratio
		float serial = ratio * posRange; // get the serialized value
		float ret = serial + posMin; // convert back to actual value
		return ret;
	}

	const float rotMax = 360f;
	private ulong CompressRotationValue(float val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1;
		ulong compressed = (ulong)Mathf.RoundToInt(val / rotMax * (float)maxVal);
		return compressed;
	}
	private float DecompressRotationValue(int val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1;
		float ratio = (float)val / (float)maxVal * rotMax;
		return ratio;
	}

	private ulong CompressColorValue(float val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1;
		ulong compressed = (ulong)Mathf.RoundToInt(val * (float)maxVal);
		return compressed;
	}

	private float DecompressColorValue(int val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1;
		return ((float)val / (float)maxVal);
		//return color;
	}
}