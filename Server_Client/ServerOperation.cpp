#include "ServerOperation.h"

#include <iostream>
#include <pthread.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "RequestFactory.h"
#include "RespondFactory.h"

using namespace std;

//线程传递参数给线程处理函数
typedef struct _pthread_info_t {
	//TCP客户端对象指针
	TcpSocket *tcpSocket;
	//密钥协商服务端对象指针
	ServerOperation *serverOperation;
}pthread_info_t;

void *myroute(void *arg);

//静态数据成员初始化
int ServerOperation::flag = 0;

//构造函数
ServerOperation::ServerOperation(ServerInfo * info)
{
	if (NULL == info)
	{
		cout << "ServerOperation info is NULL" << endl;
		return;
	}
	//创建服务端配置信息对象
	mInfo = new ServerInfo;
	memset(mInfo, 0, sizeof(ServerInfo));
	memcpy(mInfo, info, sizeof(ServerInfo));

	//创建共享内存业务类对象
	mShm = new SecKeyShm(mInfo->shmkey, mInfo->maxNode);

	//创建服务端类对象
	mServer = new TcpServer;

    mOp = new OCCIOP;

    mOp->connectDB(mInfo->dbUser, mInfo->dbPasswd, "192.168.20.42:1521/orcl");

}

//析构函数
ServerOperation::~ServerOperation()
{
	delete mInfo;
	delete mShm;
	delete mServer;

	mInfo = NULL;
	mShm = NULL;
	mServer = NULL;
}

//启动服务端
void ServerOperation::startWork()
{
    int ret = -1;

	pthread_t tid = -1;

	pthread_info_t *pInfo;

	//1. 设置监听
	ret = mServer->setListen(mInfo->sPort);
    if (0 == ret)
    {
	    cout << "服务端处于监听状态" << endl;
    }
    else
    {
        cout << "服务端设置监听失败..." << endl; 
    }

	//2. 循环接受客户端连接
	while (1)
	{
        //标志
        if (ServerOperation::flag == 1)
        {
            cout << "密钥协商服务端友好退出" << endl;
            break; 
        }

		mClient = mServer->acceptConn(3);
        if (NULL == mClient)
        {
            cout << "acceptConn 超时或者失败" << endl; 
            continue;
        }

		//内存冗余法
		pInfo = new pthread_info_t;
		pInfo->tcpSocket = mClient;
		pInfo->serverOperation = this;

		//3. 创建一个线程
		pthread_create(&tid, NULL, myroute, (void *)pInfo);

		//4. 设置线程分离
		pthread_detach(tid);
	}

	//服务端关闭套接字
	mServer->closefd();
}

//密钥协商
int ServerOperation::secKeyAgree(RequestMsg * reqmsg, char ** outData, int & outLen)
{
    int keyId = -1;
	int ret = -1;

	RespondMsg respondMsg;
	//1. 组织密钥协商响应报文
	memset(&respondMsg, 0, sizeof(RespondMsg));
	respondMsg.rv = 0;
	strcpy(respondMsg.clientId, reqmsg->clientId);
	strcpy(respondMsg.serverId, reqmsg->serverId);

	//生成随机序列
	getRandString(sizeof(respondMsg.r2),respondMsg.r2);

    //从数据库中获取
    keyId = mOp->getKeyID();
	respondMsg.seckeyid = keyId;


	//2. 创建响应报文工厂类对象
	FactoryCodec *factoryCodec = new RespondFactory; //1.free

	//3. 创建响应报文编码的对象
	Codec *codec = factoryCodec->createCodec((void*)&respondMsg); //2.free

	//4. 编码响应报文
	codec->msgEncode(outData, outLen);

	//5. 传出

	//6. 生成密钥信息  通过哈希算法模拟生成 Sha512
	NodeShmInfo nodeShmInfo;
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	nodeShmInfo.status = 1;
	nodeShmInfo.seckeyID = respondMsg.seckeyid;
	strcpy(nodeShmInfo.clientID, respondMsg.clientId);
	strcpy(nodeShmInfo.serverID, respondMsg.serverId);

	//6.1 初始化
	SHA512_CTX c;
	ret = SHA512_Init(&c);
	if (0 == ret)
	{
		cout << "SHA512_Init failed..." << endl;
		return -1;
	}

	//6.2 添加数据
	ret = SHA512_Update(&c, reqmsg->r1, strlen(reqmsg->r1));
	if (0 == ret)
	{
		cout << "SHA512_Update failed..." << endl;
		return -1;
	}

	ret = SHA512_Update(&c, respondMsg.r2, strlen(respondMsg.r2));
	if (0 == ret)
	{
		cout << "SHA512_Update failed..." << endl;
		return -1;
	}

	//6.3 计算hash值
	unsigned char shaMd[SHA512_DIGEST_LENGTH];
	memset(shaMd, 0, SHA512_DIGEST_LENGTH);
	ret = SHA512_Final(shaMd, &c);
	if (0 == ret)
	{
		cout << "SHA512_Final failed..." << endl;
		return -1;
	}

	//6.4 合成密钥信息
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		sprintf(&nodeShmInfo.secKey[i * 2], "%02x", shaMd[i]);
	}

	//7. 写共享内存
	mShm->shmWrite(&nodeShmInfo);

	//8. 写数据库
    mOp->writeSecKey(&nodeShmInfo); 

    mOp->updataKeyID(keyId + 1);

	//9. 释放内存
	if (NULL != factoryCodec)
	{
		delete factoryCodec;
		factoryCodec = NULL;
	}

	if (NULL != codec)
	{
		delete codec;
		codec = NULL;
	}

	return 0;
}

