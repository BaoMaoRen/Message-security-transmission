#include <iostream>
#include <cstring>
#include "SecKeyShm.h"

using namespace std;

#if 0
class NodeSHMInfo
{
public:
	int status;     //密钥状态
	int seckeyID;   //密钥编号
	char clientID[12];  //服务端ID
	char serverID[12];  //客户端ID
	char secKey[128];   //密钥信息
};
#endif

//写共享内存
int writeShm()
{
	SecKeyShm s(0x55, 10);

	NodeSHMInfo info;
	memset(&info, 0, sizeof(info));
	info.status = 1;
	info.seckeyID = 1;
	strcpy(info.clientID, "1111");
	strcpy(info.serverID, "0001");
	strcpy(info.secKey, "hello DD");

	s.shmWrite(&info);

	memset(&info, 0, sizeof(info));
	info.status = 1;
	info.seckeyID = 2;
	strcpy(info.clientID, "1112");
	strcpy(info.serverID, "0001");
	strcpy(info.secKey, "hello heima");

	s.shmWrite(&info);

	return 0;
}

//读共享内存
int readShm()
{
	SecKeyShm s(0x55, 10);
	NodeSHMInfo info;

	memset(&info, 0, sizeof(info));
	s.shmRead("1111", "0001", &info);
	cout << "status: " << info.status << endl;
	cout << "secKeyID: " << info.seckeyID << endl;
	cout << "clientId: " << info.clientID << endl;
	cout << "serverId: " << info.serverID << endl;
	cout << "secKey: " << info.secKey << endl;

	memset(&info, 0, sizeof(info));
	s.shmRead("1112", "0001", &info);
	cout << "status: " << info.status << endl;
	cout << "secKeyID: " << info.seckeyID << endl;
	cout << "clientId: " << info.clientID << endl;
	cout << "serverId: " << info.serverID << endl;
	cout << "secKey: " << info.secKey << endl;


	return 0;
}

int main(void)
{
	int choice;

	while (1)
	{
		cout << "please make a choice 1:read, 2:write" << endl;
		cin >> choice;

		if (1 == choice)
		{
			readShm();
		}

		if (2 == choice)
		{
			writeShm();
		}
	}

	cout << "hello DD" << endl;

	return 0;
}