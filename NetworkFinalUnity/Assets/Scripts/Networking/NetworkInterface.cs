﻿using System;
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
	MAP_EVENT,
	PLAYER_COLOR
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

	public void BroadcastPlayerColor(ulong netObjId, Color color)
	{
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);

		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.PLAYER_COLOR);
		// net obj id => 4 bits
		writer.WriteNibble((byte)netObjId);
		// pos => 3*13 bits
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

		Debug.Log("Recieved input from p " + cell + ": " + netObjID);
		//_messageLog.text = "Map evt: " + cell;
	}

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


	const float posMin = -15f;
	const float posMax = 25f;
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
