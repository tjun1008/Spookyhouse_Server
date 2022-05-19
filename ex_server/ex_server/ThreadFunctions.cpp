#include "ThreadFunctions.h"
#include "Header.h"




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
			ReadPacket(key, num_byte, ex_over);

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
				closesocket(dynamic_cast<Player*>(players[key])->GetSocket());
				do_accept(listenSocket, ex_over);
				continue;
			}

			AcceptPacket(key,c_socket,ex_over);
			
		}
		break;
		default: cout << "Unknown GQCS Error!\n";
			exit(-1);
		}
	}
}

void process_packet(int p_id, unsigned char* packet)
{
	c2s_packet_login* p = reinterpret_cast<c2s_packet_login*>(packet);
	switch (p->type) {
	case C2S_PACKET_LOGIN:
	{
		

		if (cur_player >= 4)
		{
			closesocket(listenSocket);
			cout << "더이상 접속 할 수 없습니다." << endl;
		}

		if (true == bIsgameStart) {
			closesocket(listenSocket);
			cout << "게임이 이미 시작되어 접속 할 수 없습니다." << endl;


		}

		Player* player = dynamic_cast<Player*>(players[p_id]);

		if (cur_player == 0)
		{
			player->SetHost(true);
		}
		else
		{
			player->SetHost(false);
		}

		player->SetState(STATE_INGAME);

		cur_player++;

		player->SetGameID(p->name);

		send_pc_lobby(p_id, p_id);
		send_login_info(p_id);
		cout << "확인" << endl;

		for (int i = 0; i < players.size(); ++i)
		{
			Player* pl = dynamic_cast<Player*>(players[i]);

			if (pl->GetID() == p_id) continue;

			if(is_npc(pl->GetID())) continue;

			if (STATE_INGAME != pl->GetState()) continue;

			send_pc_lobby(pl->GetID(), p_id);
			send_pc_lobby(p_id, pl->GetID());

		}

	}
	break;

	case C2S_PACKET_READY: {
		c2s_packet_ready* ready_packet = reinterpret_cast<c2s_packet_ready*>(packet);

		Player* player = dynamic_cast<Player*>(players[p_id]);

		player->SetReady(ready_packet->ready);

		send_ready(p_id, p_id);

		for (int i = 0; i < players.size(); ++i)
		{
			Player* pl = dynamic_cast<Player*>(players[i]);

			if (pl->GetID() == p_id) continue;

			if (is_npc(pl->GetID())) continue;

			if (STATE_INGAME != pl->GetState()) continue;

			send_ready(pl->GetID(), p_id);
			send_ready(p_id, pl->GetID());

		}

	}
	break;

	case C2S_PACKET_START_CLICK: {
		c2s_packet_start* start_packet = reinterpret_cast<c2s_packet_start*>(packet);

		Player* player = dynamic_cast<Player*>(players[p_id]);

		if (true == player->GetHost())
		{

			for (int i = 0; i < players.size(); ++i)
			{
				Player* pl = dynamic_cast<Player*>(players[i]);

				if (pl->GetID() == p_id) continue;

				if (is_npc(pl->GetID())) continue;

				if (STATE_INGAME != pl->GetState()) continue;

				if (false == pl->GetReady())
				{
					cout << p_id << "start test false";

					bIsgameStart = false;
					break;
				}
				cout << "start test true";
				bIsgameStart = true;

			}


		}

		if (bIsgameStart == true)
		{
			send_start(p_id); //상대방에게
			send_key_info(p_id);

			for (int i = 0; i < players.size(); ++i)
			{
				Player* pl = dynamic_cast<Player*>(players[i]);

				if (pl->GetID() == p_id) continue;

				if (is_npc(pl->GetID())) continue;

				if (STATE_INGAME != pl->GetState()) continue;

				send_start(pl->GetID()); //상대방에게
				send_key_info(pl->GetID());

				//Clients[pl.m_id].character.x = -1620;
				//Clients[pl.m_id].character.y = -29;
				//Clients[pl.m_id].character.z = 117;

				//Clients[pl.m_id].character.vx = 0;
				//Clients[pl.m_id].character.vy = 0;
				//Clients[pl.m_id].character.vz = 0;

				//Clients[pl.m_id].character.yaw = 0;
				//Clients[pl.m_id].character.pitch = 0;
				//Clients[pl.m_id].character.roll = 0;

			}

		}

	}
	break;

	case C2S_PACKET_PLAYER_INFO: {
		c2s_packet_playerinfo* info_packet = reinterpret_cast<c2s_packet_playerinfo*>(packet);

		Player* player = dynamic_cast<Player*>(players[p_id]);

		player->SetX(-1620);
		player->SetY(-29);
		player->SetZ(117);

		player->SetYaw(0);
		player->SetPitch(0);
		player->SetRoll(0);

		player->SetVX(0);
		player->SetVY(0);
		player->SetVZ(0);

		player->SetFlashlight(true);

		for (int i = 0; i < players.size(); ++i)
		{
			Player* pl = dynamic_cast<Player*>(players[i]);

			if (pl->GetID() == p_id) continue;

			if (is_npc(pl->GetID())) continue;

			if (STATE_INGAME != pl->GetState()) continue;

			send_pc_login(pl->GetID(), p_id);
			send_pc_login(p_id, pl->GetID());

		}



	}
	break;
	case C2S_PACKET_MOVE: {
		c2s_packet_move* move_packet = reinterpret_cast<c2s_packet_move*>(packet);

		player_move(p_id, move_packet);
	}

	default:
		cout << "Unknown Packet Type [" << p->type << "] Error\n";
		exit(-1);
	}
}

