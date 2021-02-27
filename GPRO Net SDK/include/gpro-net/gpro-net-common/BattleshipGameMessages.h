#pragma once

#include "GameMessage.h"

class BattleshipPlaceShipMessage :GameDataMessage
{
public:
	// creation
	BattleshipPlaceShipMessage();

	void Init(int, int, int, int, int, int);

	// getters
	int TimeStamp() { return timestamp; }
	int Player() { return player; }
	int OrX() { return orX; }
	int OrY() { return orY; }
	int Dir() { return dir; }
	int Len() { return len; }

protected:
	int timestamp;
	int player;
	int orX;
	int orY;
	int dir;
	int len;

	friend RakNet::BitStream& operator<<(BattleshipPlaceShipMessage mess, RakNet::BitStream& bs);
	friend BattleshipPlaceShipMessage operator>>(RakNet::BitStream bs, BattleshipPlaceShipMessage mess);
};