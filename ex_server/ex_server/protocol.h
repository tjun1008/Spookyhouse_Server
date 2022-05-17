#pragma once





constexpr int MAX_NAME = 20;
constexpr int MAX_STR_LEN = 50;

constexpr int MAX_BUFFER = 512;
constexpr short SERVER_PORT = 3600;
constexpr int BOARD_WIDTH = 8;
constexpr int BOARD_HEIGHT = 8;
constexpr int MAX_USER = 4;
constexpr int MAX_CLIENT = 10; //5���� npc ����
constexpr int MAX_MONSTER = 5;
constexpr short DAMAGE = 10;
constexpr int MAX_CHAT_SIZE = 50;
constexpr int MAX_PACKET_SIZE1 = 255;
constexpr int VIEW_RADIUS = 1000;



// IOCP ���� ����ü


constexpr unsigned char C2S_PACKET_LOGIN = 1;
constexpr unsigned char C2S_PACKET_READY = 2;
constexpr unsigned char C2S_PACKET_START_CLICK = 3;
constexpr unsigned char C2S_PACKET_PLAYER_INFO = 4;
constexpr unsigned char C2S_PACKET_MOVE = 5;
constexpr unsigned char C2S_PACKET_OBJECT = 6;
constexpr unsigned char C2S_PACKET_OTHEROBJECT = 7;
constexpr unsigned char C2S_PACKET_BOXMOVE = 8;
constexpr unsigned char C2S_PACKET_AIMOVE = 9;
constexpr unsigned char C2S_PACKET_CHAT = 10;
constexpr unsigned char C2S_PACKET_KEY = 11;
constexpr unsigned char C2S_PACKET_KEY_VISIBLE = 12;

constexpr unsigned char S2C_PACKET_LOBBY = 13;
constexpr unsigned char S2C_PACKET_READY = 14;
constexpr unsigned char S2C_PACKET_GAME_START = 15;
constexpr unsigned char S2C_PACKET_LOGIN_INFO = 16;
constexpr unsigned char S2C_PACKET_KEY_INFO = 17;
constexpr unsigned char S2C_PACKET_PC_LOGIN = 18;
constexpr unsigned char S2C_PACKET_PC_MOVE = 19;
constexpr unsigned char S2C_PACKET_NPC_MOVE = 20;
constexpr unsigned char S2C_PACKET_OBJECT = 21;
constexpr unsigned char S2C_PACKET_OTHEROBJECT = 22;
constexpr unsigned char S2C_PACKET_BOXMOVE = 23;
constexpr unsigned char S2C_PACKET_AIMOVE = 24;
constexpr unsigned char S2C_PACKET_CHAT = 25;
constexpr unsigned char S2C_PACKET_KEY = 26;
constexpr unsigned char S2C_PACKET_KEY_VISIBLE = 27;
constexpr unsigned char S2C_PACKET_PC_LOGOUT = 28;


#pragma pack (push,1) //������ ����°� ���ƾ���

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

struct c2s_packet_playerinfo {
	unsigned char size;
	unsigned char type;
	//char name[MAX_NAME];
};

struct c2s_packet_move {
	unsigned char size;
	unsigned char type;

	// ����
	float x, y, z;
	float yaw, pitch, roll;
	float vx, vy, vz;

	bool flashlight;

	//char dir; // 0 : UP 1: RIGHT 2:DOWN 3: LEFT
};

struct c2s_packet_npc_move {
	unsigned char size;
	unsigned char type;

	// ����
	float x, y, z;

};

struct c2s_packet_object {
	unsigned char size;
	unsigned char type;

	bool is_close_door[21];
	bool is_close_closet_left[6];
	bool is_close_closet_right[6];
	bool is_close_drawer[8];
	bool is_close_refriger[8];
	bool is_close_dryer[2];
	bool is_close_lockbox;
	bool is_close_keypad;
	bool is_close_keycard;
	bool is_open_escape;
};

struct c2s_packet_otherobject {
	unsigned char size;
	unsigned char type;

	bool is_open_coffin;
	bool get_robot;
	bool get_baby;
	bool is_open_board;
	bool is_open_safedoor;
};

struct c2s_packet_boxmove {
	unsigned char size;
	unsigned char type;

	float box_x, box_y, box_z;

};

/*
struct c2s_packet_aimove {
	unsigned char size;
	unsigned char type;

	float mug_x, mug_y, mug_z;
	float mug_yaw, mug_pitch, mug_roll;

};
*/

struct c2s_packet_aimove {
	unsigned char size;
	unsigned char type;

	float mug_x[2], mug_y[2], mug_z[2];
	float mug_roll[2], mug_pitch[2], mug_yaw[2];
	float mug_vx[2], mug_vy[2], mug_vz[2];

};

struct c2s_packet_chat {
	unsigned char	size;
	char	type;
	wchar_t  	message[MAX_STR_LEN];
};

struct c2s_packet_key {
	unsigned char size;
	unsigned char type;

	int num_key;
	int escape_key;
};

struct c2s_packet_key_visible {
	unsigned char size;
	unsigned char type;

	bool get_escapekey[3];
	bool get_objectkey[11];
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

	// ����
	float x, y, z;
	float yaw, pitch, roll;
	float vx, vy, vz;

	// etc
	bool flashlight;
};

struct  s2c_packet_key_info
{
	unsigned char size;
	unsigned char type;

	float firstf_x, firstf_y, firstf_z;
	float bf_x, bf_y, bf_z;
	float secf_x, secf_y, secf_z;
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

struct s2c_packet_npc_move
{
	unsigned char size;
	unsigned char type;
	int id;
	float x, y, z;

	int player_id;

};

struct s2c_packet_object
{
	unsigned char size;
	unsigned char type;
	int id;
	bool is_close_door[22];
	bool is_close_closet_left[6];
	bool is_close_closet_right[6];
	bool is_close_drawer[8];
	bool is_close_refriger[8];
	bool is_close_dryer[2];
	bool is_close_lockbox;
	bool is_close_keypad;
	bool is_close_keycard;
	bool is_open_escape;
};

struct s2c_packet_otherobject {
	unsigned char size;
	unsigned char type;

	bool is_open_coffin;
	bool get_robot;
	bool get_baby;
	bool is_open_board;
	bool is_open_safedoor;
};

struct s2c_packet_boxmove {
	unsigned char size;
	unsigned char type;

	float box_x, box_y, box_z;

};

/*
struct s2c_packet_aimove {
	unsigned char size;
	unsigned char type;

	float mug_x, mug_y, mug_z;
	float mug_yaw, mug_pitch, mug_roll;

};
*/
struct s2c_packet_aimove {
	unsigned char size;
	unsigned char type;

	float mug_x[2], mug_y[2], mug_z[2];
	float mug_roll[2], mug_pitch[2], mug_yaw[2];
	float mug_vx[2], mug_vy[2], mug_vz[2];

};

struct s2c_packet_chat {
	unsigned char size;
	char	type;
	int	id;
	wchar_t 	message[MAX_STR_LEN];
};

struct s2c_packet_key {
	unsigned char size;
	char	type;
	int	id;

	int num_key;
	int escape_key;
};

struct s2c_packet_key_visible {
	unsigned char size;
	unsigned char type;

	bool get_escapekey[3];
	bool get_objectkey[11];
};

struct s2c_packet_pc_logout
{
	unsigned char size;
	unsigned char type;
	int id;

};

#pragma pack(pop)

