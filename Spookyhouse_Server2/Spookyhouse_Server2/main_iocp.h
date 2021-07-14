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

	int PlayerID; // �÷��̾� ID

	// ��ġ
	short x, y, z;

	// ȸ����
	short yaw, pitch, roll;

	// �ӵ�
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
	unsigned char m_netbuf[MAX_BUFFER]; //���� ������ �޴� ����, IOCP send/recv ����
	OP_TYPE m_op; // Send/Recv/Accept ����
	SOCKET m_csocket;
};

struct SESSION
{
	int m_id;
	EX_OVER m_recv_over;
	unsigned char m_prev_recv; //���� ������, recv ���� ��ġ
	SOCKET m_s;

	//bool m_ingame;
	atomic<S_STATE>	m_state; // 0. free 1. connected 2. ingame
	mutex m_lock;
	char m_name[MAX_NAME];
	CharacterPacket character; //��������
	bool is_close_door[22];
	bool is_close_closet_left[6];
	bool is_keypad;
	bool is_escape;

	unordered_set<int> m_viewlist; //���������� �� �����Ƿ� ���
	mutex m_vl;
};

struct KeyLocation
{
	float x, y, z;
};