#pragma once





constexpr int MAX_NAME = 50;

constexpr int MAX_BUFFER = 512;
constexpr short SERVER_PORT = 3700;
constexpr int BOARD_WIDTH = 8;
constexpr int BOARD_HEIGHT = 8;
constexpr int MAX_USER = 4;
constexpr short DAMAGE =10;
constexpr int MAX_CHAT_SIZE = 50;
constexpr int MAX_PACKET_SIZE = 255;






// IOCP 소켓 구조체


constexpr unsigned char C2S_PACKET_LOGIN = 1;
constexpr unsigned char C2S_PACKET_MOVE = 2;
constexpr unsigned char S2C_PACKET_LOGIN_INFO = 3;
constexpr unsigned char S2C_PACKET_PC_LOGIN = 4;
constexpr unsigned char S2C_PACKET_PC_MOVE = 5;
constexpr unsigned char S2C_PACKET_PC_LOGOUT = 6;

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
	float x, y, z;
	float yaw, pitch, roll;
	float vx, vy, vz;

	//char dir; // 0 : UP 1: RIGHT 2:DOWN 3: LEFT
};

struct  s2c_packet_login_info
{
	unsigned char size;
	unsigned char type;
	int id;

	// 정보
	float x, y,z;
	float yaw, pitch, roll;
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
	float x, y, z;
	float yaw, pitch, roll;
	float vx, vy, vz;
	char o_type;
};

struct s2c_packet_pc_move
{
	unsigned char size;
	unsigned char type;
	int id;
	float x, y,z;
	float yaw, pitch, roll;
	float vx, vy, vz;

};

struct s2c_packet_pc_logout
{
	unsigned char size;
	unsigned char type;
	int id;

};

#pragma pack(pop)

