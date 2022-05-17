#pragma once

#include "Header.h"

class Object
{
protected:
	int				m_id;
	EX_OVER			m_recv_over;
	unsigned char	m_prev_recv;
	

	atomic<S_STATE>	m_state; // 0. free 1. connected 2. ingame
	mutex m_lock;
	char	m_name[MAX_NAME];

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
	void SetId(const int n) { m_id = n; }
	void SetX(const short n) { x = n; }
	void SetY(const short n) { y = n; }
	void SetZ(const short n) { z = n; }

	void SetYaw(const short n) { yaw = n; }
	void SetPitch(const short n) { pitch = n; }
	void SetRoll(const short n) { roll = n; }

	void SetVX(const short n) { vx = n; }
	void SetVY(const short n) { vy = n; }
	void SetVZ(const short n) { vz = n; }
public:
	const int GetID() const { return m_id; }
	const short GetX() const { return x; }
	const short GetY() const { return y; }
	const short GetZ() const { return z; }

	const short GetYaw() const { return yaw; }
	const short GetPitch() const { return pitch; }
	const short GetRoll() const { return roll; }

	const short GetVX() const { return vx; }
	const short GetVY() const { return vy; }
	const short GetVZ() const { return vz; }

};

