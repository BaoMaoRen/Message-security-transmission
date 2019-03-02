#include "Codec.h"

//构造函数
Codec::Codec():SequenceASN1()
{
}

//析构函数
Codec::~Codec()
{
}

//结构体编码
int Codec::msgEncode(char ** outData, int & len)
{
	return 0;
}

//结构体解码
void * Codec::msgDecode(char * inData, int inLen)
{
	return nullptr;
}

//内存释放
int Codec::msgMemFree(void ** point)
{
	return 0;
}
