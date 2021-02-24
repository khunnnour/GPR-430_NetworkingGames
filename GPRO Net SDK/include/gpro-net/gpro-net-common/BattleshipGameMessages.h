#pragma once

#include "GameMessage.h"

class Bitstream;

class BattleshipPlaceShipMessage :GameDataMessage
{
private:
	int player;
	int orX;
	int orY;
	int dir;
	int len;

protected:
	GameDataMessage operator<<(Bitstream* bs);
	GameDataMessage operator>>(Bitstream* bs);
};