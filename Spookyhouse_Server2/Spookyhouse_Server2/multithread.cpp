#include "main_iocp.h"


array <SESSION, MAX_USER + 1> Clients;

SOCKET listenSocket;
HANDLE h_iocp;
int cur_player = 0;
bool bIsgameStart = false;
KeyLocation firstf_key, bf_key, secf_key;

void display_error(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << L" 에러 " << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}

bool can_see(int id_a, int id_b)
{
	
	return VIEW_RADIUS * VIEW_RADIUS >= (Clients[id_a].character.x - Clients[id_b].character.x)
		* (Clients[id_a].character.x - Clients[id_b].character.x)
		+ (Clients[id_a].character.y - Clients[id_b].character.y)
		* (Clients[id_a].character.y - Clients[id_b].character.y);
		

	
}

void init_server()
{
	//네트워크 초기화

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);


	// bind() listen()
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //IP주소를 자동으로 찾아서 대입
	bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
	listen(listenSocket, SOMAXCONN);

	//iocp 커널 객체 생성
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

}

void client_init()
{
	//클라이언트 초기화

	for (int i = 0; i < MAX_USER; ++i)
	{
		auto& pl = Clients[i];
		pl.m_id = i;
		pl.m_state = STATE_FREE;

	}
}

void key_init()
{
	//탈출키 초기화
	KeyLocation loc1[6]; // = { {730, -1870, 70},{799, -1870, 70},{1661, -2061, 89},{1661, -1983, 70},{1428, 1512, 145},{1587, 1519, 144} };
	KeyLocation loc2[9];
	KeyLocation loc3[12];

	ifstream file;

	file.open("KeyLocation.txt");

	if (file.is_open() == false)
	{
		cout << "파일을 열지 못했습니다" << endl;
	}
	for (int i = 0; i < 6; ++i)
	{
		file >> loc1[i].x >> loc1[i].y >> loc1[i].z;
	}

	for (int i = 0; i < 9; ++i)
	{
		file >> loc2[i].x >> loc2[i].y >> loc2[i].z;
	}

	for (int i = 0; i < 12; ++i)
	{
		file >> loc3[i].x >> loc3[i].y >> loc3[i].z;
	}
// 파일불러오기 확인 완료

	srand(time(NULL));

	int x1, x2, x3;

	x1 = rand() % 6;
	x2 = rand() % 9;
	x3 = rand() % 12;

	firstf_key.x = loc1[x1].x;
	firstf_key.y = loc1[x1].y;
	firstf_key.z = loc1[x1].z;

	bf_key.x = loc2[x2].x;
	bf_key.y = loc2[x2].y;
	bf_key.z = loc2[x2].z;

	secf_key.x = loc3[x3].x;
	secf_key.y = loc3[x3].y;
	secf_key.z = loc3[x3].z;

	cout << firstf_key.x << " " << firstf_key.y << " " << firstf_key.z << endl;
	cout << bf_key.x << " " << bf_key.y << " " << bf_key.z << endl;
	cout << secf_key.x << " " << secf_key.y << " " << secf_key.z << endl;

}

void send_packet(int p_id, void* buf)
{
	EX_OVER* s_over = new EX_OVER;

	unsigned char packet_size = reinterpret_cast<unsigned char*>(buf)[0];
	s_over->m_op = OP_SEND;
	memset(&s_over->m_over, 0, sizeof(s_over->m_over));
	memcpy(s_over->m_netbuf, buf, packet_size);
	s_over->m_wsabuf[0].buf = reinterpret_cast<char*>(s_over->m_netbuf);
	s_over->m_wsabuf[0].len = packet_size;

	WSASend(Clients[p_id].m_s, s_over->m_wsabuf, 1, 0, 0, &s_over->m_over, 0);
}

void send_pc_lobby(int c_id, int p_id)
{
	s2c_packet_lobby packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_LOBBY;

	packet.host = Clients[p_id].character.host;
	send_packet(c_id, &packet);
}

