#pragma once

class Bitstream;

class GameDataMessage
{
protected:
	// member variables
	
	int timestamp;

	// operators
	GameDataMessage operator<<(Bitstream* bs);
	GameDataMessage operator>>(Bitstream* bs);
};