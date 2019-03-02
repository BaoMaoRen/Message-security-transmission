#include "RespondCodec.h"

#include <iostream>

using namespace std;


//构造响应报文解码对象
RespondCodec::RespondCodec()
{
	mRespondMsg = new RespondMsg;

	memset(mRespondMsg, 0, sizeof(RespondMsg));
}

//响应报文编码对象
RespondCodec::RespondCodec(RespondMsg * msg)
{
	mRespondMsg = new RespondMsg;

	memcpy(mRespondMsg, msg, sizeof(RespondMsg));
}

RespondCodec::~RespondCodec()
{
	delete mRespondMsg;
}

//响应报文编码
int RespondCodec::msgEncode(char ** outData, int & len)
{
	int ret = -1;

	//1. 编码rv
	ret = writeHeadNode(mRespondMsg->rv);
	if (0 != ret)
	{
		cout << "writeHeadNode failed..." << endl;
		return -1;
	}

	//2. 编码ClientId
	ret = writeNextNode(mRespondMsg->clientId, strlen(mRespondMsg->clientId));
	if (0 != ret)
	{
		cout << "writeNextNode failed..." << endl;
		return -1;
	}

	//3. 编码ServerId
	ret = writeNextNode(mRespondMsg->serverId, strlen(mRespondMsg->serverId));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//4. 编码r2
	ret = writeNextNode(mRespondMsg->r2, strlen(mRespondMsg->r2));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//5. 编码seckeyId
	ret = writeNextNode(mRespondMsg->seckeyid);
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//6. 编码结构体
	ret = packSequence(outData, len);
	if (0 != ret)
	{
		cout << "packSequence failed.." << endl;
		return -1;
	}

	//7. 传出

	//8. 释放内存  链表的内存
	freeSequence();


	return 0;
}

//响应报文解码
void * RespondCodec::msgDecode(char * inData, int inLen)
{
	int ret = -1;

	RespondMsg *pMsg = NULL;

	pMsg = new RespondMsg;
	memset(pMsg, 0, sizeof(RespondMsg));

	//1. 解码结构体
	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		cout << "unpackSequence failed.." << endl;
		return NULL;
	}

	//2. 解码rv
	ret = readHeadNode(pMsg->rv);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//3. 解码clientId
	ret = readNextNode(pMsg->clientId);
	if (0 != ret)
	{
		cout << "readNextNode failed..." << endl;
		return NULL;
	}

	//4. 解码ServerId
	ret = readNextNode(pMsg->serverId);
	if (0 != ret)
	{
		cout << "readNextNode failed..." << endl;
		return NULL;
	}


	//5. 解码r2
	ret = readNextNode(pMsg->r2);
	if (0 != ret)
	{
		cout << "readNextNode failed..." << endl;
		return NULL;
	}


	//6. 解码seckeyId
	ret = readNextNode(pMsg->seckeyid);
	if (0 != ret)
	{
		cout << "readNextNode failed..." << endl;
		return NULL;
	}

	//7. 传出

	//8. 释放内存 释放链表的空间
	freeSequence();


	return pMsg;
}

//释放结构体内存
int RespondCodec::msgMemFree(void ** data)
{
	//参数检查
	if (NULL == data || NULL == *data)
	{
		return -1;
	}

	//释放内存
	delete (RespondMsg*)*data;

	*data = NULL;

	return 0;
}
