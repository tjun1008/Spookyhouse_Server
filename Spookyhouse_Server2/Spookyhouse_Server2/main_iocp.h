#pragma once

#include "protocol.h"

#include <iostream>
#include <array>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <fstream>
#include <time.h>
#include <stdlib.h>

using namespace std;

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"MSWSock.lib")

constexpr int NUM_THREADS = 4;

enum S_STATE { STATE_FREE, STATE_CONNECTED, STATE_INGAME };

struct CharacterPacket {

	int PlayerID; // 플레이어 ID

	// 위치
	short x, y, z;

	// 회전값
	short yaw, pitch, roll;

	// 속도
	short vx, vy, vz;

	//lobby
	bool host;
	bool ready;


	// etc
	bool flashlight;
	
};

enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT };

struct EX_OVER {
	WSAOVERLAPPED m_over;
	WSABUF m_wsabuf[1];
	unsigned char m_netbuf[MAX_BUFFER]; //실제 데이터 받는 버퍼, IOCP send/recv 버퍼
	OP_TYPE m_op; // Send/Recv/Accept 구별
	SOCKET m_csocket;
};

struct SESSION
{
	int m_id;
	EX_OVER m_recv_over;
	unsigned char m_prev_recv; //남은 데이터, recv 시작 위치
	SOCKET m_s;

	//bool m_ingame;
	atomic<S_STATE>	m_state; // 0. free 1. connected 2. ingame
	mutex m_lock;
	char m_name[MAX_NAME];
	CharacterPacket character; //수정예정
	bool is_close_door[22];
	bool is_close_closet_left[6];
	bool is_keypad;
	bool is_escape;

	unordered_set<int> m_viewlist; //순서없을때 더 빠르므로 사용
	mutex m_vl;
};

struct KeyLocation
{
	float x, y, z;
};