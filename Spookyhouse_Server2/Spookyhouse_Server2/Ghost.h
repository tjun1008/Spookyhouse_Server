#pragma once

#include "protocol.h"

map<int, Ghost> Ghosts;

class Ghost
{
public:
	Ghost();
	virtual ~Ghost();

	// �÷��̾�� �̵�
	void MoveTo(const CharacterPacket& target);
	// �÷��̾� Ÿ��
	void HitPlayer(CharacterPacket& target);
	// ����ִ��� ����
	bool IsAlive();
	// ���������� ����
	bool IsAttacking();
	// �÷��̾ �߰� ������ �ִ���
	bool IsPlayerInTraceRange(const CharacterPacket& target);
	// �÷��̾ Ÿ�� ������ �ִ���
	bool IsPlayerInHitRange(const CharacterPacket& target);
	// ��ġ ����
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

	float	X;				// X��ǥ
	float	Y;				// Y��ǥ
	float	Z;				// Z��ǥ
	float	Health;			// ü��
	int		Id;				// ���� id
	float	TraceRange;		// �߰� ����
	float	HitRange;		// Ÿ�� ����
	float	MovePoint;		// �̵� ����Ʈ
	float	HitPoint;		// Ÿ�� ����Ʈ	
	bool	bIsAttacking;	// ����������	

private:
	bool	bIsTracking;	// �߰�������
};

/*
class GhostSet
{
public:
	map<int, Ghost> Ghosts;

};
*/