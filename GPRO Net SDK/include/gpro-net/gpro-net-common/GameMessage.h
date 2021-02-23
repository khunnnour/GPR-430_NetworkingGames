#pragma once

class Bitstream;

class GameDataMessage 
{
protected:
	GameDataMessage operator<<(Bitstream* bs);
	GameDataMessage operator>>(Bitstream* bs);
};