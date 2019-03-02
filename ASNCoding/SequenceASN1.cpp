#define _CRT_SECURE_NO_WARNINGS
#include "SequenceASN1.h"

#include <iostream>

using namespace std;

//构造函数
SequenceASN1::SequenceASN1()
{
}

//析构函数
SequenceASN1::~SequenceASN1()
{
}

//编码第一个节点 整型
int SequenceASN1::writeHeadNode(int iValue)
{
	DD_INT ret = -1;

	ret = DER_ItAsn1_WriteInteger(iValue, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteInteger failed.." << endl;
		return -1;
	}

	//永远指向链表的最后一个节点
	m_next = m_header;

	return 0;
}

//编码第一个节点 char*类型
int SequenceASN1::writeHeadNode(char * sValue, int len)
{
	DD_INT ret = -1;

	//将char*类型转化为DD_ANYBUF*类型
	ret = DER_DD_String_To_AnyBuf(&m_temp, (unsigned char*)sValue, len);
	if (0 != ret)
	{
		cout << "DER_DD_String_To_AnyBuf failed..." << endl;
		return -1;
	}

	ret = DER_ItAsn1_WritePrintableString(m_temp, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WritePrintableString failed..." << endl;
		return -1;
	}

	//next指向最后一个节点
	m_next = m_header;

	//释放内存
	DER_DD_FreeQueue(m_temp);
	m_temp = NULL;

	return 0;
}

//编码后继节点
int SequenceASN1::writeNextNode(int iValue)
{
	DD_INT ret = -1;

	ret = DER_ItAsn1_WriteInteger(iValue, &(m_next->next));
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteInteger failed.." << endl;
		return -1;
	}

	//next指向链表的最后一个节点
	m_next = m_next->next;

	return 0;
}

//编码后继节点
int SequenceASN1::writeNextNode(char * sValue, int len)
{
	int ret = -1;

	//编码char*
	ret = EncodeChar(sValue, len, &(m_next->next));
	if (0 != ret)
	{
		cout << "EncodeChar failed..." << endl;
		return -1;
	}

	//next指向链表的最后一个节点
	m_next = m_next->next;

	return 0;
}

//解码头结点 整型
int SequenceASN1::readHeadNode(int & iValue)
{
	DD_INT ret = -1;

	ret = DER_ItAsn1_ReadInteger(m_header, (DD_UINT32 *)&iValue);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadInteger failed..." << endl;
		return -1;
	}

	//指向链表的下一个节点
	m_next = m_header->next;

	return 0;
}

int SequenceASN1::readHeadNode(char * sValue)
{
	DD_INT ret = -1;

	ret = DER_ItAsn1_ReadPrintableString(m_header, &m_temp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadPrintableString failed.." << endl;
		return -1;
	}

	//拷贝数据
	memcpy(sValue, m_temp->pData, m_temp->dataLen);

	//指向链表的下一个节点
	m_next = m_header->next;

	//释放内存
	DER_DD_FreeQueue(m_temp);
	m_temp = NULL;


	return 0;
}

//解码后继节点
int SequenceASN1::readNextNode(int & iValue)
{
	DD_INT ret = -1;

	ret = DER_ItAsn1_ReadInteger(m_next, (DD_UINT32 *)&iValue);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadInteger failed..." << endl;
		return -1;
	}

	//指向下一个节点
	m_next = m_next->next;

	return 0;
}

//解码后继节点
int SequenceASN1::readNextNode(char * sValue)
{
	DD_INT ret = -1;

	ret = DER_ItAsn1_ReadPrintableString(m_next, &m_temp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadPrintableString failed.." << endl;
		return -1;
	}

	//拷贝数据
	memcpy(sValue, m_temp->pData, m_temp->dataLen);

	//指向下一个节点
	m_next = m_next->next;

	//释放内存
	DER_DD_FreeQueue(m_temp);
	m_temp = NULL;

	return 0;
}

//编码结构体
int SequenceASN1::packSequence(char ** outData, int & outLen)
{
	DD_INT ret = -1;

	char *out = NULL;

	ret = DER_ItAsn1_WriteSequence(m_header, &m_temp);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_WriteSequence failed..." << endl;
		return -1;
	}

	//分配空间
	out = new char[m_temp->dataLen + 1];
	if (NULL == out)
	{
		cout << "packSequence new char failed..." << endl;
		return -1;
	}

	//拷贝数据
	memcpy(out, m_temp->pData, m_temp->dataLen);

	//传出
	*outData = out;
	outLen = m_temp->dataLen;

	//释放内存
	DER_DD_FreeQueue(m_temp);
	m_temp = NULL;

	return 0;
}

//解码结构体
int SequenceASN1::unpackSequence(char * inData, int inLen)
{
	DD_INT ret = -1;

	//将char*转化为DD_ANYBUF类型
	ret = DER_DD_String_To_AnyBuf(&m_temp, (unsigned char *)inData, inLen);
	if (0 != ret)
	{
		cout << "DER_DD_String_To_AnyBuf failed..." << endl;
		return -1;
	}

	//解码结构体
	ret = DER_ItAsn1_ReadSequence(m_temp, &m_header);
	if (0 != ret)
	{
		cout << "DER_ItAsn1_ReadSequence failed..." << endl;
		return -1;
	}

	//释放内存
	DER_DD_FreeQueue(m_temp);
	m_temp = NULL;

	return 0;
}

//释放链表
void SequenceASN1::freeSequence(DD_ANYBUF * node)
{
	if (NULL == node)
	{
		//释放链表的内存
		DER_DD_FreeQueue(m_header);
	}
	else
	{
		//释放节点的内存
		DER_DD_FreeQueue(node);
	}

}
