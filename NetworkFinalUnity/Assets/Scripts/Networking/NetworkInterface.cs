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
	PLAYER_INPUT,
	PLAYER_SPATIAL,
	MAP_EVENT
}

public class NetworkInterface : MonoBehaviour
{
	public static NetworkInterface Instance;

	private NetworkConfig _config;
	private GameManager _manager;
	private Text _messageLog;

	private void Awake()
	{
		Instance = this;
	}

	public void Start()
	{
		_config = NetworkManager.Singleton.NetworkConfig;
		_manager = GameObject.FindGameObjectWithTag("GameManager").GetComponent<GameManager>();

		_messageLog = GameObject.FindWithTag("MessageLog").GetComponent<Text>();
		CustomMessagingManager.OnUnnamedMessage += ProcessUnnamedMessage;
	}

	// Update is called once per frame
	void Update()
	{

	}

	private void ProcessUnnamedMessage(ulong clientid, Stream stream)
	{
		Debug.Log("Unnamed Message received (" + stream.Length + " bytes) from " + clientid);

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
		}
	}

	/* - Broadcast data to all connected clients - */
	public void SendMapEvent(ulong target, int index, int cell)
	{
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);

		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.MAP_EVENT);
		// pack index => 2 bits
		writer.WriteBits((byte)index, 2);
		// cell => 6 bits
		writer.WriteBits((byte)cell, 6);

		CustomMessagingManager.SendUnnamedMessage(target, buffer, NetworkChannel.DefaultMessage);
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
	// Broadcast player spatial
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

		// get all of the client ids
		List<NetworkClient> clients = NetworkManager.Singleton.ConnectedClientsList;

		// turn to list of client ids
		List<ulong> clientIds = new List<ulong>();
		foreach (NetworkClient client in clients)
			clientIds.Add(client.ClientId);

		// send message to all connected clients
		CustomMessagingManager.SendUnnamedMessage(clientIds, buffer, NetworkChannel.DefaultMessage);

		_messageLog.text = "Sending position: " + pos.ToString("F3");
	}


	/* - private handlers for when messages are received - */
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
		int compPosComp = (int)stream.ReadBits(13);
		float posX = DecompressPositionValue(compPosComp, 13);
		compPosComp = (int)stream.ReadBits(13);
		float posY = DecompressPositionValue(compPosComp, 13);
		compPosComp = (int)stream.ReadBits(13);
		float posZ = DecompressPositionValue(compPosComp, 13);
		Vector3 pos = new Vector3(posX, posY, posZ);

		_manager.UpdateClientSpatial(netObjID, pos);

		//Debug.Log("Spatial from " + clientid + ": " + pos);
		_messageLog.text = "Spatial for " + netObjID + ": " + pos.ToString("F3");
	}
	private void ReceiveMapEvent(ulong clientid, NetworkReader stream)
	{
		//NetworkReader reader = new NetworkReader(stream);

		int pInd = (int)stream.ReadBits(2);
		int cell = (int)stream.ReadBits(6);

		//Debug.Log("Recieved input from p " + pIndex + ": " + pInput);
		_messageLog.text = "Map evt: " + cell;
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

	//public void SendPlayerSpatialToServer(ulong target, ulong netObjId, Transform t)
	//{
	//	NetworkBuffer buffer = new NetworkBuffer();
	//	NetworkWriter writer = new NetworkWriter(buffer);
	//
	//	// message type => 4 bits
	//	writer.WriteNibble((byte)MessageType.PLAYER_SPATIAL);
	//	// net obj id => 4 bits
	//	writer.WriteNibble((byte)netObjId);
	//	// pos => 3*13 bits
	//	Vector3 pos = t.position;
	//	writer.WriteBits(CompressPositionValue(pos.x, 13), 13);
	//	writer.WriteBits(CompressPositionValue(pos.y, 13), 13);
	//	writer.WriteBits(CompressPositionValue(pos.z, 13), 13);
	//	// rot => 3*13 bits
	//
	//	CustomMessagingManager.SendUnnamedMessage(target, buffer, NetworkChannel.DefaultMessage);
	//}


	const float posMin = -10f;
	const float posMax = 10f;
	const float posRange = posMax- posMin;
	private ulong CompressPositionValue(float val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1;
		float serial = (val- posMin); // get make so min value is 0
		float ratio = serial / posRange; // divide by whole range
		ulong compressed=(ulong)Mathf.RoundToInt(ratio * (float)maxVal); // convert to integer
		return compressed;
	}
	private float DecompressPositionValue(int val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1;
		float ratio = (float)val / (float)maxVal; // get the ratio
		float serial = ratio * posRange; // get the serialized value
		float ret = serial + posMin; // convert back to actual value
		return ret;
	}

	//const float rotMax = 1f;
	private ulong CompressRotationValue(float val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1;
		ulong compressed = (ulong)Mathf.RoundToInt(val * (float)maxVal);
		return compressed;
	}
	private float DecompressRotationValue(int val, int bits)
	{
		int maxVal = (2 << (bits - 1)) - 1;
		float ratio = (float)val / (float)maxVal;
		return ratio;
	}
}