void send_login_info(int p_id)
{
	s2c_packet_login_info packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_LOGIN_INFO;

	//packet.x = Clients[p_id].character.x;
	//packet.y = Clients[p_id].character.y;
	//packet.z = Clients[p_id].character.z;

	//packet.yaw = Clients[p_id].character.yaw;
	//packet.pitch = Clients[p_id].character.pitch;
	//packet.roll = Clients[p_id].character.roll;

	//packet.vx = Clients[p_id].character.vx;
	//packet.vy = Clients[p_id].character.vy;
	//packet.vz = Clients[p_id].character.vz;

	//packet.flashlight = players[p_id].character.flashlight;

	send_packet(p_id, &packet);
}

void send_pc_login(int c_id, int p_id)
{
	s2c_packet_pc_login packet;
	packet.id = p_id;
	packet.size = sizeof(packet);

	packet.type = S2C_PACKET_PC_LOGIN;
	packet.x = Clients[p_id].character.x;
	packet.y = Clients[p_id].character.y;
	packet.z = Clients[p_id].character.z;

	packet.yaw = Clients[p_id].character.yaw;
	packet.pitch = Clients[p_id].character.pitch;
	packet.roll = Clients[p_id].character.roll;

	packet.vx = Clients[p_id].character.vx;
	packet.vy = Clients[p_id].character.vy;
	packet.vz = Clients[p_id].character.vz;

	//packet.flashlight = players[p_id].character.flashlight;

	strcpy_s(packet.name, Clients[p_id].m_name);

	packet.o_type = 0;
	send_packet(c_id, &packet);
}

void send_ready(int c_id, int p_id)
{
	s2c_packet_ready p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = S2C_PACKET_READY;
	p.ready = Clients[p_id].character.ready;
	send_packet(c_id, &p);
}

void send_start(int p_id)
{
	s2c_packet_start p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = S2C_PACKET_GAME_START;
	p.start = bIsgameStart;
	send_packet(p_id, &p);
}

void send_key_info(int p_id)
{
	s2c_packet_key_info p;
	p.size = sizeof(p);
	p.type = S2C_PACKET_KEY_INFO;
	p.firstf_x = firstf_key.x;
	p.firstf_y = firstf_key.y;
	p.firstf_z = firstf_key.z;

	p.bf_x = bf_key.x;
	p.bf_y = bf_key.y;
	p.bf_z = bf_key.z;

	p.secf_x = secf_key.x;
	p.secf_y = secf_key.y;
	p.secf_z = secf_key.z;

	send_packet(p_id, &p);
}

void send_move_packet(int c_id, int p_id)
{
	s2c_packet_pc_move packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_PC_MOVE;

	packet.x = Clients[p_id].character.x;
	packet.y = Clients[p_id].character.y;
	packet.z = Clients[p_id].character.z;

	packet.pitch = Clients[p_id].character.pitch;
	packet.roll = Clients[p_id].character.roll;
	packet.yaw = Clients[p_id].character.yaw;

	packet.vx = Clients[p_id].character.vx;
	packet.vy = Clients[p_id].character.vy;
	packet.vz = Clients[p_id].character.vz;

	packet.flashlight = Clients[p_id].character.flashlight;

	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}

void send_object_packet(int c_id, int p_id)
{
	s2c_packet_object packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_OBJECT;

	for (int i = 0; i < 21; ++i)
	{
		packet.is_close_door[i] = Clients[p_id].object.is_close_door[i];

		//cout << packet.is_close_door[i] << ", ";
	}

	for (int i = 0; i < 6; ++i)
	{
		packet.is_close_closet_left[i] = Clients[p_id].object.is_close_closet_left[i];
		packet.is_close_closet_right[i] = Clients[p_id].object.is_close_closet_right[i];
		//cout << packet.is_close_closet_left[i] << ", ";
		//cout << endl;
	}

	for (int i = 0; i < 8; ++i)
	{
		packet.is_close_drawer[i] = Clients[p_id].object.is_close_drawer[i];
	}

	for (int i = 0; i < 4; ++i)
	{
		packet.is_close_refriger[i] = Clients[p_id].object.is_close_refriger[i];

	//cout << packet.is_close_refriger[i] << ", ";
	//cout << endl;
	}

	packet.is_close_keypad = Clients[p_id].object.is_keypad;
	packet.is_open_escape = Clients[p_id].object.is_escape;
	//cout << endl;



	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}

