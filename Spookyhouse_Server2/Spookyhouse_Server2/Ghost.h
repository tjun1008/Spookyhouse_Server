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