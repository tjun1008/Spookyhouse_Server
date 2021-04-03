#pragma once

#include "protocol.h"

map<int, Ghost> Ghosts;

class Ghost
{
public:
	Ghost();
	virtual ~Ghost();

	// 플레이어로 이동
	void MoveTo(const CharacterPacket& target);
	// 플레이어 타격
	void HitPlayer(CharacterPacket& target);
	// 살아있는지 여부
	bool IsAlive();
	// 공격중인지 여부
	bool IsAttacking();
	// 플레이어가 추격 범위에 있는지
	bool IsPlayerInTraceRange(const CharacterPacket& target);
	// 플레이어가 타격 범위에 있는지
	bool IsPlayerInHitRange(const CharacterPacket& target);
	// 위치 설정
	void SetLocation(float x, float y, float z);

	friend ostream& operator<<(ostream& stream, Ghost& info)
	{
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.Health << endl;
		stream << info.Id << endl;
		stream << info.bIsAttacking << endl;

		return stream;
	}

	friend istream& operator>>(istream& stream, Ghost& info)
	{
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.Health;
		stream >> info.Id;
		stream >> info.bIsAttacking;

		return stream;
	}

	float	X;				// X좌표
	float	Y;				// Y좌표
	float	Z;				// Z좌표
	float	Health;			// 체력
	int		Id;				// 고유 id
	float	TraceRange;		// 추격 범위
	float	HitRange;		// 타격 범위
	float	MovePoint;		// 이동 포인트
	float	HitPoint;		// 타격 포인트	
	bool	bIsAttacking;	// 공격중인지	

private:
	bool	bIsTracking;	// 추격중인지
};

/*
class GhostSet
{
public:
	map<int, Ghost> Ghosts;

};
*/