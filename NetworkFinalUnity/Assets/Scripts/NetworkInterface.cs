using System;
using System.IO;
using MLAPI;
using MLAPI.Configuration;
using MLAPI.Messaging;
using MLAPI.NetworkVariable;
using MLAPI.Serialization;
using MLAPI.Transports;
using UnityEngine;
using UnityEngine.UI;

[Flags]
public enum PlayerInput
{
    W = 0,
    A = 1,
    S = 2,
    D = 4
}

public class NetworkInterface : MonoBehaviour
{
    public static NetworkInterface Instance;
    
    private NetworkConfig _config;

    private void Awake()
    {
        Instance = this;
    }

    public void Start()
    {
        _config = NetworkManager.Singleton.NetworkConfig;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    /* - private handlers for when messages are received - */
    private void ReceivePlayerInput(ulong clientid, Stream stream)
    {
        
    }
    private void ReceivePlayerSpatial(ulong clientid, Stream stream)
    {
        
    }
    private void ReceiveMapEvent(ulong clientid, Stream stream)
    {
        
    }


    /* - public functions for players to send info to server - */
    
    public void SendPlayerInput(int index, PlayerInput input)
    {
        // index => 2 bits
        // input flags => 4 bits
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
