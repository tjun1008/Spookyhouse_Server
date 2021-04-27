#pragma once


constexpr int MAX_NAME = 50;

constexpr int MAX_BUFFER = 1024;
constexpr short SERVER_PORT = 3800;
constexpr int BOARD_WIDTH = 8;
constexpr int BOARD_HEIGHT = 8;
constexpr int MAX_USER = 4;
constexpr short DAMAGE = 10;


// IOCP 소켓 구조체


constexpr unsigned char C2S_PACKET_LOGIN = 1;
constexpr unsigned char C2S_PACKET_MOVE = 2;
constexpr unsigned char C2S_PACKET_COLID = 3;
constexpr unsigned char S2C_PACKET_LOGIN_INFO = 4;
constexpr unsigned char S2C_PACKET_PC_LOGIN = 5;
constexpr unsigned char S2C_PACKET_PC_MOVE = 6;
constexpr unsigned char S2C_PACKET_PC_COLID = 7;
constexpr unsigned char S2C_PACKET_GHOST_MOVE = 8;
constexpr unsigned char S2C_PACKET_PC_LOGOUT = 9;

#pragma pack (push,1) //공백이 생기는걸 막아야함

struct c2s_packet_login {
	unsigned char size;
	unsigned char type;

	char name[MAX_NAME];
};

struct c2s_packet_move {
	unsigned char size;
	unsigned char type;

	// 정보
	short x, y, z;
	short yaw, pitch, roll;
	short vx, vy, vz;

	//char dir; // 0 : UP 1: RIGHT 2:DOWN 3: LEFT
};

struct c2s_packet_colid {
	unsigned char size;
	unsigned char type;
	//char id;
};

struct c2s_packet_skill {
	unsigned char size;
	unsigned char type;
	char skill;
};

struct  s2c_packet_login_info
{
	unsigned char size;
	unsigned char type;
	int id;

	// 정보
	short x, y, z;
	short yaw, pitch, roll;
	short vx, vy, vz;

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
	short x, y, z;
	char o_type;
};

struct s2c_packet_pc_move
{
	unsigned char size;
	unsigned char type;
	int id;
	short x, y, z;
	short yaw, pitch, roll;
	short vx, vy, vz;

};

struct s2c_packet_pc_colid
{
	unsigned char size;
	unsigned char type;
	int id;
	int hp;
	bool isalive;
};

struct sc_packet_skill {
	unsigned char size;
	unsigned char type;
	char skill;
	short x, y;
};

/*
struct s2c_packet_ghost_move
{
	unsigned char size;
	unsigned char type;
	int id;

	Ghost ghosts;

};
*/

struct s2c_packet_pc_logout
{
	unsigned char size;
	unsigned char type;
	int id;

};

#pragma pack(pop)


