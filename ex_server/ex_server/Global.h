#pragma once

#include "Player.h"

constexpr short SERVER_PORT = 9990;
constexpr int NUM_THREADS = 4;

struct KeyLocation
{
	float x, y, z;

	KeyLocation();

	KeyLocation(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

};

vector<KeyLocation> EscapeKey;

extern  array <Object*, MAX_USER + 1> players;

int cur_player = 0;
bool bIsgameStart = false;