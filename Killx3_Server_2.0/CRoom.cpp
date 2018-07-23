#include "CRoom.h"
#include "CaveNormalNPC.h"
#include "CaveMiddleNPC.h"
#include "CaveBossNPC.h"
#include "IceNormalNPC.h"
#include "IceMiddleNPC.h"
#include "PacketMgr.h"
#include "IceBossNPC.h"
#include "RemainNormalNPC.h"
#include "RemainMiddleNPC.h"
#include "RemainBossNPC.h"

CRoom::CRoom(){}
CRoom::~CRoom(){}

void CRoom::Init(){
	is_gaming = false;
	is_stage1 = false;
	is_stage2 = false;
	is_stage3 = false;
	is_End = false;
	NextStageCheck = 0;

	printf("1\n");
	m_MapType = MAP_CAVE;
	
	CaveStage1NPCs.reserve(4);
	// 아이디, 위치, 스피드, 움직임상태, 포기하고 돌아갈 최대거리, HP, 공격시작 거리
	CaveStage1NPCs.push_back(new CCaveNormalNPC(0, Vec3{ 120.0f,1.8f,60.0f }, 5.0f, 0, 15.0f, 500.0f, 8.5f, MAP_CAVE));		//500
	CaveStage1NPCs.push_back(new CCaveNormalNPC(1, Vec3{ 123.0f,2.5f,119.0f}, 5.0f, 0, 15.0f, 500.0f, 8.5f, MAP_CAVE));
	CaveStage1NPCs.push_back(new CCaveNormalNPC(2, Vec3{ 171.0f,2.2f,179.0f}, 0.0f, 1, 15.0f, 500.0f, 8.5f, MAP_CAVE));
	CaveStage1NPCs.push_back(new CCaveNormalNPC(3, Vec3{ 182.0f,0.52f,210.0f }, 5.0f, 0, 10.0f, 500.0f, 8.5f, MAP_CAVE));

	CaveStage2NPCs.reserve(3);
	CaveStage2NPCs.push_back(new CCaveMiddleNPC(0, Vec3{ 106.0f, 1.06f, 74.0f }, 2.0f, 0, 15.0f, 600.0f, 4.0f,MAP_CAVE));		//600
	CaveStage2NPCs.push_back(new CCaveMiddleNPC(1, Vec3{ 142.6f, 1.98f, 90.0f }, 2.0f, 0, 15.0f, 600.0f, 4.0f, MAP_CAVE));
	CaveStage2NPCs.push_back(new CCaveMiddleNPC(2, Vec3{ 102.0f, 3.02f, 150.0f }, 2.0f, 0, 15.0f, 600.0f, 4.0f, MAP_CAVE));

	CaveStage3NPCs.push_back(new CCaveBossNPC(0,Vec3{ 128.0f, 0.0f, 220.0f }, 3.0f, 0, 50.0f, 2000.0f, 7.0f, MAP_CAVE));		//2000
}
void CRoom::InitIce()
{
	is_gaming = false;
	is_stage1 = false;
	is_stage2 = false;
	is_stage3 = false;
	is_End = false;
	NextStageCheck = 0;

	printf("2\n");
	m_MapType = MAP_ICE;

	IceStage1NPCs.reserve(4);
	// 아이디, 위치, 스피드, 움직임상태, 포기하고 돌아갈 최대거리, HP, 공격시작 거리
	IceStage1NPCs.push_back(new CIceNormalNPC(0, Vec3{ 100.f, 0.0f, 100.0f }, 3.0f, 0, 20.0f, 500.0f, 5.0f, MAP_ICE));		//500
	IceStage1NPCs.push_back(new CIceNormalNPC(1, Vec3{ 150.0f, 0.0f, 75.0f }, 3.0f, 0, 20.0f, 500.0f, 5.0f, MAP_ICE));
	IceStage1NPCs.push_back(new CIceNormalNPC(2, Vec3{ 150.0f, 0.0f, 140.0f }, 3.0f, 0, 20.0f, 500.0f, 5.0f, MAP_ICE));
	IceStage1NPCs.push_back(new CIceNormalNPC(3, Vec3{ 125.0f, 0.0f, 225.0f }, 3.0f, 0, 20.0f, 500.0f, 5.0f, MAP_ICE));

	IceStage2NPCs.reserve(3);
	IceStage2NPCs.push_back(new CIceMiddleNPC(0, Vec3{ 100.0f, 0.0f, 90.0f }, 5.0f, 0, 20.0f, 600.0f, 5.0f, MAP_ICE));		//600
	IceStage2NPCs.push_back(new CIceMiddleNPC(1, Vec3{ 150.0f, 0.0f, 120.0f }, 5.0f, 0, 20.0f, 600.0f, 5.0f, MAP_ICE));
	IceStage2NPCs.push_back(new CIceMiddleNPC(2, Vec3{ 130.0f, 0.0f, 180.0f }, 5.0f, 0, 20.0f, 600.0f, 5.0f, MAP_ICE));

	IceStage3NPCs.push_back(new CIceBossNPC(0, Vec3{ 128.0f, 0.0f, 130.0f }, 7.0f, 0, 100.0f, 2000.0f, 7.0f, MAP_ICE));
}
void CRoom::InitRemain()
{
	is_gaming = false;
	is_stage1 = false;
	is_stage2 = false;
	is_stage3 = false;
	is_End = false;
	NextStageCheck = 0;

	printf("333\n");
	m_MapType = MAP_REMAIN;

	RemainStage1NPCs.reserve(4);
	// 아이디, 위치, 스피드, 움직임상태, 포기하고 돌아갈 최대거리, HP, 공격시작 거리
	RemainStage1NPCs.push_back(new CRemainNormalNPC(0, Vec3{ 120.0f, 0.0f, 55.0f }, 10.0f, 0, 20.0f, 500.0f, 9.0f,MAP_REMAIN));		//500
	RemainStage1NPCs.push_back(new CRemainNormalNPC(1, Vec3{ 84.0f, 0.0f, 150.0f }, 10.0f, 0, 20.0f, 500.0f, 9.0f, MAP_REMAIN));
	RemainStage1NPCs.push_back(new CRemainNormalNPC(2, Vec3{ 172.0f, 0.0f, 150.0f }, 10.0f, 0, 20.0f, 500.0f, 9.0f, MAP_REMAIN));
	RemainStage1NPCs.push_back(new CRemainNormalNPC(3, Vec3{ 120.0f, 0.0f, 204.0f }, 10.0f, 0, 20.0f, 500.0f, 9.0f, MAP_REMAIN));

	RemainStage2NPCs.reserve(3);
	RemainStage2NPCs.push_back(new CRemainMiddleNPC(0, Vec3{ 128.0f, 0.0f, 80.0f }, 3.0f, 0, 20.0f, 600.0f, 6.5f, MAP_REMAIN));		//600
	RemainStage2NPCs.push_back(new CRemainMiddleNPC(1, Vec3{ 165.0f, 0.0f, 178.0f }, 3.0f, 0, 20.0f, 600.0f, 6.5f, MAP_REMAIN));
	RemainStage2NPCs.push_back(new CRemainMiddleNPC(2, Vec3{ 91.0f, 0.0f, 178.0f }, 3.0f, 0, 20.0f, 600.0f, 6.5f, MAP_REMAIN));

	RemainStage3NPCs.push_back(new CRemainBossNPC(0, Vec3{ 128.0f, 0.0f, 180.0f }, 3.0f, 0, 50.0f, 2000.0f, 9.0f, MAP_REMAIN));
}
bool CRoom::StageCheck() {
	NextStageCheck++;
	if (NextStageCheck >= 2)
		return true;
	return false;
}
void CRoom::SetStageCheckVar(){
	NextStageCheck = 0;
}

