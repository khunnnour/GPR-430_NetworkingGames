using System;
using System.IO;
using MLAPI;
using MLAPI.Configuration;
using MLAPI.Connection;
using MLAPI.Messaging;
using MLAPI.NetworkVariable;
using MLAPI.Serialization;
using MLAPI.Transports;
using UnityEngine;
using UnityEngine.UI;
using Random = UnityEngine.Random;

public class TestPlayerController : NetworkBehaviour
{
    // networking private
    private PlayerInput _lastInput; // most recent input
    // local private
    private Rigidbody _rb;

    public override void NetworkStart()
    {
    }

    private void Start()
    {
        _rb = GetComponent<Rigidbody>();
        _lastInput ^= _lastInput; // clear last input
		//NetworkInterface.Instance.SendMapEvent(OwnerClientId, (int)OwnerClientId, 3);
    }

    private void Update()
    {
        GetInput();
    }

    private void FixedUpdate()
    {
        Move();
    }
    
    private void GetInput()
    {
        // clear out _lastInput by XOR-ing
        _lastInput ^= _lastInput;
        // set flags based on if input is down
        if (Input.GetKey(KeyCode.W)) _lastInput |= PlayerInput.W;
        if (Input.GetKey(KeyCode.A)) _lastInput |= PlayerInput.A;
        if (Input.GetKey(KeyCode.S)) _lastInput |= PlayerInput.S;
        if (Input.GetKey(KeyCode.D)) _lastInput |= PlayerInput.D;
        // send player input info to server
        //NetworkInterface.Instance.SendPlayerInput(OwnerClientId,(int)OwnerClientId, _lastInput);
    }
    
    // local simulation
    private void Move()
    {
        if (!NetworkManager.Singleton.IsServer)
        {
            Vector3 dir=Vector3.zero;
            
            // get direction
            if (_lastInput.HasFlag(PlayerInput.W)) dir += Vector3.forward;
            if (_lastInput.HasFlag(PlayerInput.A)) dir += Vector3.left;
            if (_lastInput.HasFlag(PlayerInput.S)) dir += Vector3.back;
            if (_lastInput.HasFlag(PlayerInput.D)) dir += Vector3.right;
            
            // move in that direction
            _rb.MovePosition(transform.position + dir * Time.fixedDeltaTime);
        }
    }
}