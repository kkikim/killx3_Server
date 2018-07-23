#pragma once
#include "protocol.h"
//#include "CThreadManager.h"
#include <queue>
class CIceMiddleNPC
{
public:
	CIceMiddleNPC(WORD, Vec3, float, BYTE, float, int, float, BYTE);
	~CIceMiddleNPC();

	void Update(float, HANDLE, CLIENT*, const int&, const int&);

	Vec3 GetPos();
	Vec3 GetInitPos();
	bool GetAlive();
	Vec3 GetDir();
	float GetSpeed();
	int GetRoomNumber();
	BYTE GetAniState();
	int GetHP();
	float GetBackDist();
	bool GetRemove();
	void SetRoomNumber(int);

	void SetID(int);
	void SetDir(Vec3);
	void SetSpeed(float);
	void SetDestPoint(Vec3);
	void SetAniState(BYTE);
	void SetHP(int);
	void SetBackDist(float);
	void SetRemove(bool);
	void SetMapType(BYTE);

	queue<Vec3> GetDestPoint();
private:
	WORD m_ID;				// Monter ID
	int m_Target;			// Target ID
	bool m_bTarget;

	int RoomNumber;

	BYTE m_MoveType;			// 로머(0)인지, 말뚝(1)인지
	BYTE m_State;				// FSM STATE
	BYTE m_MonsterType;			// 거미인지 먼지
	BYTE m_AnimationState;
	bool m_alive;
	bool m_Remove;

	Vec3 m_InitPos;
	Vec3 m_Position;
	Vec3 m_Direction;
	float m_AttackDist;

	int m_HP;
	int m_Damage;
	float m_BackDist;
	float MonsterSpeed;
	float OriginalMonsterSpeed;
	float m_SendTime;
	float m_RemoveTime;
	float m_AttackTime;
	queue <Vec3> DestPoint;

	BYTE m_MapType;
};

