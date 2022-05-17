#pragma once

#include "Player.h" //Object
//#include "Object.h"


//Player players[MAX_USER + 1];

extern HANDLE h_iocp;
extern  array <Object*, MAX_USER + 1> players;
//array <Player, MAX_USER + 1> players;

SOCKET listenSocket;
int cur_player = 0;
bool bIsgameStart = false;

KeyLocation firstf_key, bf_key, secf_key;

struct KeyLocation
{
	float x, y, z;
};



