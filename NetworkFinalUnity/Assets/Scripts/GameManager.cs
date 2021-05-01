using MLAPI;
using MLAPI.Connection;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
	public void UpdateClientInput(ulong clientId, PlayerInput pIn)
	{
		if (clientId != NetworkManager.Singleton.LocalClientId)
		{
			NetworkManager.Singleton.ConnectedClients.TryGetValue(clientId, out NetworkClient nClient);
			nClient.PlayerObject.GetComponent<TestPlayerController>().SetInput(pIn);
		}
	}
	public void UpdateClientSpatial(ulong clientId, Transform pT)
	{
		if (clientId != NetworkManager.Singleton.LocalClientId)
		{
			NetworkManager.Singleton.ConnectedClients.TryGetValue(clientId, out NetworkClient nClient);
			nClient.PlayerObject.transform.position = pT.position;
			nClient.PlayerObject.transform.rotation = pT.rotation;
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