void send_chat(int c_id, int p_id, wchar_t mess[])
{
	s2c_packet_chat p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = S2C_PACKET_CHAT;
	wcscpy_s(p.message, mess);
	send_packet(c_id, &p);
}

void send_key_packet(int c_id, int p_id, int Escapekey, int numofkey)
{
	s2c_packet_key packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_KEY;
	packet.escape_key = Escapekey;
	packet.num_key = numofkey;

	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}

void send_key_visible_packet(int c_id, int p_id)
{
	s2c_packet_key_visible packet;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_KEY_VISIBLE;

	for (int i = 0; i < 3; ++i)
	{
		packet.get_escapekey[i] = Clients[p_id].escape_key[i];

		//cout << packet.get_escapekey[i] << ", "; //1 0 0
	}

	for (int i = 0; i < 11; ++i)
	{
		packet.get_objectkey[i] = Clients[p_id].object_key[i];

		//cout << packet.get_objectkey[i] << ", "; //1 0 0
	}

	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}

void send_pc_logout(int c_id, int p_id)
{
	s2c_packet_pc_logout packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_PC_LOGOUT;
	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}

void player_move(int p_id, c2s_packet_move* move_packet)
{
	Clients[p_id].character.x = move_packet->x;
	Clients[p_id].character.y = move_packet->y;
	Clients[p_id].character.z = move_packet->z;

	Clients[p_id].character.yaw = move_packet->yaw;
	Clients[p_id].character.pitch = move_packet->pitch;
	Clients[p_id].character.roll = move_packet->roll;

	Clients[p_id].character.vx = move_packet->vx;
	Clients[p_id].character.vy = move_packet->vy;
	Clients[p_id].character.vz = move_packet->vz;

	Clients[p_id].character.flashlight = move_packet->flashlight;

	
	for (auto& cl : Clients) {
		cl.m_lock.lock();
		if (STATE_INGAME != cl.m_state)
		{
			cl.m_lock.unlock();
			continue;
		}
		send_move_packet(cl.m_id, p_id);
		cl.m_lock.unlock();
	}
	
}

int get_new_player_id()
{

	for (int i = 0; i < MAX_USER; ++i) {
		Clients[i].m_lock.lock();
		if (STATE_FREE == Clients[i].m_state)
		{
			Clients[i].m_state = STATE_CONNECTED;
			Clients[i].m_lock.unlock();
			return i;
		}
		Clients[i].m_lock.unlock();
	}


	return -1;

}

void do_accept(SOCKET s_socket, EX_OVER* a_over)
{
	SOCKET  c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&a_over->m_over, 0, sizeof(a_over->m_over));
	DWORD num_byte;
	int addr_size = sizeof(SOCKADDR_IN) + 16;
	a_over->m_csocket = c_socket;
	BOOL ret = AcceptEx(s_socket, c_socket, a_over->m_netbuf, 0, addr_size, addr_size, &num_byte, &a_over->m_over);
	if (FALSE == ret) {
		int err = WSAGetLastError();
		if (WSA_IO_PENDING != err) {
			display_error("AcceptEx : ", err);
			exit(-1);
		}
	}
}

void do_recv(int p_id)
{
	SESSION& pl = Clients[p_id];
	EX_OVER& r_over = pl.m_recv_over;
	//r_over.m_op = OP_RECV;
	memset(&r_over.m_over, 0, sizeof(r_over.m_over));
	r_over.m_wsabuf[0].buf = reinterpret_cast<CHAR*>(r_over.m_netbuf) + pl.m_prev_recv;
	r_over.m_wsabuf[0].len = MAX_BUFFER - pl.m_prev_recv;
	DWORD r_flag = 0;
	WSARecv(pl.m_s, r_over.m_wsabuf, 1, 0, &r_flag, &r_over.m_over, 0);
}

