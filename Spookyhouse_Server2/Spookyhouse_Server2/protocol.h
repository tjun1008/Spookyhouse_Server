#pragma once





constexpr int MAX_NAME = 20;
constexpr int MAX_STR_LEN = 50;

constexpr int MAX_BUFFER = 512;
constexpr short SERVER_PORT = 3800;
constexpr int BOARD_WIDTH = 8;
constexpr int BOARD_HEIGHT = 8;
constexpr int MAX_USER = 4;
constexpr short DAMAGE = 10;
constexpr int MAX_CHAT_SIZE = 50;
constexpr int MAX_PACKET_SIZE1 = 255;
constexpr int VIEW_RADIUS = 1000;





// IOCP 소켓 구조체


constexpr unsigned char C2S_PACKET_LOGIN = 1;
constexpr unsigned char C2S_PACKET_READY = 2;
constexpr unsigned char C2S_PACKET_START_CLICK = 3;
constexpr unsigned char C2S_PACKET_MOVE = 4;
constexpr unsigned char C2S_PACKET_OBJECT = 5;
constexpr unsigned char C2S_PACKET_CHAT = 6;

constexpr unsigned char S2C_PACKET_LOBBY = 7;
constexpr unsigned char S2C_PACKET_READY = 8;
constexpr unsigned char S2C_PACKET_GAME_START = 9;
constexpr unsigned char S2C_PACKET_LOGIN_INFO = 10;
constexpr unsigned char S2C_PACKET_PC_LOGIN = 11;
constexpr unsigned char S2C_PACKET_PC_MOVE = 12;
constexpr unsigned char S2C_PACKET_OBJECT = 13;
constexpr unsigned char S2C_PACKET_CHAT = 14;
constexpr unsigned char S2C_PACKET_PC_LOGOUT = 15;

#pragma pack (push,1) //공백이 생기는걸 막아야함

struct c2s_packet_login {
	unsigned char size;
	unsigned char type;
	char name[MAX_NAME];
};

struct c2s_packet_ready {
	unsigned char size;
	unsigned char type;
	bool ready;
};

struct c2s_packet_start {
	unsigned char size;
	unsigned char type;
	bool start;
};

struct c2s_packet_move {
	unsigned char size;
	unsigned char type;

	// 정보
	float x, y, z;
	float yaw, pitch, roll;
	float vx, vy, vz;

	bool flashlight;

	//char dir; // 0 : UP 1: RIGHT 2:DOWN 3: LEFT
};

struct c2s_packet_object {
	unsigned char size;
	unsigned char type;

	bool is_close_door[22];
	bool is_close_keypad;
	bool is_open_escape;
};

struct c2s_packet_chat {
	unsigned char	size;
	char	type;
	wchar_t 	message[MAX_STR_LEN];
};

struct  s2c_packet_lobby
{
	unsigned char size;
	unsigned char type;
	int id;

	bool host;
};

struct  s2c_packet_ready
{
	unsigned char size;
	unsigned char type;
	int id;

	bool ready;
};

struct  s2c_packet_start
{
	unsigned char size;
	unsigned char type;
	int id;

	bool start;
};

struct  s2c_packet_login_info
{
	unsigned char size;
	unsigned char type;
	int id;

	// 정보
	float x, y, z;
	float yaw, pitch, roll;
	float vx, vy, vz;

	// etc
	bool flashlight;
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
	bool flashlight;
	char o_type;
};

struct s2c_packet_pc_move
{
	unsigned char size;
	unsigned char type;
	int id;
	float x, y, z;
	float yaw, pitch, roll;
	float vx, vy, vz;
	bool flashlight;
};

struct s2c_packet_object
{
	unsigned char size;
	unsigned char type;
	int id;
	bool is_close_door[22];
	bool is_close_keypad;
	bool is_open_escape;
};

struct s2c_packet_chat {
	unsigned char size;
	char	type;
	int	id;
	wchar_t	message[MAX_STR_LEN];
};

struct s2c_packet_pc_logout
{
	unsigned char size;
	unsigned char type;
	int id;

};

#pragma pack(pop)

