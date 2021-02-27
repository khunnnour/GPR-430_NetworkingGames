#pragma once

#include "gpro-net/gpro-net.h"

#include "RakNet/RakNetTypes.h" // MessageID

class BitStream;

class GameDataMessage
{
public:
	GameDataMessage(RakNet::MessageID mID) { messageID = mID; }

protected:
	// member variables
	RakNet::MessageID messageID;
};