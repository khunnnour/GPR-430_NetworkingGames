﻿using System;
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

public class NetworkedPlayerController : NetworkBehaviour
{
	// networking private
	private PlayerInput _lastInput; // most recent input
									// local private
	private Rigidbody _rb;
	private Text _infoText;
	private ulong _serverID;

	public PlayerInput LastInput => _lastInput;

	public override void NetworkStart()
	{
		GameObject.FindGameObjectWithTag("GameManager").GetComponent<NetworkedGameManager>().ReportIn(this);
	}

	private void Start()
	{
		_rb = GetComponent<Rigidbody>();
		_lastInput ^= _lastInput; // clear last input

		if (IsOwner)//if this controller is owned by the connection
		{
			//_infoText.text = "sent color request";
			NetworkInterface.Instance.SendColorRequest(_serverID);
		}

		_serverID = NetworkManager.Singleton.ServerClientId;

		//NetworkInterface.Instance.SendMapEvent(OwnerClientId, (int)OwnerClientId, 3);
	}

	private void Update()
	{
		if (IsOwner)
		{
			GetInput();
		}
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
		NetworkInterface.Instance.SendPlayerInputToServer(_serverID, NetworkObjectId, _lastInput);
	}

	// local simulation
	private void Move()
	{
		Vector3 dir = Vector3.zero;

		// get direction
		if (_lastInput.HasFlag(PlayerInput.W)) dir += Vector3.forward;
		if (_lastInput.HasFlag(PlayerInput.A)) dir += Vector3.left;
		if (_lastInput.HasFlag(PlayerInput.S)) dir += Vector3.back;
		if (_lastInput.HasFlag(PlayerInput.D)) dir += Vector3.right;

		// move in that direction
		_rb.AddForceAtPosition(dir * Time.fixedDeltaTime * 300f, transform.position + 0.5f * Vector3.up);
	}

	public void SetInput(PlayerInput pIn)
	{
		//Debug.Log("Got input set");
		_lastInput = pIn;
	}
}