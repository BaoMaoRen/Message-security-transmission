#include "ClientOperation.h"
#include "RequestCodec.h"
#include "FactoryCodec.h"
#include "RequestFactory.h"
#include "RespondFactory.h"

#include <cstring>
#include <iostream>
#include <openssl/hmac.h>
#include <openssl/sha.h>

using namespace std;

//构造函数
ClientOperation::ClientOperation(ClientInfo * info)
{
	//参数检查
	if (NULL == info)
	{
		cout << "Info is NULL" << endl;
	}

	//创建客户端配置信息对象
	mInfo = new ClientInfo;
	memcpy(mInfo, info, sizeof(ClientInfo));

	//创建套接字对象
	mSocket = new TcpSocket;

	//创建共享内存业务类对象
	mShm = new SecKeyShm(mInfo->shmKey, mInfo->maxNode);

}

//析构函数
ClientOperation::~ClientOperation()
{
	delete mInfo;
	delete mSocket;
	delete mShm;

	mInfo = NULL;
	mSocket = NULL;
	mShm = NULL;
}

//密钥协商
int ClientOperation::secKeyAgree()
{
	int ret = -1;

	RequestMsg requestMsg;

	//1. 组织密钥协商请求报文
	memset(&requestMsg, 0, sizeof(RequestMsg));
	//报文类型
	requestMsg.cmdType = RequestCodec::NewOrUpdate;
	//客户端的编号
	strcpy(requestMsg.clientId, mInfo->clientID);
	//服务端编号
	strcpy(requestMsg.serverId, mInfo->serverID);

	//生成客户端随机数
	getRandString(sizeof(requestMsg.r1), requestMsg.r1);

	//1.1 生成认证码
	HMAC_CTX *ctx = HMAC_CTX_new();
	if (NULL == ctx)
	{
		cout << "HMAC_CTX_new failed..." << endl;
		return -1;
	}

	//1.2 初始化
	char key[64];
	//组织密钥
	memset(key, 0, sizeof(key));
	sprintf(key, "@%s+%s@", mInfo->clientID, mInfo->serverID);
	ret = HMAC_Init_ex(ctx, key, strlen(key), EVP_sha256(), NULL);
	if (0 == ret)
	{
		cout << "HMAC_Init_ex failed.. " << endl;
		return -1;
	}

	//1.3 添加数据
	ret = HMAC_Update(ctx, (unsigned char *)requestMsg.r1, strlen(requestMsg.r1));
	if (0 == ret)
	{
		cout << "HMAC_Update failed.. " << endl;
		return -1;
	}

	//1.4 计算结果
	unsigned char md[SHA256_DIGEST_LENGTH];
	unsigned int mdLen = -1;

	ret = HMAC_Final(ctx, md, &mdLen);
	if (0 == ret)
	{
		cout << "HMAC_Final failed.. " << endl;
		return -1;
	}

	//1.5 生成消息认证码
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&requestMsg.authCode[i * 2], "%02x", md[i]);
	}

	//1.6 释放空间
	HMAC_CTX_free(ctx);

	//2. 创建报文工厂对象
	FactoryCodec *factoryCodec = new RequestFactory; //1.free

	//3. 创建请求报文编码对象
	Codec *codec = factoryCodec->createCodec(&requestMsg); //2.free

	//4. 编码密钥协商请求报文
	char *outData = NULL;
	int outDataLen = 0;

	codec->msgEncode(&outData, outDataLen); //3.free

	delete factoryCodec;
	factoryCodec = NULL;

	delete codec;
	codec = NULL;


	//5. 连接到密钥协商服务端
	mSocket->connectToHost(mInfo->serverIP, mInfo->serverPort);

	//6. 发送密钥协商请求报文
	mSocket->sendMsg(outData, outDataLen);

	delete [] outData;
	outData = NULL;

	//7. 接收服务端响应报文
	char *recvData = NULL;
	int recvDataLen = 0;
	mSocket->recvMsg(&recvData, recvDataLen); //4.free


	//8. 创建密钥协商响应报文解码对象
	factoryCodec = new RespondFactory; //5. free

	codec = factoryCodec->createCodec(); //6.free

	//9. 解码密钥协商响应报文
	RespondMsg *respondMsg = static_cast<RespondMsg*>(codec->msgDecode(recvData, recvDataLen)); //7.free
	
	//10. 根据服务端响应码做出错误处理
	if (0 != respondMsg->rv)
	{
		cout << "密钥协商失败...." << endl;
		return -1;
	}

	delete [] recvData;
	recvData = NULL;

	delete factoryCodec;
	factoryCodec = NULL;


	//11. 生成密钥
	NodeShmInfo nodeShmInfo;
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	nodeShmInfo.status = 0;
	nodeShmInfo.seckeyID = 1;
	strcpy(nodeShmInfo.clientID, mInfo->clientID);
	strcpy(nodeShmInfo.serverID, mInfo->serverID);

	//通过哈希算法模拟生成密钥信息 64Byte  0x1B --> "1B"
	//11.1 初始化
	SHA512_CTX c;
	ret = SHA512_Init(&c);
	if (0 == ret)
	{
		cout << "SHA512_Init failed..." << endl;
		return -1;
	}

	//11.2 添加数据
	ret = SHA512_Update(&c, requestMsg.r1, strlen(requestMsg.r1));
	if (0 == ret)
	{
		cout << "SHA512_Update failed..." << endl;
		return -1;
	}

	ret = SHA512_Update(&c, respondMsg->r2, strlen(respondMsg->r2));
	if (0 == ret)
	{
		cout << "SHA512_Update failed..." << endl;
		return -1;
	}

	//11.3 计算hash值
	unsigned char shaMd[SHA512_DIGEST_LENGTH];
	memset(shaMd, 0, SHA512_DIGEST_LENGTH);
	ret = SHA512_Final(shaMd, &c);
	if (0 == ret)
	{
		cout << "SHA512_Final failed..." << endl;
		return -1;
	}

	//11.4 合成密钥信息
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		sprintf(&nodeShmInfo.secKey[i * 2], "%02x", shaMd[i]);
	}

	//12. 写入共享内存
	mShm->shmWrite(&nodeShmInfo);

	codec->msgMemFree((void **)&respondMsg);


	delete codec;
	codec = NULL;

	//13. 断开连接
	mSocket->disConnect();

	return 0;
}

//密钥校验
int ClientOperation::secKeyCheck()
{
	return 0;
}

//密钥注销
int ClientOperation::secKeyRevoke()
{
	return 0;
}

//密钥查看
int ClientOperation::secKeyView()
{
	return 0;
}

//获取随机字符串
void ClientOperation::getRandString(int len, char * randBuf)
{
	//数字 0 - 9
	//大写字母 A - Z
	//小写字母 a - z
	//特殊字符 ~!@#$%^&*()-+=

	int tag = 0;
	int i = 0;

	//参数检查
	if (len <= 0 || NULL == randBuf)
	{
		cout << "len 或者 randBuf 非法" << endl;
		return;
	}

	//设置随机种子
	srandom(time(NULL));

	for (i = 0; i < len; i++)
	{
		//随机字符的种类
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
			randBuf[i] = "~!@#$%^&*()-+="[random() % 13];
			break;
		}
	}
}
