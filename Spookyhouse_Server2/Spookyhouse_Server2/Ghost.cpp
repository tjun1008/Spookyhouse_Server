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
	// ���Ƿ� ����, ���� id
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
		// 1�ʿ� �ѹ��� ��������		
		bIsAttacking = true;
		printf_s("����\n");
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