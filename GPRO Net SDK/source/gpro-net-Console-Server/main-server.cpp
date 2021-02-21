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

	main-server.c/.cpp
	Main source for console server application.
*/

#include "gpro-net/gpro-net.h"

// C++ Libraries
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

// our libraries
#include "gpro-net/gpro-net-common/GameRoom.h"

// RakNet Libraries
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h" // MessageID
#include "RakNet/GetTime.h"
#include "RakNet/RakPeerInterface.h"

// Defines
#define MAX_CLIENTS 10
#define SERVER_PORT 7777
#define MAX_GAME_ROOM 5

// protos
void connectionMade(std::vector<client>& c,RakNet::Packet* p);
void connectionLost(std::vector<client>& c,RakNet::Packet* p);
int findClient(std::vector<client>& c,RakNet::RakString u);

int main(int const argc, char const* const argv[])
{
	// -- Start-Up -- //
	printf("Starting the server.\n");

	// vector for connected clients
	std::vector<client> connectedClients;

	// set up peer stuff
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer = true;
	RakNet::Packet* packet;

	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);
	// We need to let the server accept incoming connections from the clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	// set up log file
	std::ofstream messLogFile;
	messLogFile.open("messages.txt",std::ios::app);

	if (!messLogFile.is_open())
		printf("!! ERROR !! File not found\n\n");
	else
		messLogFile << "START => \n";

	while (1)
	{
		// if 'T' is pressed wait for input
		if ((int)GetAsyncKeyState(VK_RCONTROL) != 0)
		{
			printf("Command: ");
			std::string str;
			std::getline(std::cin, str);
			// if input is 'q' then break form loop (quit)
			if (str == "q")
			{
				break;
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
			{
				printf("Another client has connected.\n");

			}
			break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Our connection request has been accepted.\n");
				// use a bitstream to write a custom user message
				/*RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				char str[32];
				//std::cin >> str;
				RakNet::RakString rakStr(str);
				bsOut.Write(rakStr);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);*/
			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
			{
				printf("A connection is incoming.\n");

				// send welcome message to new client
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_WELCOME_MESSAGE);
				bsOut.Write(RakNet::GetTime());
				bsOut.Write("Welcome to the server");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			}
			break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
			{
				// update connections
				connectionLost(connectedClients, packet);
			}
			break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
				}
				break;
			case ID_WELCOME_MESSAGE:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("Message: %s\n", rs.C_String());
			}
			break;
			case ID_LOGIN_MESSAGE:
			{
				// update connections
				connectionMade(connectedClients, packet);
			}
			break;
			case ID_SEND_PUBLIC_MESSAGE:
			{
				// read in all of the data
				RakNet::BitStream bsIn(packet->data, packet->length, false);

				// ignore message id
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				// read in time
				RakNet::Time inTime;
				bsIn.Read(inTime);

				// read in the user
				RakNet::RakString user;
				bsIn.Read(user);

				RakNet::RakString mess;
				bsIn.Read(mess);

				// log message
				messLogFile << (int)inTime << " - " << mess << "\n";

				// print message to server console
				printf("%d | %s => ALL: %s\n", (int)inTime, user.C_String(), mess.C_String());

				// send message to intended recipient
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_SEND_PUBLIC_MESSAGE);
				bsOut.Write(inTime);
				bsOut.Write(user);
				bsOut.Write(mess);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
			}
			break;
			case ID_SEND_PRIVATE_MESSAGE:
			{
				// read in all of the data
				RakNet::BitStream bsIn(packet->data, packet->length, false);

				// ignore message id
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				// read in time
				RakNet::Time inTime;
				bsIn.Read(inTime);

				// read in the sending user
				RakNet::RakString user;
				bsIn.Read(user);

				// read in the target user
				RakNet::RakString targUser;
				bsIn.Read(targUser);

				// read in the message
				RakNet::RakString mess;
				bsIn.Read(mess);

				// log message
				messLogFile << (int)inTime << " - " << mess << "\n";

				// print message to server console
				printf("%d | %s => %s: %s\n", (int)inTime, user.C_String(), targUser.C_String(), mess.C_String());

				// get the target
				int tUIndex = findClient(connectedClients, targUser);
				printf("Sending message to client #%d; ", tUIndex);

				if (tUIndex != -1)
				{
					// send message to intended recipient
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_SEND_PRIVATE_MESSAGE);
					bsOut.Write(RakNet::GetTime());
					bsOut.Write(user);
					bsOut.Write(mess);
					printf("Send code: %u\n",peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, connectedClients[tUIndex].address, false));
				}
				else {
					printf("User not connected\n");
				}
			}
			break;
			case ID_USER_LIST_REQUEST:
			{
				// send list of users online
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_USER_LIST_REQUEST);
				bsOut.Write(RakNet::GetTime());

				// start with first client
				RakNet::RakString users = connectedClients[0].name;
				// then ad every other one
				for (int i = 1; i < connectedClients.size(); i++)
				{
					users += ", " + connectedClients[i].name;
				}
				// write to bitstream
				bsOut.Write(users);

				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				printf("A connection is incoming.\n");
			}
			break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}

	messLogFile << "\n";
	messLogFile.close();
	peer->Shutdown(5);
	RakNet::RakPeerInterface::DestroyInstance(peer);

	printf("\n\nQuitting...\n\n");
	system("pause");
}

/// <summary>
/// Updates the list of connections when ID_NEW_INCOMING_CONNECTION message is recieved
/// </summary>
/// <param name="c">the list of connected clients</param>
/// <param name="p">packet to be processed</param>
void connectionMade(std::vector<client>& c, RakNet::Packet* p)
{
	// read in all of the data
	RakNet::BitStream bsIn(p->data, p->length, false);

	// ignore message id
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

	// read in time
	RakNet::Time inTime;
	bsIn.Read(inTime);

	// read in the user
	RakNet::RakString user;
	bsIn.Read(user);

	// print message to server console
	printf("%d | %s has connected.\n", (int)inTime, user.C_String());

	// create a new client
	client newClient;
	newClient.name = user;
	newClient.address = p->systemAddress;

	// add to client vector
	c.push_back(newClient);
}
/// <summary>
/// Updates the list of connections when ID_DISCONNECTION_NOTIFICATION message is recieved
/// </summary>
/// <param name="c">the list of connected clients</param>
/// <param name="p">packet to be processed</param>
void connectionLost(std::vector<client>& c, RakNet::Packet* p)
{
	// read in all of the data
	RakNet::BitStream bsIn(p->data, p->length, false);

	// ignore message id
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

	// read in time
	RakNet::Time inTime;
	bsIn.Read(inTime);

	// read in the user
	RakNet::RakString user;
	bsIn.Read(user);

	// print message to server console
	printf("%d | %s has disconnected.\n", (int)inTime, user.C_String());

	// remove client from list
	int index = findClient(c, user);
	if (index == -1)
		printf("!! ERROR !! - Username not found in list");
	else
		c.erase(c.begin() + index);
}

/// <summary>
/// Finds a user in the connected client list. Returns the index if they exist and -1 if not
/// </summary>
/// <param name="c">the list of connected clients</param>
/// <param name="u">the username to search for</param>
/// <returns></returns>
int findClient(std::vector<client>& c, RakNet::RakString u)
{
	// loop thru entire vector
	for (int i = 0; i < c.size(); i++)
	{
		// if client's username is the same as provided string then return index
		if (c[i].name == u)
			return i;
	}
	// else return -1
	return -1;
}