//密钥校验
int ServerOperation::secKeyCheck(RequestMsg * reqmsg, char ** outData, int & outLen)
{
	return 0;
}

//密钥注销
int ServerOperation::secKeyRevoke(RequestMsg * reqmsg, char ** outData, int & outLen)
{
	return 0;
}

//密钥查看
int ServerOperation::secKeyView(void)
{
	return 0;
}

//获取随机序列
void ServerOperation::getRandString(int len, char * randBuf)
{
	//数字 0 - 9
	//大写字母 A-Z
	//小写字母 a-z
	//特殊字符 !@#$%^&*()_+-=
	int tag;

	//参数检查
	if (len <= 0 || NULL == randBuf)
	{
		cout << "GetRandString parameters are invalid" << endl;
		return;
	}

	//设置随机种子
	srandom(time(NULL));

	for (int i = 0; i < len; i++)
	{
		//随机字符种类
		tag = random() % 4;

		switch (tag)
		{
			//数字
		case 0:
			randBuf[i] = '0' + random() % 10;
			break;
			//大写字母
		case 1:
			randBuf[i] = 'A' + random() % 26;
			break;

			//小写字母
		case 2:
			randBuf[i] = 'a' + random() % 26;
			break;

			//特殊字符
		case 3:
			randBuf[i] = "!@#$%^&*()_+-="[random() % 14];
			break;
		}
	}
}

//线程处理函数  和客户端进行数据交互
void *myroute(void *arg)
{
	int ret = -1;

	pthread_info_t *pInfo = static_cast<pthread_info_t*>(arg);

	char *recvData = NULL;
	int recvDataLen = -1;

	//1. 接收客户端请求报文
	pInfo->tcpSocket->recvMsg(&recvData, recvDataLen); //1.free

	//2. 创建请求报文类工厂对象
	FactoryCodec *factoryCodec = new RequestFactory; //2.free

	//3. 创建请求报文类解码对象
	Codec *codec = factoryCodec->createCodec(); //3.free

	//4. 解码请求报文
	RequestMsg *requestMsg = static_cast<RequestMsg*>(codec->msgDecode(recvData, recvDataLen)); //4.free


	delete factoryCodec;


	//5. 验证消息认证码
	//5.1 生成认证码
	HMAC_CTX *ctx = HMAC_CTX_new();
	if (NULL == ctx)
	{
		cout << "HMAC_CTX_new failed..." << endl;
		return (void*)-1;
	}

	//5.2 初始化
	char key[64];
	//组织密钥
	memset(key, 0, sizeof(key));
	sprintf(key, "@%s+%s@", requestMsg->clientId, requestMsg->serverId);
	ret = HMAC_Init_ex(ctx, key, strlen(key), EVP_sha256(), NULL);
	if (0 == ret)
	{
		cout << "HMAC_Init_ex failed.. " << endl;
		return (void *)-1;
	}

	//5.3 添加数据
	ret = HMAC_Update(ctx, (unsigned char *)requestMsg->r1, strlen(requestMsg->r1));
	if (0 == ret)
	{
		cout << "HMAC_Update failed.. " << endl;
		return (void*)-1;
	}

	//5.4 计算结果
	unsigned char md[SHA256_DIGEST_LENGTH];
	unsigned int mdLen = -1;

	ret = HMAC_Final(ctx, md, &mdLen);
	if (0 == ret)
	{
		cout << "HMAC_Final failed.. " << endl;
		return (void *)-1;
	}

	//5.5 生成消息认证码
	char authCode[65];
	memset(authCode, 0, 65);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&authCode[i * 2], "%02x", md[i]);
	}

    cout << "authCode: " << authCode << endl;

	//1.6 释放空间
	HMAC_CTX_free(ctx);

	//1.7 验证消息认证码
	if (strcmp(authCode, requestMsg->authCode) == 0)
	{
		cout << "消息认证码验证ok..." << endl;
	}
	else
	{
		cout << "消息认证码验证失败..." << endl;
		return (void*)-1;
	}

	//6. 根据请求类型做出对应的响应 cmdType
	char *outData = NULL;
	int outDataLen = -1;

	switch (requestMsg->cmdType)
	{
		//密钥协商
	case RequestCodec::NewOrUpdate:
		ret = pInfo->serverOperation->secKeyAgree(requestMsg, &outData, outDataLen); //5. free
		break;
		//密钥校验
	case RequestCodec::Check:
		ret = pInfo->serverOperation->secKeyCheck(requestMsg, &outData, outDataLen);
		break;
		//密钥注销
	case RequestCodec::Revoke:
		ret = pInfo->serverOperation->secKeyRevoke(requestMsg, &outData, outDataLen);
		break;
	default:
		cout << "无效的请求类型" << endl;
	}

	//根据返回值判断处理结果
	if (0 != ret)
	{
		cout << "做对应的业务失败.." << endl;
		return (void*)-1;
	}
	else
	{
		cout << "做对应的业务成功..." << endl;
	}
	
	//7. 发送响应报文
	pInfo->tcpSocket->sendMsg(outData, outDataLen);

	//8. 断开连接
	pInfo->tcpSocket->disConnect();

	//9. 释放内存
	codec->msgMemFree((void**)&requestMsg);

	delete codec;

	delete recvData;

	delete outData;

	delete pInfo;


	return NULL;
}
