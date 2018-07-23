#include "CaveNormalNPC.h"

CCaveNormalNPC::CCaveNormalNPC(WORD ID, Vec3 _InitPos, float _Speed, BYTE _MoveState,
	float _Dist, int _HP, float _AttackDist, BYTE MapType)
{//생성자 인자로 방향도 초기화해저야되
	m_ID = ID;
	m_Target = -1;
	m_alive = true;
	m_Remove = false;
	m_bTarget = false;
	m_HP = _HP;
	m_BackDist = _Dist;
	m_MoveType = _MoveState;
	m_AnimationState = TRACKING;
	m_MapType = MapType;

	m_State = IDLE;			//IDLE,TRAKCING,ATTAKCK

	m_InitPos.x = _InitPos.x;
	m_Position.x = _InitPos.x;

	m_InitPos.y = _InitPos.y;
	m_Position.y = _InitPos.y;

	m_InitPos.z = _InitPos.z;
	m_Position.z = _InitPos.z;

	m_AttackDist = _AttackDist;
	MonsterSpeed = _Speed;
	OriginalMonsterSpeed = _Speed;
	m_RemoveTime = 0.0f;
	m_AttackTime = 0.0f;
}
CCaveNormalNPC::~CCaveNormalNPC() {}

void CCaveNormalNPC::SetID(int _ID) {
	m_ID = _ID;
}
Vec3 CCaveNormalNPC::GetPos() { return m_Position; }
bool CCaveNormalNPC::GetAlive() { return m_alive; }
Vec3 CCaveNormalNPC::GetInitPos() { return m_InitPos; };

