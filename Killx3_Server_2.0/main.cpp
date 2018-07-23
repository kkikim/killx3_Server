#include "CServer.h"
int main()
{
	printf("KILL KILL KILL Server Running..\n");

	CServer::GetInstance()->Run();
	return 0;
}

// 키 눌렀을때만 마우스 1초당 4번 갱신되게