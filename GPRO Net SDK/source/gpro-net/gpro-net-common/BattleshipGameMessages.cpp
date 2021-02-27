/*
	By Conner Root

	BattleshipGameMessages.cpp
	Holds class definitions for the battleship message data classes
*/

#include "gpro-net/gpro-net-common/BattleshipGameMessages.h"

#include "RakNet/BitStream.h"
#include "RakNet/GetTime.h"

BattleshipPlaceShipMessage::BattleshipPlaceShipMessage() :GameDataMessage(GameMessages::ID_BS_PLACE_SHIP)
{
	timestamp = -1;
	player = -1;
	orX = -1;
	orY = -1;
	dir = -1;
	len = -1;
}

void BattleshipPlaceShipMessage::Init(int tS, int plyr, int oX, int oY, int d, int l)
{
	timestamp = tS;
	player = plyr;
	orX = oX;
	orY = oY;
	dir = d;
	len = l;
}

RakNet::BitStream& operator<<(BattleshipPlaceShipMessage mess, RakNet::BitStream& bs)
{
	bs.Reset();
	bs.Write(mess.messageID);		// write the message id
	bs.Write(RakNet::GetTime());	// write timestamp
	bs.Write(mess.player);			// write player
	bs.Write(mess.orX);				// place origin x
	bs.Write(mess.orY);				// place origin y
	bs.Write(mess.dir);				// ship direction
	bs.Write(mess.len);				// ship length

	return bs;
}

BattleshipPlaceShipMessage operator>>(RakNet::BitStream bs, BattleshipPlaceShipMessage mess)
{
	bs.IgnoreBytes(sizeof(RakNet::MessageID)); // ignore message id

	bs.Read(mess.timestamp);
	bs.Read(mess.player);
	bs.Read(mess.orX);
	bs.Read(mess.orY);
	bs.Read(mess.dir);
	bs.Read(mess.len);

	return mess;
}