// 해당 id에 대한 플레이어가 이 패킷을 읽고
// recv를 호출한다.
void ReadPacket(const UINT& id, const DWORD& transferred, EX_OVER* ex_over)
{

	dynamic_cast<Player*>(players[id])->ReadPacket(id, transferred, ex_over);
	dynamic_cast<Player*>(players[id])->RecvPacket(id);
}

void AcceptPacket(const UINT& p_id, SOCKET c_socket, EX_OVER* ex_over)
{

	dynamic_cast<Player*>(players[p_id])->AcceptPacket(p_id, c_socket);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(dynamic_cast<Player*>(players[p_id])->GetSocket()), h_iocp, p_id, 0);

	dynamic_cast<Player*>(players[p_id])->do_recv(p_id);
	do_accept(listenSocket, ex_over);
	cout << "New CLient [" << p_id << "] connected.\n";

}

void disconnect(int p_id)
{

	Player* player = dynamic_cast<Player*>(players[p_id]);

	player->Disconnect(p_id,players);

}

void send_pc_logout(int c_id, int p_id)
{
	s2c_packet_pc_logout packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_PC_LOGOUT;
	send_packet(c_id, &packet);
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

	WSASend(dynamic_cast<Player*>(players[p_id])->GetSocket(), s_over->m_wsabuf, 1, 0, 0, &s_over->m_over, 0);
}

void send_login_info(int p_id)
{
	s2c_packet_login_info packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_LOGIN_INFO;
	packet.x = dynamic_cast<Player*>(players[p_id])->GetX();
	packet.y = dynamic_cast<Player*>(players[p_id])->GetY();
	send_packet(p_id, &packet);
}

void send_pc_lobby(int c_id, int p_id)
{
	s2c_packet_lobby packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_LOBBY;

	packet.host = dynamic_cast<Player*>(players[p_id])->GetHost();
	send_packet(c_id, &packet);
}

void send_ready(int c_id, int p_id)
{
	s2c_packet_ready p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = S2C_PACKET_READY;
	p.ready = dynamic_cast<Player*>(players[p_id])->GetReady();
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

	Player* player = dynamic_cast<Player*>(players[p_id]);

	packet.x = player->GetX();
	packet.y = player->GetY();
	packet.z = player->GetZ();

	packet.pitch = player->GetPitch();
	packet.roll = player->GetRoll();
	packet.yaw = player->GetYaw();

	packet.vx = player->GetVX();
	packet.vy = player->GetVY();
	packet.vz = player->GetVZ();

	packet.flashlight = player->GetFlashlight();

	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}


int get_new_player_id()
{

	for (int i = 0; i < MAX_USER; ++i) {
		
		Player* player = dynamic_cast<Player*>(players[i]);
		player->Mutex_lock();
		
		if (STATE_FREE == player->GetState())
		{
			player->SetState(STATE_CONNECTED);

			player->Mutex_unlock();
			return i;
		}
		player->Mutex_unlock();
	}


	return -1;

}

bool is_npc(int id)
{
	return id > MAX_USER;
}

void player_move(int p_id, c2s_packet_move* move_packet)
{
	Player* player = dynamic_cast<Player*>(players[p_id]);
	player->SetX(move_packet->x);
	player->SetY(move_packet->y);
	player->SetZ(move_packet->z);

	player->SetYaw(move_packet->yaw);
	player->SetPitch(move_packet->pitch);
	player->SetRoll(move_packet->roll);

	player->SetVX(move_packet->vx);
	player->SetVY(move_packet->vy);
	player->SetVZ(move_packet->vz);

	player->SetFlashlight(move_packet->flashlight);


	for (int i = 0; i < players.size(); ++i)
	{
		Player* pl = dynamic_cast<Player*>(players[i]);

		if (pl->GetID() == p_id) continue;

		if (is_npc(pl->GetID())) continue;

		if (STATE_INGAME != pl->GetState()) continue;

		send_move_packet(pl->GetID(), p_id);

	}

}

void send_pc_login(int c_id, int p_id)
{
	s2c_packet_pc_login packet;
	packet.id = p_id;
	packet.size = sizeof(packet);

	Player* player = dynamic_cast<Player*>(players[p_id]);

	packet.type = S2C_PACKET_PC_LOGIN;
	packet.x = player->GetX();
	packet.y = player->GetY();
	packet.z = player->GetZ();

	//cout << p_id << endl;
	//cout << Clients[p_id].character.x << " " << Clients[p_id].character.y << " " << Clients[p_id].character.z << endl;

	packet.yaw = player->GetYaw();
	packet.pitch = player->GetPitch();
	packet.roll = player->GetRoll();

	packet.vx = player->GetVX();
	packet.vy = player->GetVY();
	packet.vz = player->GetVZ();

	//packet.flashlight = players[p_id].character.flashlight;

	strcpy_s(packet.name, player->GetGameID());

	

	packet.o_type = 0;
	send_packet(c_id, &packet);
}