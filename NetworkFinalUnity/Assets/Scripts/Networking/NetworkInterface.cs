using System;
using System.IO;
using MLAPI;
using MLAPI.Configuration;
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
	private Text _messageLog;

	private void Awake()
	{
		Instance = this;
	}

	public void Start()
	{
		_config = NetworkManager.Singleton.NetworkConfig;
		_messageLog = GameObject.FindWithTag("MessageLog").GetComponent<Text>();
		CustomMessagingManager.OnUnnamedMessage += ProcessUnnamedMessage;
	}

	// Update is called once per frame
	void Update()
	{

	}

	private void ProcessUnnamedMessage(ulong clientid, Stream stream)
	{
		Debug.Log("Unnamed Message received (" + stream.Length + " bytes)");

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

	/* - private handlers for when messages are received - */
	private void ReceivePlayerInput(ulong clientid, NetworkReader stream)
	{
		//NetworkReader reader = new NetworkReader(stream);

		int pIndex = stream.ReadNibble();
		PlayerInput pInput = (PlayerInput)stream.ReadNibble();

		//Debug.Log("Recieved input from p " + pIndex + ": " + pInput);
		_messageLog.text = "Recieved input from p " + pIndex + ": " + pInput;
	}
	private void ReceivePlayerSpatial(ulong clientid, NetworkReader stream)
	{
		// read in player index
		int pIndex = (int)stream.ReadBits(2);
		// read in position
		int compPosComp = (int)stream.ReadBits(13);
		float posX = DecompressPositionValue(compPosComp, 12);

		//Debug.Log("Recieved input from p " + pIndex + ": " + pInput);
		_messageLog.text = "Recieved spatial from p " + pIndex + ": " + posX;
	}
	private void ReceiveMapEvent(ulong clientid, NetworkReader stream)
	{
		//NetworkReader reader = new NetworkReader(stream);

		int pInd = (int)stream.ReadBits(2);
		int cell = (int)stream.ReadBits(6);

		//Debug.Log("Recieved input from p " + pIndex + ": " + pInput);
		_messageLog.text = "Recieved map evt from p " + pInd + ": " + cell;
	}


	/* - public functions for players to send info to server - */
	public void SendPlayerInput(ulong target, int index, PlayerInput input)
	{
		//Debug.Log("Attempted to send " + input + " for client " + index);

		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);
		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.PLAYER_INPUT);
		// index => 4 bits
		writer.WriteNibble((byte)index);
		// input flags => 4 bits
		writer.WriteNibble((byte)input);

		CustomMessagingManager.SendUnnamedMessage(target, buffer, NetworkChannel.DefaultMessage);
	}

	public void SendPlayerSpatial(ulong target, int index, Transform t)
	{
		NetworkBuffer buffer = new NetworkBuffer();
		NetworkWriter writer = new NetworkWriter(buffer);

		// message type => 4 bits
		writer.WriteNibble((byte)MessageType.PLAYER_SPATIAL);
		// pack index => 2 bits
		writer.WriteBits((byte)index, 2);
		// pos => 3*13 bits
		writer.WriteBits(CompressPositionValue(37.1f, 12), 13);
		// rot => 3*13 bits

		CustomMessagingManager.SendUnnamedMessage(target, buffer, NetworkChannel.DefaultMessage);
	}

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

	const float posMax = 50f;
	private ulong CompressPositionValue(float val, int bits)
	{
		int maxVal = (2 << bits) - 1;
		float ratio = val / posMax;
		ulong compressed=(ulong)Mathf.RoundToInt(ratio * (float)maxVal);
		return compressed;
	}
	private float DecompressPositionValue(int val, int bits)
	{
		int maxVal = (2 << bits) - 1;
		float ratio = (float)val / (float)maxVal;
		return ratio * posMax;
	}

	//const float rotMax = 1f;
	private ulong CompressRotationValue(float val, int bits)
	{
		int maxVal = (2 << bits) - 1;
		ulong compressed = (ulong)Mathf.RoundToInt(val * (float)maxVal);
		return compressed;
	}
	private float DecompressRotationValue(int val, int bits)
	{
		int maxVal = (2 << bits) - 1;
		float ratio = (float)val / (float)maxVal;
		return ratio;
	}
}
