/*
	By Conner Root

	GameRoom.h
	Holds base classes for the game room.
*/

#include "gpro-net/gpro-net-common/GameRoom.h"

// --- for base class --- //
GameRoom::GameRoom()
{
	gameType = game_type::INVALID_GAME;
	roomID = -1;
	moveNum = 0;
}
GameRoom::GameRoom(game_type gT, int rID)
{
	gameType = gT;
	roomID = rID;
	moveNum = 0;
}


// -- for battleship room -- //
BattleshipRoom::BattleshipRoom(int rID) :GameRoom(BATTLESHIP, rID)
{
	// initialize the player boards
	ResetBoard(0);
	ResetBoard(1);
}

void BattleshipRoom::Move(int player, int tX, int tY)
{

}

void BattleshipRoom::ResetBoard(int player)
{
	gpro_battleship_reset(pBoards[player]);
}

void BattleshipRoom::SetShip(int player, int orX, int orY, int dir, int len)
{
	if (IsValidPlacement(player, orX,  orY,  dir,  len))
	{
		// GET THE DIRECTION COMPONENTS
		// delta x and y
		int dX, dY;
		switch (dir)
		{
		case 0: // up
		{
			dX = 0;
			dY = 1;
		}
		break;
		case 1: // right
		{
			dX = 1;
			dY = 0;
		}
		break;
		case 2: // down
		{
			dX = 0;
			dY = -1;
		}
		break;
		case 3: // left
		default:
		{
			dX = -1;
			dY = 0;
		}
		}

		// Check [len] slots in given direction
		int currX = orX;
		int currY = orY;
		for (int i = 0; i < len; i++)
		{
			// have already checked if this is valid placement
			// set location to there is a ship there
			pBoards[player][currX][currY] = gpro_battleship_ship;

			// shift the current coord in direction
			currX += dX;
			currY += dY;
		}
	}
}

// Returns: 0 if valid; 1 if out of bounds; 2 if overlap
int BattleshipRoom::IsValidPlacement(int player, int orX, int orY, int dir, int len)
{
	// GET THE DIRECTION COMPONENTS
	// delta x and y
	int dX, dY;
	switch (dir)
	{
	case 0: // up
	{
		dX = 0;
		dY = 1;
	}
	break;
	case 1: // right
	{
		dX = 1;
		dY = 0;
	}
	break;
	case 2: // down
	{
		dX = 0;
		dY = -1;
	}
	break;
	case 3: // left
	default:
	{
		dX = -1;
		dY = 0;
	}
	}

	// Check [len] slots in given direction
	int currX = orX;
	int currY = orY;
	for (int i = 0; i < len; i++)
	{
		// check if current point is in bounds
		if ((currX > 9 || currX < 0) || (currY > 9 || currY < 0))
			return 1;

		// check if slot is full
		if (pBoards[player][currX][currY] != gpro_battleship_open)
			return 2;

		currX += dX;
		currY += dY;
	}

	// if able to get thru whole loop, then no conflicts!
	return 0;
}