void disconnect(int p_id)
{

	
		lock_guard<mutex> guard0{ Clients[p_id].m_lock };
		//players[p_id].m_lock.lock();
		Clients[p_id].m_state = STATE_CONNECTED;
		closesocket(Clients[p_id].m_s);
		Clients[p_id].m_viewlist.clear();
		//players[p_id].m_lock.unlock();
		//players.erase(p_id);
	
		for (auto& cl : Clients)
		{
			if (cl.m_id == p_id) continue;
			//cl.m_cLock.lock();
			if (STATE_INGAME == cl.m_state)
				send_pc_logout(cl.m_id, p_id);
			//cl.m_cLock.unlock();
		}

	Clients[p_id].m_state = STATE_FREE;
}

void process_packet(int p_id, unsigned char* packet)
{
	c2s_packet_login* p = reinterpret_cast<c2s_packet_login*>(packet);
	switch (p->type) {
	case C2S_PACKET_LOGIN:
	{
		lock_guard<mutex> guard2{ Clients[p_id].m_lock };

		if (cur_player >= 4)
		{
			closesocket(listenSocket);
			cout << "더이상 접속 할 수 없습니다." << endl;
		}

		if (true == bIsgameStart) {
			closesocket(listenSocket);
			cout << "게임이 이미 시작되어 접속 할 수 없습니다." << endl;


		}

		if (cur_player == 0)
		{
			Clients[p_id].character.host = true;
		}
		else
		{
			Clients[p_id].character.host = false;
		}

		Clients[p_id].m_state = STATE_INGAME;

		cur_player++;

		strcpy_s(Clients[p_id].m_name, p->name);

		send_pc_lobby(p_id, p_id);
		send_login_info(p_id);
		cout << "확인" << endl;

		for (auto& pl : Clients)
		{
			if (pl.m_id == p_id) continue;

			if (STATE_INGAME != pl.m_state)
			{
				continue;
			}

			
			send_pc_lobby(pl.m_id, p_id);
			send_pc_lobby(p_id, pl.m_id);
		}

	}
		break;

	case C2S_PACKET_READY: {
		c2s_packet_ready* ready_packet = reinterpret_cast<c2s_packet_ready*>(packet);

		Clients[p_id].character.ready = ready_packet->ready;

		send_ready(p_id, p_id);

		for (auto& pl : Clients)
		{
			if (pl.m_id == p_id) continue;

			if (STATE_INGAME != pl.m_state)
			{
				continue;
			}

			send_ready(pl.m_id, p_id); //상대방에게
			send_ready(p_id, pl.m_id); //상대방에게

		}
		
	}
						break;

	case C2S_PACKET_START_CLICK: {
		c2s_packet_start* start_packet = reinterpret_cast<c2s_packet_start*>(packet);

		if (true == Clients[p_id].character.host)
		{

			for (auto& pl : Clients)
			{
				if (pl.m_id == p_id) continue;

				if (STATE_INGAME != pl.m_state)
				{
					continue;
				}

				if (false == Clients[pl.m_id].character.ready)
				{
					cout << p_id <<"start test false"; 

					bIsgameStart = false;
					break;
				}
				cout << "start test true";
				bIsgameStart = true;
			}

		}

		if(bIsgameStart == true)
		{ 
			send_start(p_id); //상대방에게
			send_key_info(p_id);

		for (auto& pl : Clients)
		{
			if (pl.m_id == p_id) continue;

			if (STATE_INGAME != pl.m_state)
			{
				continue;
			}
			send_start(pl.m_id); //상대방에게
			send_key_info(pl.m_id);
			/*
			Clients[pl.m_id].character.x = -1620;
			Clients[pl.m_id].character.y = -29;
			Clients[pl.m_id].character.z = 117;

			Clients[pl.m_id].character.vx = 0;
			Clients[pl.m_id].character.vy = 0;
			Clients[pl.m_id].character.vz = 0;

			Clients[pl.m_id].character.yaw = 0;
			Clients[pl.m_id].character.pitch = 0;
			Clients[pl.m_id].character.roll = 0;

			Clients[pl.m_id].character.flashlight = true;

			//플레이어 정보 초기화
			//send_login_info(pl.m_id);
			cout << "p_id : " << p_id << "pl.id: " << pl.m_id << endl;

			send_pc_login(p_id, pl.m_id);
			send_pc_login(pl.m_id, p_id);
			
			*/

			

			
		}

		}

	}
						 break;
	case C2S_PACKET_MOVE: {
		c2s_packet_move* move_packet = reinterpret_cast<c2s_packet_move*>(packet);

		player_move(p_id, move_packet);
	}
						break;
	case C2S_PACKET_PLAYER_INFO: {
		c2s_packet_playerinfo* info_packet = reinterpret_cast<c2s_packet_playerinfo*>(packet);
		
		Clients[p_id].character.x = -1620;
		Clients[p_id].character.y = -29;
		Clients[p_id].character.z = 117;

		Clients[p_id].character.vx = 0;
		Clients[p_id].character.vy = 0;
		Clients[p_id].character.vz = 0;

		Clients[p_id].character.yaw = 0;
		Clients[p_id].character.pitch = 0;
		Clients[p_id].character.roll = 0;

		Clients[p_id].character.flashlight = true;

		for (auto& pl : Clients)
		{
			if (pl.m_id == p_id) continue;

			if (STATE_INGAME != pl.m_state)
			{
				continue;
			}


			send_pc_login(p_id, pl.m_id);
			send_pc_login(pl.m_id, p_id);

			

		}

		
		
	}
						break;

	case C2S_PACKET_OBJECT: {
		c2s_packet_object* object_packet = reinterpret_cast<c2s_packet_object*>(packet);

		/*
		for (int i = 0; i < 6; ++i)
		{
			cout << object_packet->is_close_closet_left[i] << ", ";
		}
		cout << endl;
		*/

		//cout << "object 확인" << endl;

		for (int i = 0; i < 21; ++i)
		{
			Clients[p_id].object.is_close_door[i] = object_packet->is_close_door[i];
		}

		for (int i = 0; i < 6; ++i)
		{
			Clients[p_id].object.is_close_closet_left[i] = object_packet->is_close_closet_left[i];
			Clients[p_id].object.is_close_closet_right[i] = object_packet->is_close_closet_right[i];
		}

		for (int i = 0; i < 8; ++i)
		{
			Clients[p_id].object.is_close_drawer[i] = object_packet->is_close_drawer[i];
		}

		for (int i = 0; i < 4; ++i)
		{
			Clients[p_id].object.is_close_refriger[i] = object_packet->is_close_refriger[i];
		}
		
		Clients[p_id].object.is_keypad = object_packet->is_close_keypad;
		Clients[p_id].object.is_escape = object_packet->is_open_escape;

		for (auto& cl : Clients) {
			if (cl.m_id == p_id) continue;
			//cl.m_lock.lock();
			if (STATE_INGAME != cl.m_state)
			{
				//cl.m_lock.unlock();
				continue;
			}
		

			send_object_packet(cl.m_id, p_id);
			//cl.m_lock.unlock();
		}
	}
						break;

	case C2S_PACKET_CHAT: {
		c2s_packet_chat* chat_packet = reinterpret_cast<c2s_packet_chat*>(packet);

		

		for (auto& pl : Clients)
		{
			if (pl.m_id == p_id) continue;

			if (STATE_INGAME != pl.m_state)
			{
				//cl.m_lock.unlock();
				continue;
			}

				send_chat(pl.m_id, p_id, chat_packet->message); //상대방에게

				
		}
		wcout << chat_packet->message << endl;
		send_chat(p_id, p_id, chat_packet->message); //나에게
	}
				break;

	case C2S_PACKET_KEY: {
		c2s_packet_key* key_packet = reinterpret_cast<c2s_packet_key*>(packet);

		/*
		int ekey = key_packet->escape_key;
		int numkey = key_packet->num_key;

		cout << "ekey: " << ekey << "numkey: " << numkey << endl;
		*/
			
		for (auto& pl : Clients)
		{
			if (pl.m_id == p_id) continue;

			if (STATE_INGAME != pl.m_state)
			{
				//cl.m_lock.unlock();
				continue;
			}

			send_key_packet(pl.m_id, p_id, key_packet->escape_key, key_packet->num_key); //상대방에게


		}
		//send_key_packet(p_id, p_id, key_packet->escape_key, key_packet->num_key); //나에게
		
	}
						break;

	case C2S_PACKET_KEY_VISIBLE: {
		c2s_packet_key_visible* key_visible_packet = reinterpret_cast<c2s_packet_key_visible*>(packet);

		for (int i = 0; i < 3; ++i)
		{
			Clients[p_id].escape_key[i] = key_visible_packet->get_escapekey[i];
		}

		for (int i = 0; i < 11; ++i)
		{
			Clients[p_id].object_key[i] = key_visible_packet->get_objectkey[i];
		}

		for (auto& pl : Clients)
		{
			if (pl.m_id == p_id) continue;

			if (STATE_INGAME != pl.m_state)
			{
				//cl.m_lock.unlock();
				continue;
			}

			send_key_visible_packet(pl.m_id, p_id); //상대방에게


		}
		//send_key_packet(p_id, p_id, key_packet->escape_key, key_packet->num_key); //나에게

	}
					   break;
	default:
		cout << "Unknown Packet Type [" << p->type << "] Error\n";
		exit(-1);
	}
}

