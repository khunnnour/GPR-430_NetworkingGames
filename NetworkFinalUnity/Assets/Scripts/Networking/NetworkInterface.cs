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

		// process message based on size
		if (stream.Length == 2)
		{
			ReceivePlayerInput(clientid, stream);
		}
	}

	/* - private handlers for when messages are received - */
	private void ReceivePlayerInput(ulong clientid, Stream stream)
    {
		NetworkReader reader = new NetworkReader(stream);

		//reader.ReadBits(8); // skip extra byte
		//reader.SkipPadBits();
		int pIndex = reader.ReadNibble();
		PlayerInput pInput = (PlayerInput)reader.ReadNibble();

		//Debug.Log("Recieved input from p " + pIndex + ": " + pInput);
		_messageLog.text = "Recieved input from p " + pIndex + ": " + pInput;
	}
    private void ReceivePlayerSpatial(ulong clientid, Stream stream)
    {
        
    }
    private void ReceiveMapEvent(ulong clientid, Stream stream)
    {
        
    }


    /* - public functions for players to send info to server - */
    
    public void SendPlayerInput(ulong target, int index, PlayerInput input)
    {
        //Debug.Log("Attempted to send " + input + " for client " + index);

		// TODO: Remove debug overwrites
		//index = 1;
		//input = PlayerInput.W | PlayerInput.S | PlayerInput.D;

        NetworkBuffer buffer = new NetworkBuffer();
        NetworkWriter writer = new NetworkWriter(buffer);
		// pack index => 4 bits
        writer.WriteNibble((byte)index);
		// input flags => 4 bits
		writer.WriteNibble((byte)input);

		CustomMessagingManager.SendUnnamedMessage(target, buffer, NetworkChannel.DefaultMessage);
	}
    
    public void SendPlayerSpatial(int index, Vector3 pos,Quaternion rot)
    {
        // index => 2 bits
        // pos => 3*13 bits
        // rot => 3*13 bits
    }
    
    public void SendMapEvent(int index, int cell)
    {
        // index => 2 bits
        // cell => 6 bits
    }
}
