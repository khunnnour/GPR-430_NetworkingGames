#pragma once

/*
	By Conner Root

	GameRoom.h
	Holds base classes for the game room.
*/

#include "gpro-net/gpro-net.h"

#include "Raknet/RakString.h"

// client struct
struct client {
	RakNet::RakString name;
	RakNet::SystemAddress address;
};

class GameRoom {
public:
	const GameType gameType;


};