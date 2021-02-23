#pragma once

/*
	By Conner Root

	GameRoom.h
	Holds base classes for the game room.
*/

#include "gpro-net/gpro-net.h"
#include "gpro-net-gamestate.h"

// C++ Libraries
#include <vector>

// RakNet Libraries
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h" // MessageID
#include "RakNet/GetTime.h"
#include "RakNet/RakPeerInterface.h"


// client struct
struct client {
	RakNet::RakString name;
	RakNet::SystemAddress address;
};

class GameRoom {
public:
	// read only members for the gametype of the room and its id
	static game_type gameType;
	static int roomID;

	// contructors
	GameRoom();
	GameRoom(game_type,int);

protected:
	// 2 slot array for the plays
	client players[2];
	// vector for all spectators
	std::vector<client> spectators;
};

class BattleshipRoom : public GameRoom
{
public:
	BattleshipRoom(int);

protected:
	void Move(int player, int tX, int tY);

	void ResetBoard(int player);
	void SetShip(int player, int orX, int orY, int dir, int len);

	int IsValidPlacement(int player, int orX, int orY, int dir, int len);

private:
	gpro_battleship pBoards[2];
};