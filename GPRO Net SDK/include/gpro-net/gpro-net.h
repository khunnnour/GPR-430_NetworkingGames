/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	gpro-net.h
	Main include for framework.
*/

/* *
 * PACKET DATA ORDER (^ = required):
 * MESSAGE_ID^
 * TIMESTAMP^
 * USER^
 * OTHER DATA
 * */

#ifndef _GPRO_NET_H_
#define _GPRO_NET_H_

#include "RakNet/MessageIdentifiers.h"

enum GameMessages
{
	ID_WELCOME_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_LOGIN_MESSAGE,
	ID_SEND_PUBLIC_MESSAGE,
	ID_SEND_PRIVATE_MESSAGE,
	ID_USER_LIST_REQUEST
};

enum GameType
{
	INVALID_GAME=-1,
	BATTLESHIP=0,
	CHECKERS
};

#endif	// !_GPRO_NET_H_