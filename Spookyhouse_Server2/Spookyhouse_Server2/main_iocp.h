#pragma once

#include "protocol.h"

#include <iostream>
#include <map>
using namespace std;
#include <WS2tcpip.h>
#include <MSWSock.h>



#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"MSWSock.lib")

struct CharacterPacket {

	int PlayerID; // 플레이어 ID

	// 위치
	short x, y, z;

	// 회전값
	short yaw, pitch, roll;

	// 속도
	short vx, vy, vz;

	// etc
	int hp;
	int skill_gage;
	bool isalive = true;

	SOCKET sock;

};

enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT };

struct EX_OVER {
	WSAOVERLAPPED m_over;
	WSABUF m_wsabuf[1];
	unsigned char m_netbuf[MAX_BUFFER]; //실제 데이터 받는 버퍼, IOCP send/recv 버퍼
	OP_TYPE m_op; // Send/Recv/Accept 구별
};

struct SESSION
{
	int m_id;
	EX_OVER m_recv_over;
	unsigned char m_prev_recv; //남은 데이터, recv 시작 위치
	SOCKET m_s;

	bool m_ingame;
	char m_name[MAX_NAME];
	CharacterPacket character; //수정예정
};

void error_display(const char* msg, int err_no);

