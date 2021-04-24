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

	int PlayerID; // �÷��̾� ID

	// ��ġ
	float x, y, z;

	// ȸ����
	float yaw, pitch, roll;

	// �ӵ�
	float vx, vy, vz;

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
	unsigned char m_netbuf[MAX_BUFFER]; //���� ������ �޴� ����, IOCP send/recv ����
	OP_TYPE m_op; // Send/Recv/Accept ����
};

struct SESSION
{
	int m_id;
	EX_OVER m_recv_over;
	unsigned char m_prev_recv; //���� ������, recv ���� ��ġ
	SOCKET m_s;

	bool m_ingame;
	char m_name[MAX_NAME];
	CharacterPacket character; //��������
	//Ghost ghosts;
};

void error_display(const char* msg, int err_no);

