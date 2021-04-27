#pragma once

#include <iostream>
#include <map>
using namespace std;
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "protocol.h"


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"MSWSock.lib")


void error_display(const char* msg, int err_no);



enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT };

struct CharacterPacket {

	int PlayerID; // 플레이어 ID

	// 위치
	float x, y, z;

	// 회전값
	float yaw, pitch, roll;

	// 속도
	float vx, vy, vz;

	// etc
	int hp;
	int skill_gage;
	bool isalive = true;

	SOCKET sock;

};

class Ghost
{
public:
	Ghost();
	virtual ~Ghost();

	// 플레이어로 이동
	void MoveTo(const CharacterPacket& target);
	// 플레이어 타격
	void HitPlayer(CharacterPacket& target);
	// 살아있는지 여부
	bool IsAlive();
	// 공격중인지 여부
	bool IsAttacking();
	// 플레이어가 추격 범위에 있는지
	bool IsPlayerInTraceRange(const CharacterPacket& target);
	// 플레이어가 타격 범위에 있는지
	bool IsPlayerInHitRange(const CharacterPacket& target);
	// 위치 설정
	void SetLocation(float x, float y, float z);


	float	X;				// X좌표
	float	Y;				// Y좌표
	float	Z;				// Z좌표
	float	Health;			// 체력
	int		Id;				// 고유 id
	float	TraceRange;		// 추격 범위
	float	HitRange;		// 타격 범위
	float	MovePoint;		// 이동 포인트
	float	HitPoint;		// 타격 포인트	
	bool	bIsAttacking;	// 공격중인지	

private:
	bool	bIsTracking;	// 추격중인지
};


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
	Ghost ghosts;
};