void worker()
{
	while (true) {
		DWORD num_byte;
		ULONG_PTR i_key;
		WSAOVERLAPPED* over;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_byte, &i_key, &over, INFINITE);
		int key = static_cast<int> (i_key);
		if (FALSE == ret) {
			int err = WSAGetLastError();
			display_error("GQCS : ", err);
			disconnect(key);
			continue;
		}

		EX_OVER* ex_over = reinterpret_cast<EX_OVER*>(over);

		switch (ex_over->m_op) {
		case OP_RECV:
		{
			unsigned char* ps = ex_over->m_netbuf;
			int remain_data = num_byte + Clients[key].m_prev_recv;
			while (remain_data > 0) {
				int packet_size = ps[0];
				if (packet_size > remain_data) break;
				process_packet(key, ps);
				remain_data -= packet_size;
				ps += packet_size;
			}
			if (remain_data > 0)
				memcpy(ex_over->m_netbuf, ps, remain_data);
			Clients[key].m_prev_recv = remain_data;
			do_recv(key);

			//일단 패킷조립 이렇게 하고.. 만약 안되면 기존꺼쓰자ㅠ

		}
		break;
		case OP_SEND:
			if (num_byte != ex_over->m_wsabuf[0].len)
				disconnect(key);
			delete ex_over;
			break;
		case OP_ACCEPT:
		{
			SOCKET c_socket = ex_over->m_csocket;
			int p_id = get_new_player_id();
			if (-1 == p_id) {
				closesocket(Clients[p_id].m_s);
				do_accept(listenSocket, ex_over);
				continue;
			}

			Clients[p_id].m_prev_recv = 0;
			Clients[p_id].m_recv_over.m_op = OP_RECV;
			Clients[p_id].m_s = c_socket;

			

			CreateIoCompletionPort(reinterpret_cast<HANDLE>(Clients[p_id].m_s), h_iocp, p_id, 0);


			do_recv(p_id);
			do_accept(listenSocket, ex_over);


			cout << "New CLient [" << p_id << "] connected.\n";

		
		}
		break;
		default: cout << "Unknown GQCS Error!\n";
			exit(-1);
		}
	}
}

int main()
{

	std::locale::global(std::locale("korean"));
	//SetConsoleOutputCP(CP_UTF8);

	wcout.imbue(locale("korean"));

	key_init();
	cout << "key 초기화 완료" << endl;

	init_server();

	client_init();

	// iocp 객체와 소켓 연결
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket),
		h_iocp, 100000, 0);

	EX_OVER a_over;
	a_over.m_op = OP_ACCEPT;
	do_accept(listenSocket, &a_over);

	vector <thread> worker_threads;
	for (int i = 0; i < NUM_THREADS; ++i)
		worker_threads.emplace_back(worker);

	for (auto& th : worker_threads) th.join();
	closesocket(listenSocket);
	WSACleanup();
}