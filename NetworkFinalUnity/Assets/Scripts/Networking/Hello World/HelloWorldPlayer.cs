using System.IO;
using MLAPI;
using MLAPI.Configuration;
using MLAPI.Messaging;
using MLAPI.NetworkVariable;
using MLAPI.Serialization;
using MLAPI.Transports;
using UnityEngine;
using UnityEngine.UI;
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

        public override void NetworkStart()
        {
            Debug.Log("network start");
            _config = NetworkManager.Singleton.NetworkConfig;
            //CustomMessagingManager.OnUnnamedMessage += MessageDelegate;
        }

        private void Start()
        {
            Debug.Log("start");
            _messageLog = GameObject.FindWithTag("MessageLog").GetComponent<Text>();
            Move();
        }

        private void MessageDelegate(ulong clientid, Stream stream)
        {
            Debug.Log("Unnamed Message received");

            NetworkReader reader = new NetworkReader(stream);

            string message = reader.ReadString().ToString();

            _messageLog.text = Time.time+" - Received message: " + message;
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
                NetworkBuffer buffer = new NetworkBuffer();
                NetworkWriter writer = new NetworkWriter(buffer);
                writer.WriteString("Hello Server");
                
                CustomMessagingManager.SendUnnamedMessage(OwnerClientId, buffer,
                    NetworkChannel.DefaultMessage);
                Debug.Log("Message sent");
            }
        }

        [ServerRpc]
        void SubmitPositionRequestServerRpc(ServerRpcParams rpcParams = default)
        {
            Position.Value = GetRandomPositionOnPlane();
        }

        [ServerRpc]
        void SendTextMessageServerRpc(string message, ServerRpcParams rpcParams = default)
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