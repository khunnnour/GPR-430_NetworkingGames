using MLAPI;
using MLAPI.Connection;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
	public float inputUpdateInterval   = 0.0333f;	// every 2 frames
	public float spatialUpdateInterval = 0.1667f;	// every 10 frames

	private List<TestPlayerController> controllers;
	private float _nextInputTime,_nextSpatialTime; // time to update respective values

	private void Awake()
	{
		controllers = new List<TestPlayerController>();
	}

	private void Start()
	{
		Debug.Log("LocalClientId: " + NetworkManager.Singleton.LocalClientId +
				"ServerClientId: " + NetworkManager.Singleton.ServerClientId);

		_nextInputTime = -1;
		_nextSpatialTime = -1;
	}

	private void Update()
	{
		if (NetworkManager.Singleton.IsServer)
			UpdateServer();
	}

	private void UpdateServer()
	{
		// check if time to update player inputs
		if (Time.time > _nextInputTime)
		{
			_nextInputTime = Time.time + inputUpdateInterval;
		}

		// check if time to update player spatials
		if (Time.time > _nextSpatialTime)
		{
			_nextSpatialTime = Time.time + spatialUpdateInterval;
		}
	}

	public void ReportIn(TestPlayerController cont)
	{
		if (!controllers.Contains(cont))
		{
			controllers.Add(cont);
			Debug.Log("Added controller for " + cont.OwnerClientId);
		}
	}

	public void UpdateClientInput(ulong netObjId, PlayerInput pIn)
	{
		//TestPlayerController controller=null;
		// get controller from list
		for(int i=0;i<controllers.Count;i++)
		{
			// check if player is still connected
			if (controllers[i])
			{
				controllers.RemoveAt(i);
				continue;
			}

			if (controllers[i].NetworkObjectId == netObjId)
			{
				//controller = controllers[i];
				controllers[i].SetInput(pIn);
			}
		}

		//// dont set actual player's input
		//if (controller.IsLocalPlayer)
		//{
		//	NetworkManager.Singleton.ConnectedClients.TryGetValue(clientId, out NetworkClient nClient);
		//	nClient.PlayerObject.GetComponent<TestPlayerController>().SetInput(pIn);
		//}
	}
	public void UpdateClientSpatial(ulong clientId, Transform pT)
	{
		NetworkManager.Singleton.ConnectedClients.TryGetValue(clientId, out NetworkClient nClient);
		nClient.PlayerObject.transform.position = pT.position;
		nClient.PlayerObject.transform.rotation = pT.rotation;
	}

	void OnGUI()
	{
		GUILayout.BeginArea(new Rect(10, 10, 300, 300));
		if (!NetworkManager.Singleton.IsClient && !NetworkManager.Singleton.IsServer)
		{
			StartButtons();
		}
		else
		{
			StatusLabels();
		}

		GUILayout.EndArea();
	}

	static void StartButtons()
	{
		if (GUILayout.Button("Client")) NetworkManager.Singleton.StartClient();
		if (GUILayout.Button("Server")) NetworkManager.Singleton.StartServer();
	}		

	static void StatusLabels()
	{
		var mode = NetworkManager.Singleton.IsServer ? "Server" : "Client";

		GUILayout.Label("Transport: " + NetworkManager.Singleton.NetworkConfig.NetworkTransport.GetType().Name);
		GUILayout.Label("Mode: " + mode);
	}
}
