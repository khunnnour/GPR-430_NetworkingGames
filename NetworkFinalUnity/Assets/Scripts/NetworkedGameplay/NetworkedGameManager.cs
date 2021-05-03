using MLAPI;
using MLAPI.Connection;
using System.Collections.Generic;
using UnityEngine;

public class NetworkedGameManager : MonoBehaviour
{
	public float inputUpdateInterval = 0.0500f; // every 3  frames at 60fps
	public float spatialUpdateInterval = 0.5000f;   // every 30 frames at 60fps

	private List<NetworkedPlayerController> controllers;
	private float _nextInputTime, _nextSpatialTime; // time to update respective values

	//public GameObject gridBuilder;
	public GridBuilder builderScript;

	private void Awake()
	{
		controllers = new List<NetworkedPlayerController>();
		Application.targetFrameRate = 60;
	}

	private void Start()
	{
		//Debug.Log("LocalClientId: " + NetworkManager.Singleton.LocalClientId +
		//		"ServerClientId: " + NetworkManager.Singleton.ServerClientId);
		_nextInputTime = -1;
		_nextSpatialTime = -1;
	}

	private void Update()
	{
		if (NetworkManager.Singleton.IsServer) // only update server if server
			UpdateServer();
	}

	// check if time to synchronize clients
	private void UpdateServer()
	{
		// check if time to update player inputs
		if (Time.time > _nextInputTime)
		{
			//Debug.Log("Broadcasted inputs to clients");

			// broadcast last input for all controllers connected
			foreach (NetworkedPlayerController controller in controllers)
				NetworkInterface.Instance.BroadcastPlayerInput(controller.NetworkObjectId, controller.LastInput);

			_nextInputTime = Time.time + inputUpdateInterval;
		}

		// check if time to update player spatials
		if (Time.time > _nextSpatialTime)
		{
			// broadcast current spatial for all controllers
			foreach (NetworkedPlayerController controller in controllers)
				NetworkInterface.Instance.BroadcastPlayerSpatial(controller.NetworkObjectId, controller.transform);

			_nextSpatialTime = Time.time + spatialUpdateInterval;
		}
	}

	// for controllers to report in to the gamemanager when they are created
	public void ReportIn(NetworkedPlayerController cont)
	{
		if (!controllers.Contains(cont))
		{
			controllers.Add(cont);
			//Debug.Log("Added controller for " + cont.OwnerClientId);
		}
	}

	public void UpdateClientInput(ulong netObjId, PlayerInput pIn)
	{
		//Debug.Log("Updating input for net object " + netObjId + " in list of " + controllers.Count);
		// get controller from list
		for (int i = 0; i < controllers.Count; i++)
		{
			// check if player is still connected
			if (!controllers[i])
			{
				// remove it from the list and skip
				controllers.RemoveAt(i);
				continue;
			}

			// set last input for the correct controller (not self)
			if (controllers[i].OwnerClientId != NetworkManager.Singleton.LocalClientId && controllers[i].NetworkObjectId == netObjId)
			{
				controllers[i].SetInput(pIn);
			}
		}
	}
	public void UpdateClientSpatial(ulong netObjId, Vector3 pos, Quaternion rot)
	{
		//Debug.Log("Updating spatial for net object " + netObjId + " in list of " + controllers.Count);
		// get controller from list
		for (int i = 0; i < controllers.Count; i++)
		{
			// check if player is still connected
			if (!controllers[i])
			{
				// remove it from the list and skip
				controllers.RemoveAt(i);
				continue;
			}

			// set last input for the correct controller
			if (controllers[i].NetworkObjectId == netObjId)
			{
				controllers[i].transform.position = pos;
				controllers[i].transform.rotation = rot;
			}
		}

		//NetworkManager.Singleton.ConnectedClients.TryGetValue(clientId, out NetworkClient nClient);
		//nClient.PlayerObject.transform.position = pT.position;
		//nClient.PlayerObject.transform.rotation = pT.rotation;
	}
	public void UpdateClientMap(ulong netObjId, int index) 
	{
		for(int i = 0; i < controllers.Count; i++)
		{
			// check if player is still connected
			if (!controllers[i])
			{
				// remove it from the list and skip
				controllers.RemoveAt(i);
				continue;
			}

			// set last input for the correct controller
			if (controllers[i].NetworkObjectId == netObjId)
			{
				Debug.Log("UpdateClientMap");
				builderScript.UpdateGrid(controllers[i].GetComponent<PlayerScript>(), index);
			}
		}
	}

	public void UpdateClientColor(ulong netObjId, Color color)
	{
		//Debug.Log("Updating spatial for net object " + netObjId + " in list of " + controllers.Count);
		// get controller from list
		for (int i = 0; i < controllers.Count; i++)
		{
			// check if player is still connected
			if (!controllers[i])
			{
				// remove it from the list and skip
				controllers.RemoveAt(i);
				continue;
			}

			// set last input for the correct controller
			if (controllers[i].NetworkObjectId == netObjId)
			{
				controllers[i].GetComponent<PlayerScript>().generatedColor = color;
				//controllers[i].GetComponent<PlayerScript>().playerColor = color;
				controllers[i].GetComponent<PlayerScript>().UpdateColor();
				//controllers[i].transform.position = pos;
				//controllers[i].transform.rotation = rot;
			}
		}
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
