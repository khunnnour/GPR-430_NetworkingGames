using System;
using System.IO;
using System.Text;
using MLAPI;
using MLAPI.Configuration;
using MLAPI.Messaging;
using MLAPI.NetworkVariable;
using MLAPI.Serialization;
using MLAPI.Transports;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.UIElements;
using Random = UnityEngine.Random;

namespace HelloWorld
{
    public class HelloWorldPlayer : NetworkBehaviour
    {
        public NetworkVariableVector3 Position = new NetworkVariableVector3(new NetworkVariableSettings
        {
            WritePermission = NetworkVariablePermission.ServerOnly,
            ReadPermission = NetworkVariablePermission.Everyone
        });

        private Text _messageLog;
        private NetworkConfig _config;
        private CustomMessagingManager.UnnamedMessageDelegate _messageDelegate;

        private void Start()
        {
            _messageLog = GameObject.FindWithTag("MessageLog").GetComponent<Text>();
        }

        public override void NetworkStart()
        {
            Move();
            _config = NetworkManager.Singleton.NetworkConfig;
            _messageDelegate = MessageDelegate;
        }

        private void MessageDelegate(ulong clientid, Stream stream)
        {
            byte[] message = new byte[] { };
            
            stream.Read(message, 0, 8);
            _messageLog.text = "Received message: " + message;
            Debug.Log("Message recieved");
        }

        public void Move()
        {
            if (NetworkManager.Singleton.IsServer)
            {
                var randomPosition = GetRandomPositionOnPlane();
                transform.position = randomPosition;
                Position.Value = randomPosition;
            }
            else
            {
                //SubmitPositionRequestServerRpc();
                //SendTextMessageServerRpc("hello server");
                byte[] message = Encoding.ASCII.GetBytes("hello server");
                NetworkBuffer buffer = new NetworkBuffer(message);
                CustomMessagingManager.SendUnnamedMessage(_config.NetworkTransport.ServerClientId, buffer,
                    NetworkChannel.DefaultMessage);
            }
        }

        [ServerRpc]
        void SubmitPositionRequestServerRpc(ServerRpcParams rpcParams = default)
        {
            Position.Value = GetRandomPositionOnPlane();
        }

        [ServerRpc]
        void SendTextMessageServerRpc(string message,ServerRpcParams rpcParams = default)
        {
            _messageLog.text = "Received message: " + message;
        }
        
        static Vector3 GetRandomPositionOnPlane()
        {
            return new Vector3(Random.Range(-4f, 4f), 1f, Random.Range(-4f, 4f));
        }

        void Update()
        {
            transform.position = Position.Value;
        }
    }
}