void CCaveNormalNPC::Update(float DeltaTime, HANDLE _hIOCP, CLIENT * p_Clients, const int& ID1, const int& ID2)
{
	if (m_HP <= 0)
		m_alive = false;

	Sleep(1);

	m_Position += m_Direction * DeltaTime * MonsterSpeed;

	int mIDOne = ID1;
	int mIDTwo = ID2;

	if (!m_alive)
	{
		m_RemoveTime += DeltaTime;
		MonsterSpeed = 0.0f;
		if (m_RemoveTime > 5.0f)
			m_Remove = true;

		Sleep(1);

		OverlapEx* send_over = new OverlapEx;

		send_over->operation = OP_NPC_DEATH;
		send_over->ID = m_ID;
		send_over->MapType = m_MapType;

		PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));

		return;
	}

	if (m_AttackTime >= 2.0f)
	{
		m_AttackTime = 0.0f;
		m_MoveType = 0;
	}

	if (m_MoveType == 2)
	{
		m_AttackTime += DeltaTime;
		return;
	}

	if (m_MoveType == 1)
		return;

	if (IDLE == m_State)
	{
		Sleep(1);
		m_AnimationState = NPC_WALK;
		if (DestPoint.size() != 0)
		{
			if (DestPoint.front().Distance(m_Position) <= 1.75f)
			{
				m_Position = DestPoint.front();
				DestPoint.push(DestPoint.front());
				DestPoint.pop();
				Vec3 DestinationDir = DestPoint.front() - m_Position;
				m_Direction = DestinationDir.Normalize();

				Sleep(1);

				OverlapEx* send_over = new OverlapEx;

				send_over->operation = OP_SEND_NPC_DIR;
				send_over->ID = m_ID;
				PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));

			}
		}

		if ((m_Position.Distance(p_Clients[mIDOne].player.m_vPosition) < 20.0f) && !m_bTarget)
		{
			if (m_MoveType == 1)
				m_State = ATTACK;
			else
				m_State = TRACKING;

			m_Target = mIDOne;
			m_bTarget = true;
			Vec3 DestinationDir = p_Clients[m_Target].player.m_vPosition - m_Position;
			m_Direction = DestinationDir.Normalize();

			Sleep(1);

			OverlapEx *send_over = new OverlapEx;

			send_over->operation = OP_SEND_NPC_DIR;
			send_over->ID = m_ID;
			PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));

		}

		else if ((m_Position.Distance(p_Clients[mIDTwo].player.m_vPosition) < 20.0f) && !m_bTarget)
		{
			if (m_MoveType == 1)
				m_State = ATTACK;
			else
				m_State = TRACKING;

			m_Target = mIDTwo;
			m_bTarget = true;

			Vec3 DestinationDir = p_Clients[m_Target].player.m_vPosition - m_Position;
			m_Direction = DestinationDir.Normalize();

			Sleep(1);

			OverlapEx *send_over = new OverlapEx;

			send_over->operation = OP_SEND_NPC_DIR;
			send_over->ID = m_ID;
			PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));

		}

	}

	else if (TRACKING == m_State)
	{
		if (p_Clients[m_Target].player.m_vPosition.Distance(m_Position) <= m_AttackDist)
		{
			m_State = ATTACK;
			m_AnimationState = NPC_ATTACK;

			Vec3 DestinationDir = p_Clients[m_Target].player.m_vPosition - m_Position;
			m_Direction = DestinationDir.Normalize();

			MonsterSpeed = 0.0f;

			Sleep(1);

			OverlapEx *send_over = new OverlapEx;
			send_over->operation = OP_SEND_NPC_DIR;
			send_over->ID = m_ID;
			PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));
		}

		else
		{
			MonsterSpeed = OriginalMonsterSpeed;

			m_AnimationState = NPC_WALK;

				if (DestPoint.front().Distance(m_Position) > m_BackDist)
				{
					m_State = GOBACK;

					Vec3 DestDir = DestPoint.front() - m_Position;
					m_Direction = DestDir.Normalize();
					m_bTarget = false;

					Sleep(1);

					OverlapEx *send_over = new OverlapEx;

					send_over->operation = OP_SEND_NPC_DIR;
					send_over->ID = m_ID;
					PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));
				}

				else if (DestPoint.front().Distance(m_Position) <= m_BackDist && m_bTarget)
				{
					Vec3 DestinationDir = p_Clients[m_Target].player.m_vPosition - m_Position;
					m_Direction = DestinationDir.Normalize();

					Sleep(1);

					OverlapEx *send_over = new OverlapEx;

					send_over->operation = OP_SEND_NPC_DIR;
					send_over->ID = m_ID;
					PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));
				}
			
		}
	}

	else if (m_State == GOBACK)
	{
		m_AnimationState = NPC_WALK;

		if (DestPoint.front().Distance(m_Position) <= 1.75f)
		{
			m_State = IDLE;
		}

		Vec3 DestDir = DestPoint.front() - m_Position;
		m_Direction = DestDir.Normalize();

		Sleep(1);

		OverlapEx *send_over = new OverlapEx;

		send_over->operation = OP_SEND_NPC_DIR;
		send_over->ID = m_ID;
		PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));
	}

	else if (ATTACK == m_State)
	{
		if (p_Clients[m_Target].player.m_vPosition.Distance(m_Position) > m_AttackDist)
		{
			if (m_MoveType == 1)
			{
				m_State = ATTACK;
				MonsterSpeed = 0.0f;
				m_AnimationState = NPC_ATTACK;
			}
			else
			{
				m_State = TRACKING;
				MonsterSpeed = OriginalMonsterSpeed;
				m_AnimationState = NPC_WALK;
			}

			Vec3 DestinationDir = p_Clients[m_Target].player.m_vPosition - m_Position;
			m_Direction = DestinationDir.Normalize();

			Sleep(1);

			OverlapEx *send_over = new OverlapEx;

			send_over->operation = OP_SEND_NPC_DIR;
			send_over->ID = m_ID;
			PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));

		}

		if (DestPoint.front().Distance(m_Position) > m_BackDist)
		{
			if (m_MoveType != 1)
			{
				m_State = GOBACK;
				MonsterSpeed = OriginalMonsterSpeed;
				m_AnimationState = NPC_WALK;
			}

			Vec3 DestDir = DestPoint.front() - m_Position;
			m_Direction = DestDir.Normalize();
			m_bTarget = false;

			Sleep(1);

			OverlapEx *send_over = new OverlapEx;

			send_over->operation = OP_SEND_NPC_DIR;
			send_over->ID = m_ID;
			PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));
		}

		else
		{
			MonsterSpeed = 0.0f;
			m_AnimationState = NPC_ATTACK;
			m_MoveType = 2;

			Vec3 DestinationDir = p_Clients[m_Target].player.m_vPosition - m_Position;
			m_Direction = DestinationDir.Normalize();

			Sleep(1);

			OverlapEx *send_over = new OverlapEx;

			send_over->operation = OP_SEND_NPC_DIR;
			send_over->ID = m_ID;
			PostQueuedCompletionStatus(_hIOCP, 1, RoomNumber, &(send_over->original_overlap));
		}
	}

}
void CCaveNormalNPC::SetDestPoint(Vec3 _DestVec3) {
	DestPoint.push(_DestVec3);
}
void CCaveNormalNPC::SetAniState(BYTE _Ani)
{
	m_AnimationState = _Ani;
}
void CCaveNormalNPC::SetHP(int _Hp)
{
	m_HP -= _Hp;
}
void CCaveNormalNPC::SetBackDist(float dist)
{
	m_BackDist = dist;
}
void CCaveNormalNPC::SetRemove(bool remove)
{
	m_Remove = remove;
}
void CCaveNormalNPC::SetMapType(BYTE map)
{
	m_MapType = map;
}
queue<Vec3> CCaveNormalNPC::GetDestPoint() {
	return DestPoint;
}
void CCaveNormalNPC::SetDir(Vec3 _Dir) {
	m_Direction = _Dir;
}
void CCaveNormalNPC::SetSpeed(float _Speed)
{
	MonsterSpeed = _Speed;
}
void CCaveNormalNPC::SetRoomNumber(int _RoomNumber) {
	RoomNumber = _RoomNumber;
}
int CCaveNormalNPC::GetRoomNumber() {
	return RoomNumber;
}
BYTE CCaveNormalNPC::GetAniState()
{
	return m_AnimationState;
}
int CCaveNormalNPC::GetHP()
{
	return m_HP;
}
float CCaveNormalNPC::GetBackDist()
{
	return m_BackDist;
}
bool CCaveNormalNPC::GetRemove()
{
	return m_Remove;
}
Vec3 CCaveNormalNPC::GetDir() {
	return m_Direction;
}

float CCaveNormalNPC::GetSpeed()
{
	return MonsterSpeed;
}
