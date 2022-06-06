#include "stdafx.h" //�� ���� ���� ����!!
#include "iocp_server.h"
#include "Global.h"


//IocpServer* GIocpServer = nullptr;

IocpServer::IocpServer() : mIOCP(NULL), mListenSocket(NULL), mWorkerThreads()
{
}


IocpServer::~IocpServer()
{
}

bool IocpServer::Initialize()
{
	wcout.imbue(locale("korean"));

	CreateObjectPool();

	// begin : GetSystemInfo�� cpu�� ������ Ȯ���սô�

	/*
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	ThreadCount = si.dwNumberOfProcessors;

	// int ThreadCount

	*/

	// 1. winsock initializing 
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
		return false;


	// 2. Create I/O Completion Port

	mIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (mIOCP == NULL)
		return false;


	/// 3. create TCP socket
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mListenSocket == INVALID_SOCKET)
		return false;



	// 4. iocp ��ü�� ���� ����

	HANDLE handle = CreateIoCompletionPort((HANDLE)mListenSocket, mIOCP, 0, 0);
	if (handle != mIOCP)
	{
		printf_s("[DEBUG] listen socket IOCP register error: %d\n", GetLastError());
		return false;
	}

	// 5. ���� ���� ��ü ����

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //IP�ּҸ� �ڵ����� ã�Ƽ� ����

	//bind ���� ����
	int opt = 1;
	setsockopt(mListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));

	// 6. bind

	if (SOCKET_ERROR == bind(mListenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)))
		return false;


	return true;

}

bool IocpServer::StartWorkerThreads()
{
	mWorkerThreads.reserve(NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; ++i)
		mWorkerThreads.emplace_back(thread{ WorkerThread, (LPVOID)this });

	for (auto& th : mWorkerThreads) th.join();

	return true;
}

//�����忡�� ��� ������ ����ϱ� ����, Ŭ���� ���ο��� ��Ŀ�����忡 �ʿ��� �Լ��� ȣ��

void IocpServer::WorkerThread(LPVOID arg)
{
	IocpServer* pServer = static_cast<IocpServer*>(arg);
	pServer->WorkerThreadFunc();


};

void IocpServer::WorkerThreadFunc()
{
	//������ ������~~

	while (true) {
		DWORD num_byte;
		ULONG_PTR i_key;
		//WSAOVERLAPPED* over;

		EX_OVER* over;
		BOOL ret = GetQueuedCompletionStatus(mIOCP, &num_byte, &i_key, reinterpret_cast<LPOVERLAPPED*>(&over), INFINITE);
		int key = static_cast<int> (i_key);
		if (FALSE == ret) {
			int err = WSAGetLastError();
			//display_error("GQCS : ", err);
			Disconnect(key);
			continue;
		}

		EX_OVER* ex_over = reinterpret_cast<EX_OVER*>(over);

	}
}

void IocpServer::Disconnect(int p_id)
{
	Player* player = dynamic_cast<Player*>(players[p_id]);

	
	player->m_lock.lock();
	player->SetState(false);
	closesocket(player->GetSocket());
	player->m_lock.unlock();
	//players.erase(p_id);

	for (int i = 0; i < players.size(); ++i)
	{
		Player* pl = dynamic_cast<Player*>(players[i]);

		if (pl->GetID() == p_id) continue;

		if (false != pl->GetState())
			send_pc_logout(pl->GetID(), p_id);

	}

}

bool IocpServer::StartAcceptLoop()
{

	SOCKET clientSocket{};

	///7.listen
	if (SOCKET_ERROR == listen(mListenSocket, SOMAXCONN))
		return false;

	
	

	while (true)
	{
		clientSocket = accept(mListenSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "invalid socket\n";
			continue;
		}

		// id �޾ƿ��� �۾�

		int new_id = -1;
		
		for (int i = 0; i < MAX_USER; ++i)
		{
			// id�� ����ִ°����� �������ݴϴ�
			if (false == players[i]->GetState())
			{
				new_id = i;
				break;
			}
		}

		if (cur_player >= 4)
		{
			closesocket(mListenSocket);
			cout << "Max User" << endl;
		}

		if (true == bIsgameStart) {
			closesocket(mListenSocket);
			cout << "Game Already Start" << endl;

		}

		cout << "[" << new_id << "] ����" << endl;

		Player* player = dynamic_cast<Player*>(players[new_id]);

		if (cur_player == 0)
		{
			player->SetHost(true);
		}
		else
		{
			player->SetHost(false);
		}

		cur_player++;

		// id, socket ���� ����
		player->SetState(STATE_INGAME);
		player->SetSocket(clientSocket);
		player->SetId(new_id);

		//�ش� Ŭ���̾�Ʈ ���� ����, ������� ���Ѵ�

		send_pc_lobby(new_id, new_id);

		for (int i = 0; i < players.size(); ++i)
		{
			Player* pl = dynamic_cast<Player*>(players[i]);

			if (pl->GetID() == new_id) continue;

			if (false != pl->GetState()) continue;

			send_pc_lobby(pl->GetID(), new_id);
			send_pc_lobby(new_id, pl->GetID());

		}

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), mIOCP, new_id, 0);
	}

	return true;

}

void IocpServer::CreateObjectPool()
{
	//Ż��Ű �ʱ�ȭ
	KeyLocation loc1[6]; // = { {730, -1870, 70},{799, -1870, 70},{1661, -2061, 89},{1661, -1983, 70},{1428, 1512, 145},{1587, 1519, 144} };
	KeyLocation loc2[9];
	KeyLocation loc3[12];

	ifstream file;

	file.open("KeyLocation.txt");

	if (file.is_open() == false)
	{
		cout << "������ ���� ���߽��ϴ�" << endl;
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
	// ���Ϻҷ����� Ȯ�� �Ϸ�

	srand(time(NULL));

	int x1, x2, x3;

	x1 = rand() % 6;
	x2 = rand() % 9;
	x3 = rand() % 12;

	EscapeKey.push_back(KeyLocation(loc1[x1].x, loc1[x1].y, loc1[x1].z));
	EscapeKey.push_back(KeyLocation(loc2[x2].x, loc2[x2].y, loc2[x2].z));
	EscapeKey.push_back(KeyLocation(loc3[x3].x, loc3[x3].y, loc3[x3].z));


	cout << EscapeKey[0].x << " " << EscapeKey[0].y << " " << EscapeKey[0].z << endl;
	cout << EscapeKey[1].x << " " << EscapeKey[1].y << " " << EscapeKey[1].z << endl;
	cout << EscapeKey[2].x << " " << EscapeKey[2].y << " " << EscapeKey[2].z << endl;


	//Ŭ���̾�Ʈ �ʱ�ȭ

	for (int i = 0; i < MAX_USER; ++i)
	{
		// ������ ȣ���
		players[i] = new Player;
		//auto& pl = players[i];
		//players[i]->SetId(i);
		//pl.SetId(i);

	}

}

void IocpServer::Finalize()
{
	closesocket(mListenSocket);
	CloseHandle(mIOCP);

	// ���� ����
	WSACleanup();
}

void IocpServer::send_pc_lobby(int c_id, int p_id)
{
	s2c_packet_lobby packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_LOBBY;

	packet.host = dynamic_cast<Player*>(players[p_id])->GetHost();
	send_packet(c_id, &packet);
}

void IocpServer::send_packet(int p_id, void* buf)
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

void IocpServer::send_pc_logout(int c_id, int p_id)
{
	s2c_packet_pc_logout packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_PC_LOGOUT;
	send_packet(c_id, &packet);
}