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
	static GameType gameType;
	static int roomID;

	GameRoom();
	GameRoom(GameType,int);

protected:
	client players[2];
	std::vector<client> spectators;

	virtual void Move(int player) {};
};

class BattleshipRoom : public GameRoom
{
public:
	BattleshipRoom(GameType, int);

protected:
	void Move(int player, int tX, int tY);

	void ResetBoard(int player);
	void SetShip(int player, int orX, int orY, int dir, int len);

	int IsValidPlacement(int player, int orX, int orY, int dir, int len);

private:
	gpro_battleship pBoards[2];
};