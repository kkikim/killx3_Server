#pragma once
#include "protocol.h"

class CRoom
{
	//Hash_Table or Vector
public:
	CRoom();
	~CRoom();
	void Init();
	void InitIce();
	void InitRemain();

	bool CheckFullRoom();			//Ǯ������ �ƴ��� �˻�, FALSE�� ������, TRUE�� ����ִ°�
	void PushClient(CLIENT*);
	bool FindClient(int);
	void DestroyData();				//������ �������� �� �濡 �����͵� �ʱ�ȭ
	INT	 GetClientNum();			//Ŭ���̾�Ʈ ���� ��ȯ
	INT	 GetClientID(INT);
	void SetRoomState(bool);
	bool GetRoomState();
	void SetClientsNumber();		//Ŭ���̾�Ʈ������ ���� �濡 �ִ¾ֵ� ���̵� ����.

	void NPCUpdate(float,HANDLE,CLIENT*);
	void NPCHp(WORD, int);

	void SetStageState(int);

	bool StageCheck();
	void SetStageCheckVar();

	int GetCurrentStage();
	void SetCurrentState1(bool enable);
	void SetCurrentState2(bool enable);
	void SetCurrentState3(bool enable);

	vector<class CCaveNormalNPC*>	GetCaveStage1_NPC_Vector();
	vector<class CCaveMiddleNPC*>	GetCaveStage2_NPC_Vector();
	vector<class CCaveBossNPC*>		GetCaveStage3_NPC_Vector();

	vector<class CIceNormalNPC*>	GetIceStage1_NPC_Vector();
	vector<class CIceMiddleNPC*>	GetIceStage2_NPC_Vector();
	vector<class CIceBossNPC*>		GetIceStage3_NPC_Vector();

	vector<class CRemainNormalNPC*>	GetRemainStage1_NPC_Vector();
	vector<class CRemainMiddleNPC*>	GetRemainStage2_NPC_Vector();
	vector<class CRemainBossNPC*>	GetRemainStage3_NPC_Vector();


private:
	INT					mRoomSize;					//�ִ� ��� �ο���
	vector<CLIENT*>		clients;
	int					NextStageCheck;
	int					m_RoomNumber;
	BYTE				m_MapType;
	
	vector<class CCaveNormalNPC*>		CaveStage1NPCs;
	vector<class CCaveMiddleNPC*>		CaveStage2NPCs;
	vector<class CCaveBossNPC*>			CaveStage3NPCs;

	vector<class CIceNormalNPC*>		IceStage1NPCs;
	vector<class CIceMiddleNPC*>		IceStage2NPCs;
	vector<class CIceBossNPC*>			IceStage3NPCs;

	vector<class CRemainNormalNPC*>		RemainStage1NPCs;
	vector<class CRemainMiddleNPC*>		RemainStage2NPCs;
	vector<class CRemainBossNPC*>		RemainStage3NPCs;

	bool				is_gaming;
	bool				is_stage1;
	bool				is_stage2;
	bool				is_stage3;
	bool				is_End;
};

