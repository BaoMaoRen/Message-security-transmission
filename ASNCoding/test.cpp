#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include "RequestFactory.h"

using namespace std;

int mywritefile(unsigned char *buf, int len)
{
	FILE *fp = NULL;
	fp = fopen("D:/teacher.ber", "wb+");
	if (fp == NULL)
	{
		printf("fopen file error \n");
		return -1;
	}
	fwrite(buf, 1, len, fp);
	fclose(fp);
	return 0;
}



int main()
{
	//编码相关的 参数
	char *outData = NULL;
	int outDataLen = 0;

	RequestMsg msg;
	memset(&msg, 0, sizeof(msg));
	msg.cmdType = 1;
	strcpy(msg.clientId, "0001");
	strcpy(msg.authCode, "0000");
	strcpy(msg.serverId, "1111");
	strcpy(msg.r1, "1234567890");

	//创建一个工厂类的对象
	FactoryCodec *factoryCodec = new RequestFactory;

	//通过简单工厂对象创建请求报文编码对象
	Codec *codec = factoryCodec->createCodec((void*)&msg);
	
	//1. 编码
	codec->msgEncode(&outData, outDataLen);

	cout << outData << " " << outDataLen << endl;
	mywritefile((unsigned char *)outData, outDataLen);

	delete codec;

	//2. 解码
	//通过简单工厂对象创建请求报文解码对象
	codec = factoryCodec->createCodec();

	RequestMsg *pMsg = static_cast<RequestMsg*>(codec->msgDecode(outData, outDataLen));
	cout << pMsg->cmdType << endl;
	cout << pMsg->clientId << endl;
	cout << pMsg->authCode << endl;
	cout << pMsg->serverId << endl;
	cout << pMsg->r1 << endl;

	//3. 释放内存
	codec->msgMemFree((void **)&pMsg);

	delete codec;

	cout << "hello DD" << endl;

	system("pause");
	return 0;
}
