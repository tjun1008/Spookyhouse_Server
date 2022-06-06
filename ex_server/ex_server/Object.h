#pragma once

#include "Header.h"

class Object
{
public:
	//mutex
	mutex m_lock;

protected:
	
	EX_OVER			m_recv_over;
	unsigned char	m_prev_recv;
	

	//atomic<bool>	m_state; // 0. free 1. connected 2. ingame

	bool m_useble;
	
	char	m_name[MAX_NAME];
	int				m_id;

	// 위치
	short x, y, z;

	// 회전값
	short yaw, pitch, roll;

	// 속도
	short vx, vy, vz;
	
	

public:
	Object();
	virtual ~Object();

public:
	
	void SetX(const short n) { x = n; }
	void SetY(const short n) { y = n; }
	void SetZ(const short n) { z = n; }

	void SetYaw(const short n) { yaw = n; }
	void SetPitch(const short n) { pitch = n; }
	void SetRoll(const short n) { roll = n; }

	void SetVX(const short n) { vx = n; }
	void SetVY(const short n) { vy = n; }
	void SetVZ(const short n) { vz = n; }

	void SetState(const bool& state) { m_useble = state; } //포인터니까 주소값 쓰기

public:
	
	const short GetX() const { return x; }
	const short GetY() const { return y; }
	const short GetZ() const { return z; }

	const short GetYaw() const { return yaw; }
	const short GetPitch() const { return pitch; }
	const short GetRoll() const { return roll; }

	const short GetVX() const { return vx; }
	const short GetVY() const { return vy; }
	const short GetVZ() const { return vz; }

	bool& GetState() { return m_useble; }

};

