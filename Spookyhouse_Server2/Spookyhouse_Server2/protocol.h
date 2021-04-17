#pragma once


#include <iostream>
#include <map>
using namespace std;
#include <WS2tcpip.h>
#include <MSWSock.h>



#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"MSWSock.lib")


void error_display(const char* msg, int err_no);

constexpr int MAX_NAME = 50;

constexpr int MAX_BUFFER = 1024;
constexpr short SERVER_PORT = 3800;
constexpr int BOARD_WIDTH = 8;
constexpr int BOARD_HEIGHT = 8;
constexpr int MAX_USER = 4;
constexpr short DAMAGE =10;

enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT };

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

class Ghost
{
public:
	Ghost();
	virtual ~Ghost();

	// �÷��̾�� �̵�
	void MoveTo(const CharacterPacket& target);
	// �÷��̾� Ÿ��
	void HitPlayer(CharacterPacket& target);
	// ����ִ��� ����
	bool IsAlive();
	// ���������� ����
	bool IsAttacking();
	// �÷��̾ �߰� ������ �ִ���
	bool IsPlayerInTraceRange(const CharacterPacket& target);
	// �÷��̾ Ÿ�� ������ �ִ���
	bool IsPlayerInHitRange(const CharacterPacket& target);
	// ��ġ ����
	void SetLocation(float x, float y, float z);


	float	X;				// X��ǥ
	float	Y;				// Y��ǥ
	float	Z;				// Z��ǥ
	float	Health;			// ü��
	int		Id;				// ���� id
	float	TraceRange;		// �߰� ����
	float	HitRange;		// Ÿ�� ����
	float	MovePoint;		// �̵� ����Ʈ
	float	HitPoint;		// Ÿ�� ����Ʈ	
	bool	bIsAttacking;	// ����������	

private:
	bool	bIsTracking;	// �߰�������
};


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
	Ghost ghosts;
};


// IOCP ���� ����ü



constexpr unsigned char C2S_PACKET_LOGIN = 1;
constexpr unsigned char C2S_PACKET_MOVE = 2;
constexpr unsigned char C2S_PACKET_COLID = 3;
constexpr unsigned char S2C_PACKET_LOGIN_INFO = 4;
constexpr unsigned char S2C_PACKET_PC_LOGIN = 5;
constexpr unsigned char S2C_PACKET_PC_MOVE = 6;
constexpr unsigned char S2C_PACKET_PC_COLID = 7;
constexpr unsigned char S2C_PACKET_GHOST_MOVE = 8;
constexpr unsigned char S2C_PACKET_PC_LOGOUT = 9;

#pragma pack (push,1) //������ ����°� ���ƾ���

struct c2s_packet_login {
	unsigned char size;
	unsigned char type;
	char name[MAX_NAME];
};

struct c2s_packet_move {
	unsigned char size;
	unsigned char type;

	char dir; // 0 : UP 1: RIGHT 2:DOWN 3: LEFT
};

struct c2s_packet_colid {
	unsigned char size;
	unsigned char type;
	//char id;
};

struct  s2c_packet_login_info
{
	unsigned char size;
	unsigned char type;
	int id;

	// ����
	short x, y,z;
	short yaw, pitch, roll;
	float vx, vy, vz;

	// etc
	int hp;
	int skill_gage;
	bool isalive = true;
};

struct s2c_packet_pc_login
{
	unsigned char size;
	unsigned char type;
	int id;
	char name[MAX_NAME];
	short x, y;
	char o_type;
};

struct s2c_packet_pc_move
{
	unsigned char size;
	unsigned char type;
	int id;
	short x, y,z;
	short yaw, pitch, roll;
	float vx, vy, vz;

};

struct s2c_packet_pc_colid
{
	unsigned char size;
	unsigned char type;
	int id;
	int hp;
	bool isalive;
};

struct s2c_packet_ghost_move
{
	unsigned char size;
	unsigned char type;
	int id;

	Ghost ghosts;

};

struct s2c_packet_pc_logout
{
	unsigned char size;
	unsigned char type;
	int id;

};

#pragma pack(pop)

