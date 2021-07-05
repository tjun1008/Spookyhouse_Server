#include "main_iocp.h"
#include "Ghost.h"
#include <thread>


Ghost::Ghost()
	:X(0), Y(0), Z(0),
	Health(0), MovePoint(5), HitPoint(0.1f),
	TraceRange(500), HitRange(100),
	bIsAttacking(false),
	bIsTracking(false)
{
	// 임의로 정함, 고유 id
}


Ghost::~Ghost()
{
}

void Ghost::MoveTo(const CharacterPacket& target)
{
	if (target.x > X)
		X += MovePoint;
	if (target.x < X)
		X -= MovePoint;
	if (target.y > Y)
		Y += MovePoint;
	if (target.y < Y)
		Y -= MovePoint;
	if (target.z > Z)
		Z += MovePoint;
	if (target.z < Z)
		Z -= MovePoint;
}

void Ghost::HitPlayer(CharacterPacket& target)
{
	std::thread t([&]() {
		// 1초에 한번씩 때리도록		
		bIsAttacking = true;
		printf_s("때림\n");
		//target.hp -= HitPoint;
		std::this_thread::sleep_for(1s);
		bIsAttacking = false;
		});
	t.detach();
}

bool Ghost::IsAlive()
{
	if (Health <= 0)
		return false;

	return true;
}

bool Ghost::IsAttacking()
{
	return bIsAttacking;
}

bool Ghost::IsPlayerInTraceRange(const CharacterPacket& target)
{
	if (abs(target.x - X) < TraceRange && abs(target.y - Y) < TraceRange)
		return true;

	return false;
}

bool Ghost::IsPlayerInHitRange(const CharacterPacket& target)
{
	if (abs(target.x - X) < HitRange && abs(target.y - Y) < HitRange)
		return true;

	return false;
}

void Ghost::SetLocation(float x, float y, float z)
{
	X = x;
	Y = y;
	Z = z;
}