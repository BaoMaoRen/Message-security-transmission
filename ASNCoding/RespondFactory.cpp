#include "RespondFactory.h"

RespondFactory::RespondFactory()
{
}

RespondFactory::~RespondFactory()
{
}

//创建响应报文编解码对象的工厂 成员方法
Codec * RespondFactory::createCodec(void * arg)
{
	Codec *codec = NULL;

	if (NULL == arg)
	{
		//创建响应报文解码对象
		codec = new RespondCodec;
	}
	else
	{
		//创建响应报文编码对象
		codec = new RespondCodec(static_cast<RespondMsg*>(arg));
	}

	return codec;
}
