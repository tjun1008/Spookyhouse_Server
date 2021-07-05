#include "main_iocp.h"


array <SESSION, MAX_USER + 1> Clients;

SOCKET listenSocket;
HANDLE h_iocp;

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

void send_login_info(int p_id)
{
	s2c_packet_login_info packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_LOGIN_INFO;

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

	for (int i = 0; i < 22; ++i)
	{
		packet.is_close_door[i] = Clients[p_id].is_close_door[i];

		//cout << packet.is_close_door[i] << ", ";
	}

	packet.is_close_keypad = Clients[p_id].is_keypad;
	packet.is_open_escape = Clients[p_id].is_escape;
	//cout << endl;



	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}

void send_chat(int c_id, int p_id, const char* mess)
{
	s2c_packet_chat p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = S2C_PACKET_CHAT;
	strcpy_s(p.message, mess);
	send_packet(c_id, &p);
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


	unordered_set<int> old_vl;
	Clients[p_id].m_vl.lock();
	old_vl = Clients[p_id].m_viewlist;
	Clients[p_id].m_vl.unlock();

	unordered_set <int> new_vl;

	for (auto& cl : Clients) {
		if (p_id == cl.m_id) continue;
		if (STATE_INGAME != cl.m_state)
		{
			continue;
		}
		if (can_see(p_id, cl.m_id))
			new_vl.insert(cl.m_id);
		//send_move_packet(cl.m_id, p_id);
	}

	//send_move_packet(p_id, p_id);

	for (auto& pl : new_vl) {

		if (0 == old_vl.count(pl))
		{
			// 1. 새로 시야에 들어오는 플레이어 처리

			Clients[p_id].m_vl.lock();
			Clients[p_id].m_viewlist.insert(pl);
			Clients[p_id].m_vl.unlock();
			send_pc_login(p_id, pl); //이름 바꾸기

			Clients[pl].m_vl.lock();

			if (0 == Clients[pl].m_viewlist.count(p_id))
			{
				Clients[pl].m_viewlist.insert(p_id);
				Clients[pl].m_vl.unlock();
				send_pc_login(pl, p_id); //이름 바꾸기
			}

			else
			{
				Clients[pl].m_vl.unlock();
				send_move_packet(pl, p_id);
			}

		}
		else
		{
			//2. 처음부터 끝까지 시야에 존재하는 플레이어 처리
			//players[p_id].m_vl.unlock();
			Clients[pl].m_vl.lock();

			if (0 != Clients[pl].m_viewlist.count(p_id)) {
				Clients[pl].m_vl.unlock();
				send_move_packet(pl, p_id);
			}
			else
			{
				Clients[pl].m_viewlist.insert(p_id);
				Clients[pl].m_vl.unlock();
				send_pc_login(p_id, pl); //이름 바꾸기

			}

		}

	}

	//3. 시야에서 벗어나는 플레이어 처리

	for (auto pl : old_vl)
	{
		if (0 == new_vl.count(pl)) {
			Clients[p_id].m_vl.lock();
			Clients[p_id].m_viewlist.erase(pl);
			Clients[p_id].m_vl.unlock();

			send_pc_logout(p_id, pl);



			if (0 != Clients[pl].m_viewlist.count(p_id))
			{
				Clients[pl].m_vl.lock();
				Clients[pl].m_viewlist.erase(p_id);
				Clients[pl].m_vl.unlock();
				send_pc_logout(pl, p_id);
			}
		}
		//else
			//players[pl].m_vl.unlock();

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

		strcpy_s(Clients[p_id].m_name, p->name);

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

		//플레이어 정보 초기화
		send_login_info(p_id);
		Clients[p_id].m_state = STATE_INGAME;



		for (auto& p : Clients) {
			if (p.m_id == p_id) continue;
			lock_guard<mutex> guard1{ p.m_lock };
			//p.m_lock.lock();
			if (p.m_state != STATE_INGAME)
			{
				//p.m_lock.unlock();
				continue;
			}

			if (can_see(p_id, p.m_id))
			{


				Clients[p_id].m_vl.lock();
				Clients[p_id].m_viewlist.insert(p.m_id);
				Clients[p_id].m_vl.unlock();

				send_pc_login(p.m_id, p_id);

				p.m_vl.lock();
				p.m_viewlist.insert(p_id);
				send_pc_login(p_id, p.m_id);
				p.m_vl.unlock();



			}

			//p.m_lock.unlock();
		}
	}
	break;
	case C2S_PACKET_MOVE: {
		c2s_packet_move* move_packet = reinterpret_cast<c2s_packet_move*>(packet);

		player_move(p_id, move_packet);
	}
						break;

	case C2S_PACKET_OBJECT: {
		c2s_packet_object* object_packet = reinterpret_cast<c2s_packet_object*>(packet);

		/*
		for (int i = 0; i < 24; ++i)
		{
			cout << object_packet->is_close_door[i] << ", ";
		}
		cout << endl;
		*/
		for (int i = 0; i < 22; ++i)
		{
			Clients[p_id].is_close_door[i] = object_packet->is_close_door[i];
		}

		Clients[p_id].is_keypad = object_packet->is_close_keypad;
		Clients[p_id].is_escape = object_packet->is_open_escape;

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
		cout << chat_packet->message << endl;
		send_chat(p_id, p_id, chat_packet->message); //나에게
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

	wcout.imbue(locale("korean"));


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