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

	bool CheckFullRoom();			//풀방인지 아닌지 검사, FALSE면 꽉찬거, TRUE면 비어있는거
	void PushClient(CLIENT*);
	bool FindClient(int);
	void DestroyData();				//게임이 끝났으니 이 방에 데이터들 초기화
	INT	 GetClientNum();			//클라이언트 숫자 반환
	INT	 GetClientID(INT);
	void SetRoomState(bool);
	bool GetRoomState();
	void SetClientsNumber();		//클라이언트들한테 같은 방에 있는애들 아이디 셋팅.

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
	INT					mRoomSize;					//최대 허용 인원수
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