bool CRoom::CheckFullRoom()			//꽉찼으면 false반환, 안찼으면 true 반환
{
	if (clients.size() >= 2)
		return true;
	else
		return false;
}
void CRoom::PushClient(CLIENT * client)
{
	if (clients.size()<2)			//일단은 2인용 차후 최대 4인으로 변경 예정
		clients.push_back(client);
}

bool CRoom::FindClient(int client_id)
{
	for (int i = 0; i < clients.size(); ++i){
		if (clients[i]->id == client_id)
			return true;
		else
			continue;
	}
	return false;

}

INT CRoom::GetClientNum()
{
	return clients.size();
}
INT CRoom::GetClientID(INT i)
{
	return clients[i]->id;
}
void CRoom::SetRoomState(bool state)
{
	is_gaming = state;
}
bool CRoom::GetRoomState()		//게임중 - True // 아닐시 -false
{
	return is_gaming;
}
void CRoom::SetClientsNumber()
{
	for (int i = 0; i < clients.size(); ++i)
	{
		for (int j = 0; j < clients.size(); ++j)
		{
			clients[i]->roomList.push_back(clients[j]->id);
		}
	}
}
void CRoom::DestroyData()
{
	clients[0]->is_gaming = false;
	clients[1]->is_gaming = false;
	printf("1\n");
	CaveStage1NPCs.clear();
	CaveStage2NPCs.clear();
	CaveStage3NPCs.clear();
	printf("2\n");
	IceStage1NPCs.clear();
	IceStage2NPCs.clear();
	IceStage3NPCs.clear();
	printf("3\n");
	RemainStage1NPCs.clear();
	RemainStage2NPCs.clear();
	RemainStage3NPCs.clear();

	CPacketMgr::GetInstance()->GetRoom(0)->SetRoomState(false);
	CPacketMgr::GetInstance()->GetRoom(0)->SetCurrentState1(false);
	CPacketMgr::GetInstance()->GetRoom(0)->SetCurrentState2(false);
	CPacketMgr::GetInstance()->GetRoom(0)->SetCurrentState3(false);
	printf("4\n");
	CPacketMgr::GetInstance()->GetIceRoom(0)->SetRoomState(false);
	CPacketMgr::GetInstance()->GetIceRoom(0)->SetCurrentState1(false);
	CPacketMgr::GetInstance()->GetIceRoom(0)->SetCurrentState2(false);
	CPacketMgr::GetInstance()->GetIceRoom(0)->SetCurrentState3(false);
	printf("5\n");
	CPacketMgr::GetInstance()->GetRemainRoom(0)->SetRoomState(false);
	CPacketMgr::GetInstance()->GetRemainRoom(0)->SetCurrentState1(false);
	CPacketMgr::GetInstance()->GetRemainRoom(0)->SetCurrentState2(false);
	CPacketMgr::GetInstance()->GetRemainRoom(0)->SetCurrentState3(false);
	is_gaming = false;
	is_stage1 = false;
	is_stage2 = false;
	is_stage3 = false;
	printf("6\n");
	clients.clear(); 
	printf("7\n");
}

