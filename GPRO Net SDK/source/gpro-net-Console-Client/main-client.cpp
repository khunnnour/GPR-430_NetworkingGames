/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	main-client.c/.cpp
	Main source for console client application.
*/

#include "gpro-net/gpro-net.h"


// C++ Libraries
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
// RakNet Libraries
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h" // MessageID
#include "RakNet/GetTime.h"
#include "RakNet/RakPeerInterface.h"
//#include "RakNet/MessageIdentifiers.h"

// Defines
#define SERVER_PORT 7777

// prototypes
// TODO: send server message proto

//enum GameMessages
//{
//	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
//};

int main(int const argc, char const* const argv[])
{
	RakNet::RakString username ="DEFAULT";
	bool connected=false;

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	RakNet::Packet* packet;

	RakNet::SocketDescriptor sd;
	peer->Startup(1, &sd, 1);
	isServer = false;

	printf("Starting the client.\n");
	peer->Connect("172.16.2.57:4024", SERVER_PORT, 0, 0);
	RakNet::SystemAddress sysAdd = RakNet::SystemAddress("172.16.2.57:4024", SERVER_PORT);

	while (1)
	{
		// TODO: add for button presses to ask for input
		// Get user message
		if (connected)
		{
			printf("Message: ");
			std::string str;
			std::getline(std::cin, str);
			// send the user message if not empty string
			if (str != "")
			{
				if (str == "/d") 
				{
					RakNet::BitStream bsOut;
					// write mId and timestamp to bitstream
					bsOut.Write((RakNet::MessageID)ID_SEND_PRIVATE_MESSAGE);
					bsOut.Write(RakNet::GetTime());
					// write username to the bitstream
					bsOut.Write(username);

					// get target username
					std::string tUser;
					printf("Send to: ");
					std::getline(std::cin, tUser);
					bsOut.Write(tUser.c_str());

					// get message
					std::string mess;
					printf("Send what to %s: ", tUser.c_str());
					std::getline(std::cin, mess);
					bsOut.Write(mess.c_str());

					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, sysAdd, false);
				}
				else 
				{
					RakNet::BitStream bsOut;
					// write mId and timestamp to bitstream
					bsOut.Write((RakNet::MessageID)ID_SEND_PUBLIC_MESSAGE);
					bsOut.Write(RakNet::GetTime());
					// write username to the bitstream
					bsOut.Write(username);
					// write message to bitstream
					bsOut.Write(str.c_str());

					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, sysAdd, false);
				}
			}
		}

		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{	
				printf("Our connection request has been accepted.\n");

				RakNet::BitStream bsOut;
				// write mId and timestamp to bitstream
				bsOut.Write((RakNet::MessageID)ID_LOGIN_MESSAGE);
				bsOut.Write(RakNet::GetTime());
				// get a username
				printf("Username: ");
				std::string str;
				std::getline(std::cin, str);
				username = str.c_str();
				// write username to bitstream
				bsOut.Write(username);

				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				connected = true;
			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected.\n");
					connected = false;
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
					connected = false;
				}
				break;
			case ID_WELCOME_MESSAGE:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				
				// - recieve welcome message - //
				// ignore message id
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				
				// read in time
				RakNet::Time inTime;
				bsIn.Read(inTime);
				
				// read in the message
				bsIn.Read(rs);
				
				printf("%d >  %s\n", (int)inTime, rs.C_String());
			}
			break;
			case ID_SEND_PRIVATE_MESSAGE:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);

				// - recieve welcome message - //
				// ignore message id
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				// read in time
				RakNet::Time inTime;
				bsIn.Read(inTime);

				// read in username
				RakNet::RakString user;
				bsIn.Read(user);

				// read in the message
				bsIn.Read(rs);

				printf("%d > %s: %s\n", (int)inTime, user.C_String(), rs.C_String());
			}
			break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}

	RakNet::RakPeerInterface::DestroyInstance(peer);

	printf("\n\n");
	system("pause");
}
