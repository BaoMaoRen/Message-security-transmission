#define _CRT_SECURE_NO_WARNINGS
#include "RequestCodec.h"

#include <iostream>

using namespace std;


//构造请求报文解码对象
RequestCodec::RequestCodec():Codec()
{
	//分配空间
	mRequestMsg = new RequestMsg;

	//清零
	memset(mRequestMsg, 0, sizeof(RequestMsg));
}

//构造请求报文编码对象
RequestCodec::RequestCodec(RequestMsg * msg):Codec()
{
	//分配空间
	mRequestMsg = new RequestMsg;

	//赋值
	mRequestMsg->cmdType = msg->cmdType;
	strcpy(mRequestMsg->clientId, msg->clientId);
	strcpy(mRequestMsg->authCode, msg->authCode);
	strcpy(mRequestMsg->serverId, msg->serverId);
	strcpy(mRequestMsg->r1, msg->r1);

}

//析构函数
RequestCodec::~RequestCodec()
{
	delete mRequestMsg;
}

//请求报文编码方法
int RequestCodec::msgEncode(char ** outData, int & len)
{
	int ret = -1;

	//1. 编码cmdType
	ret = writeHeadNode(mRequestMsg->cmdType);
	if (0 != ret)
	{
		cout << "writeHeadNode failed.." << endl;
		return -1;
	}

	//2. 编码ClientID
	ret = writeNextNode(mRequestMsg->clientId, strlen(mRequestMsg->clientId));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//3. 编码authCode
	ret = writeNextNode(mRequestMsg->authCode, strlen(mRequestMsg->authCode));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//4. 编码ServerId
	ret = writeNextNode(mRequestMsg->serverId, strlen(mRequestMsg->serverId));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//5. 编码r1
	ret = writeNextNode(mRequestMsg->r1, strlen(mRequestMsg->r1));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//6. 编码结构体
	ret = packSequence(outData, len);
	if (0 != ret)
	{
		cout << "packSequence failed..." << endl;
		return -1;
	}

	//7. 传出

	//8. 释放内存 链表
	freeSequence(NULL);

	return 0;
}

//请求报文解码函数
void * RequestCodec::msgDecode(char * inData, int inLen)
{
	int ret = -1;

	RequestMsg *pMsg = NULL;

	//分配空间
	pMsg = new RequestMsg;
	memset(pMsg, 0 , sizeof(RequestMsg));

	//1. 解码结构体
	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		cout << "unpackSequence failed.." << endl;
		return NULL;
	}

	//2. 解码cmdType
	ret = readHeadNode(pMsg->cmdType);
	if (0 != ret)
	{
		cout << "readHeadNode failed.." << endl;
		return NULL;
	}

	//3. 解码clientId
	ret = readNextNode(pMsg->clientId);
	if (0 != ret)
	{
		cout << "readNextNode failed.." << endl;
		return NULL;
	}

	//4. 解码authCode
	ret = readNextNode(pMsg->authCode);
	if (0 != ret)
	{
		cout << "readNextNode failed.." << endl;
		return NULL;
	}

	//5. 解码serverId
	ret = readNextNode(pMsg->serverId);
	if (0 != ret)
	{
		cout << "readNextNode failed.." << endl;
		return NULL;
	}

	//6. 解码r1
	ret = readNextNode(pMsg->r1);
	if (0 != ret)
	{
		cout << "readNextNode failed.." << endl;
		return NULL;
	}

	//7. 传出

	//8. 释放内存
	freeSequence(NULL);

	return pMsg;
}

//结构体内存释放
int RequestCodec::msgMemFree(void ** point)
{
	//参数检查
	if (NULL == point || NULL == *point)
	{
		return -1;
	}

	delete (RequestMsg*)*point;

	*point = NULL;

	return 0;
}
