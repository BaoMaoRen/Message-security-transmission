#pragma once
#include "TcpSocket.h"
#include "SecKeyShm.h"
#include "TcpServer.h"
#include "RequestCodec.h"
#include "OCCIOP.h"


//服务端配置信息
class ServerInfo
{
public:
	char			serverID[12];	// 服务器端编号
	char			dbUser[24]; 	// 数据库用户名
	char			dbPasswd[24]; 	// 数据库密码
	char			dbSID[24]; 		// 数据库sid

	unsigned short 	sPort;			// 服务器绑定的端口
	int				maxNode;		// 共享内存最大网点数 客户端默认1个
	int 			shmkey;			// 共享内存keyid 创建共享内存时使用	 
};

class ServerOperation
{
public:
	ServerOperation(ServerInfo *info);
	~ServerOperation();

	// 服务器开始工作
	void startWork();
	// 秘钥协商
	int secKeyAgree(RequestMsg* reqmsg, char** outData, int& outLen);
	// 秘钥校验
	int secKeyCheck(RequestMsg* reqmsg, char** outData, int& outLen);
	// 秘钥注销
	int secKeyRevoke(RequestMsg* reqmsg, char** outData, int& outLen);
	// 秘钥查看
	int secKeyView(void);

    //静态数据成员
    static int flag;
private:
	//获取随机序列
	void getRandString(int len, char* randBuf);

private:
	ServerInfo *mInfo;
	SecKeyShm	*mShm;
	TcpServer *mServer;
	TcpSocket* mClient;
    OCCIOP *mOp;
};


