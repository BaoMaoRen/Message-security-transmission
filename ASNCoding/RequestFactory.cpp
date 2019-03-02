#include "RequestFactory.h"



//构造函数
RequestFactory::RequestFactory()
{
}

//析构函数
RequestFactory::~RequestFactory()
{
}

//创建请求报文编码对象或者解码对象
Codec * RequestFactory::createCodec(void * arg)
{
	Codec *codec = NULL;

	//创建请求报文解码对象
	if (NULL == arg)
	{
		codec = new RequestCodec;
	}
	else
	{
		codec = new RequestCodec(static_cast<RequestMsg*>(arg));
	}

	return codec;
}
