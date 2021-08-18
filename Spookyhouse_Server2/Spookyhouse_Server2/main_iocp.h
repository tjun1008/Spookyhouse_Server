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
#include <queue>
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

struct MonsterPacket {

	int MonsterID; // ���� ID

	// ��ġ
	float x, y, z;

	// ȸ����
	float yaw, pitch, roll;

};

enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT, OP_NPC_MOVE};

struct EX_OVER {
	WSAOVERLAPPED m_over;
	WSABUF m_wsabuf[1];
	unsigned char m_netbuf[MAX_BUFFER]; //���� ������ �޴� ����, IOCP send/recv ����
	OP_TYPE m_op; // Send/Recv/Accept ����
	SOCKET m_csocket;
};

struct OBJECT {
	bool is_close_door[22];
	bool is_close_closet_left[6];
	bool is_close_closet_right[6];
	bool is_close_drawer[6];
	bool is_close_refriger[6];
	bool is_close_dryer[2];
	bool is_close_lockbox;
	bool is_keypad;
	bool is_keycard;
	bool is_escape;
};

struct OTHEROBJECT {

	bool is_open_coffin;
	bool get_robot;
	bool get_baby;
	bool is_open_board;
	bool is_open_safedoor;
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

	OBJECT object;
	OTHEROBJECT otherobject;
	bool escape_key[3];
	bool object_key[11];
	unordered_set<int> m_viewlist; //���������� �� �����Ƿ� ���
	mutex m_vl;
};

struct KeyLocation
{
	float x, y, z;
};

struct timer_event {
	int object_id;
	OP_TYPE event_type;
	chrono::system_clock::time_point exec_time;
	int target_id;

	constexpr bool operator < (const timer_event& l) const
	{
		return exec_time > l.exec_time;
	}
};