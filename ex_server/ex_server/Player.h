#pragma once

#include "Object.h"

class Player :public Object
{
private:
	SOCKET   m_s;


	int last_move_time;

	//lobby
	bool host;
	bool ready;

	// etc
	bool flashlight;


public:
	Player();
	virtual ~Player();
	void RecvPacket(int p_id);
	void ReadPacket(const UINT id, const int transferred, EX_OVER* ex_over);
	void AcceptPacket(const UINT& id, SOCKET c_socket);
	void Disconnect(int p_id, array <Object*, MAX_USER + 1> players);
	void do_recv(int p_id);


public:
	void Mutex_lock() { m_lock.lock(); }
	void Mutex_unlock() { m_lock.unlock(); }
	
	void SetGameID(char* name) {
		strcpy_s(m_name, name);
	}
	void SetHost(const bool state) { host = state; }
	void SetReady(const bool state) { ready = state; }
	void SetFlashlight(const bool n) { flashlight = n; }

	void SetSocket(const SOCKET& socket) { m_s = socket; } 
	void SetId(const int& n) { m_id = n; }

public:
	const SOCKET& GetSocket() const { return m_s; }
	
	const char* GetGameID() const { return m_name; }
	const bool GetHost() const { return host; }
	const bool GetReady() const { return ready; }
	const bool GetFlashlight() const { return flashlight; }

	SOCKET& GetSocket() { return m_s; }
	int& GetID() { return m_id; }
	
};




