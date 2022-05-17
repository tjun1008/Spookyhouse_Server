#pragma once

#include "protocol.h"
#include "Player.h"


class IocpServer
{
private:
	


	vector<thread*>	Workerthread;
	thread			Acceptthread;
	thread			t_thread;
	

public:
	IocpServer();
	~IocpServer();
public:
	bool Initialize();
	void Progress();
	void Release();

public:
	void LoadCharacter(); // +ÃßÈÄ¿£ LoadObject

public:
	void do_accept(SOCKET s_socket, EX_OVER* a_over);
	void display_error(const char* msg, int err_no);
};