void CRoom::SetStageState(int num)
{
	if (num == 1) {
		is_stage1 = true;
		is_stage2 = false;
		is_stage3 = false;
	}
	if (num == 2) {
		is_stage1 = false;
		is_stage2 = true;
		is_stage3 = false;
	}
	if (num == 3) {
		is_stage1 = false;
		is_stage2 = false;
		is_stage3 = true;
	}
}

void CRoom::NPCUpdate(float DeltaTime,HANDLE _hIOCP,CLIENT* p_Clients)
{
	if (true == is_stage1) 
	{
		for (int i = 0; i < 4; ++i) 
		{
			if (m_MapType == MAP_CAVE)
				CaveStage1NPCs[i]->Update(DeltaTime, _hIOCP, p_Clients, clients[0]->id, clients[1]->id);
			else if (m_MapType == MAP_ICE)
				IceStage1NPCs[i]->Update(DeltaTime, _hIOCP, p_Clients, clients[0]->id, clients[1]->id);
			else if (m_MapType == MAP_REMAIN)
				RemainStage1NPCs[i]->Update(DeltaTime, _hIOCP, p_Clients, clients[0]->id, clients[1]->id);
		}
	}

	else if (true == is_stage2) 
	{
		for (int i = 0; i < 3; ++i) 
		{
			if (m_MapType == MAP_CAVE)
				CaveStage2NPCs[i]->Update(DeltaTime, _hIOCP, p_Clients, clients[0]->id, clients[1]->id);
			else if (m_MapType == MAP_ICE)
				IceStage2NPCs[i]->Update(DeltaTime, _hIOCP, p_Clients, clients[0]->id, clients[1]->id);
			else if (m_MapType == MAP_REMAIN)
				RemainStage2NPCs[i]->Update(DeltaTime, _hIOCP, p_Clients, clients[0]->id, clients[1]->id);
		}
	}

	else if (true == is_stage3) 
	{
		if (m_MapType == MAP_CAVE)
			CaveStage3NPCs[0]->Update(DeltaTime, _hIOCP, p_Clients, clients[0]->id, clients[1]->id);
		else if (m_MapType == MAP_ICE)
			IceStage3NPCs[0]->Update(DeltaTime, _hIOCP, p_Clients, clients[0]->id, clients[1]->id);
		else if (m_MapType == MAP_REMAIN)
			RemainStage3NPCs[0]->Update(DeltaTime, _hIOCP, p_Clients, clients[0]->id, clients[1]->id);
	}
}
void CRoom::NPCHp(WORD _Id, int _Hp)
{
	if (true == is_stage1)
	{
		if(m_MapType == MAP_CAVE)
			CaveStage1NPCs[_Id]->SetHP(_Hp);
		else if (m_MapType == MAP_ICE)
			IceStage1NPCs[_Id]->SetHP(_Hp);
		else if (m_MapType == MAP_REMAIN)
			RemainStage1NPCs[_Id]->SetHP(_Hp);
	}
		
	else if (true == is_stage2)
	{
		if (m_MapType == MAP_CAVE)
			CaveStage2NPCs[_Id]->SetHP(_Hp);
		else if (m_MapType == MAP_ICE)
			IceStage2NPCs[_Id]->SetHP(_Hp);
		else if (m_MapType == MAP_REMAIN)
			RemainStage2NPCs[_Id]->SetHP(_Hp);
	}

	else if (true == is_stage3)
	{
		if (m_MapType == MAP_CAVE)
			CaveStage3NPCs[_Id]->SetHP(_Hp);
		else if (m_MapType == MAP_ICE)
			IceStage3NPCs[_Id]->SetHP(_Hp);
		else if (m_MapType == MAP_REMAIN)
			RemainStage3NPCs[_Id]->SetHP(_Hp);
	}
	
}
int CRoom::GetCurrentStage()
{
	if (true == is_stage1)
		return 1;
	else if (true == is_stage2)
		return 2;
	else if (true == is_stage3)
		return 3;
	else if (!is_stage1 && !is_stage2 && !is_stage3)
		return -1;
}
void CRoom::SetCurrentState1(bool enable)
{
	is_stage1 = enable;
}
void CRoom::SetCurrentState2(bool enable)
{
	is_stage2 = enable;
}
void CRoom::SetCurrentState3(bool enable)
{
	is_stage3 = enable;
}
vector<class CCaveNormalNPC*> CRoom::GetCaveStage1_NPC_Vector() { return CaveStage1NPCs; }
vector<class CCaveMiddleNPC*> CRoom::GetCaveStage2_NPC_Vector() { return CaveStage2NPCs; }
vector<class CCaveBossNPC*> CRoom::GetCaveStage3_NPC_Vector() { return CaveStage3NPCs; }

vector<class CIceNormalNPC*> CRoom::GetIceStage1_NPC_Vector()
{
	return IceStage1NPCs;
}

vector<class CIceMiddleNPC*> CRoom::GetIceStage2_NPC_Vector()
{
	return IceStage2NPCs;
}

vector<class CIceBossNPC*> CRoom::GetIceStage3_NPC_Vector()
{
	return IceStage3NPCs;
}

vector<class CRemainNormalNPC*> CRoom::GetRemainStage1_NPC_Vector()
{
	return RemainStage1NPCs;
}

vector<class CRemainMiddleNPC*> CRoom::GetRemainStage2_NPC_Vector()
{
	return RemainStage2NPCs;
}

vector<class CRemainBossNPC*> CRoom::GetRemainStage3_NPC_Vector()
{
	return RemainStage3NPCs;
}
