#pragma once
#include "protocol.h"
#include "CRoom.h"
#include "CThreadManager.h"
#include <iostream>
#include <vector>
using namespace std;
class CPacketMgr
{
public:
	static CPacketMgr* GetInstance();

	CPacketMgr();
	~CPacketMgr();

	void SendPacket(int, unsigned char*, CLIENT * clients);
	void ProcessPacket(int, unsigned char *, CLIENT *);
	void error_display(char *msg, int err_num);


	void MakeCaveRoom(int, unsigned char, CLIENT *);
	void CaveNextStage(int , unsigned char  , CLIENT * );

	void MakeIceRoom(int, unsigned char, CLIENT *);
	void IceNextStage(int, unsigned char, CLIENT *);

	void MakeRemainRoom(int, unsigned char, CLIENT *);
	void RemainNextStage(int, unsigned char, CLIENT *);

	//void KeyChange(BYTE);
	CRoom* GetRoom(int);			//Get Cave Room Class
	CRoom* GetIceRoom(int);
	CRoom* GetRemainRoom(int);

	void SetKeyState(int i, bool enable);
	bool GetKeyState(int i);
private:
	CPacketMgr(const CPacketMgr& other) {}
	static CPacketMgr* instance;

	vector<CRoom> mCaveRoom;
	vector<CRoom> mIceRoom;
	vector<CRoom> mRemainRoom;
	bool		KeyState[2];

};

