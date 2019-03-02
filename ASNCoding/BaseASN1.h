#ifndef BASEASN1_H
#define BASEASN1_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DDLog.h"

// #define DDDER_NoErr 0
// 自定义基础数据类型
typedef int              DD_INT;
typedef unsigned char    DD_UINT8;
typedef unsigned short   DD_UINT16;
typedef unsigned long    DD_UINT32;
typedef signed long      DD_SINT32;
typedef unsigned char    ITASN1_BOOLEAN;

// 数据节点结构体(类)
typedef struct DD_ANYBUF_
{
    DD_UINT8 	  *pData;
    DD_UINT32     dataLen;

    DD_UINT32     unusedBits;	/* for bit string */
    DD_UINT32     memoryType;
    DD_UINT32     dataType;
    struct DD_ANYBUF_ *next;    /* for sequence and set */
    struct DD_ANYBUF_ *prev;
}DD_ANYBUF;

// 自定义复合数据类型
typedef DD_ANYBUF    ITASN1_INTEGER;
typedef DD_ANYBUF    ITASN1_OCTETSTRING;
typedef DD_ANYBUF    ITASN1_BITSTRING;
typedef DD_ANYBUF    ITASN1_PRINTABLESTRING;
/*begin of bmpstring*/
typedef DD_ANYBUF	 ITASN1_BMPSTRING;
/*end of bmpstring*/
typedef DD_ANYBUF    ITASN1_ENUMERATED;
typedef DD_ANYBUF    ITASN1_IA5STRING;
typedef DD_ANYBUF    ITASN1_SEQUENCE;
typedef DD_ANYBUF    ITASN1_SET;

class BaseASN1
{
public:
    // 标记内存类型
    enum MemoryType{StaticMemory=1, MallocMemory=2};
    // 错误描述
    enum DerErrType{
        NoErr=0,
        MemoryErr=200,
        LengthErr,
        LengthNotEqual,
        DataRangeErr,
        InvalidTag
    };
    // 整形数的每一位代表不同的数据类型0-31, 即: tag的值对应的数据类型
    enum DerTag{
        DD_DER_ID_RESERVED,
        DD_DER_ID_BOOLEAN,
        DD_DER_ID_INTEGER,
        DD_DER_ID_BITSTRING,
        DD_DER_ID_OCTETSTRING,
        DD_DER_ID_NULL,
        DD_DER_ID_OBJECT_IDENTIFIER,
        DD_DER_ID_OBJECT_DESCRIPTOR,
        DD_DER_ID_EXTERNAL,
        DD_DER_ID_REAL,
        DD_DER_ID_ENUMERATED,
        DD_DER_ID_EMBEDDLogED_PDV,
        DD_DER_ID_STRING_UTF8,
        DD_DER_ID_13,
        DD_DER_ID_14,
        DD_DER_ID_15,
        DD_DER_ID_SEQUENCE,
        DD_DER_ID_SET,
        DD_DER_ID_STRING_NUMERIC,
        DD_DER_ID_STRING_PRINTABLE,
        DD_DER_ID_STRING_T61,
        DD_DER_ID_STRING_VIDEOTEX,
        DD_DER_ID_STRING_IA5,
        DD_DER_ID_TIME_UTC,
        DD_DER_ID_TIME_GENERALIZED,
        DD_DER_ID_STRING_GRAPHIC,
        DD_DER_ID_STRING_ISO646,
        DD_DER_ID_STRING_GENERAL,
        DD_DER_ID_STRING_UNIVERSAL,
        DD_DER_ID_29,
        DD_DER_ID_STRING_BMP
    };

    // 构造函数
    BaseASN1();

    DD_INT DER_ItAsn1_WriteInteger(DD_UINT32 integer, ITASN1_INTEGER **ppDerInteger);

    DD_INT DER_ItAsn1_ReadInteger(ITASN1_INTEGER *pDerInteger, DD_UINT32 *pInteger);

    DD_INT DER_ItAsn1_WriteBitString(ITASN1_BITSTRING *pBitString, ITASN1_BITSTRING **ppDerBitString);

    DD_INT DER_ItAsn1_ReadBitString(ITASN1_BITSTRING *pDerBitString, ITASN1_BITSTRING **ppBitString);

    DD_INT DER_ItAsn1_WritePrintableString(ITASN1_PRINTABLESTRING *pPrintString, ITASN1_PRINTABLESTRING **ppDerPrintString);

    DD_INT DER_ItAsn1_ReadPrintableString(ITASN1_PRINTABLESTRING *pDerPrintString, ITASN1_PRINTABLESTRING **ppPrintString);

    DD_INT DER_ItAsn1_WriteSequence(ITASN1_SEQUENCE *pSequence, DD_ANYBUF **ppDerSequence);

    DD_INT DER_ItAsn1_ReadSequence(DD_ANYBUF *pDerSequence, ITASN1_SEQUENCE **ppSequence);

    DD_INT DER_ItAsn1_WriteNull(DD_ANYBUF ** ppDerNull);

    DD_INT DER_ItAsn1_ReadNull(DD_ANYBUF * ppDerNull, DD_UINT8 * pInt);

    // 释放一个序列(链表), pAnyBuf为链表的头结点
    DD_INT DER_DD_FreeQueue(DD_ANYBUF *pAnyBuf);

    // 创建DD_ANYBUF, 将strOrigin写入创建的DD_ANYBUF内存中, 通过pOriginBuf将内存地址传出
    DD_INT DER_DD_String_To_AnyBuf(DD_ANYBUF **pOriginBuf, unsigned char * strOrigin, int strOriginLen);

    int WriteNullSequence(DD_ANYBUF **pOutData);

    // 同 EncodeChar 函数
    int EncodeUnsignedChar(unsigned char *pData, int dataLen, DD_ANYBUF **outBuf);

    int DecodeUnsignedChar(DD_ANYBUF *inBuf, unsigned char **Data, int *pDataLen);

    // pData编码为DD_ANYBUF(有malloc动作), 将新的DD_ANYBUF节点地址赋值给outBuf
    int EncodeChar(char *pData, int dataLen, DD_ANYBUF **outBuf);

    // 解析节点inBuf中的字符串数据, 通过第二个参数Data指针传出
    int DecodeChar(DD_ANYBUF *inBuf, char **Data, int *pDataLen);

private:
    DD_INT DER_ItAsn1_Low_GetTagInfo(
            DD_UINT8 **ppDerData,
            DD_UINT32 **ppTagValue,
            DD_UINT32 **ppTagSize);
    DD_UINT32 DER_ItAsn1_Low_Count_LengthOfSize(DD_UINT32 iLength);
    DD_INT DER_ItAsn1_GetLengthInfo(
            DD_ANYBUF *pDerData,
            int *pLengthValue,
            int *pLengthSize);
    DD_INT DER_ItAsn1_Low_GetLengthInfo(
            DD_UINT8 **ppDerData,
            DD_UINT32 **ppLengthValue,
            DD_UINT32 **ppLengthSize);
    DD_INT DER_ItAsn1_Low_IntToChar(
            DD_UINT32 integer,
            DD_UINT8 **ppData,
            DD_UINT32 **ppLength);
    DD_INT DER_ItAsn1_Low_CharToInt(
            DD_UINT8 *aData,
            DD_UINT32 lLength,
            DD_UINT32 **ppInteger);
    DD_INT DER_ItAsn1_Low_WriteTagAndLength(
            DD_ANYBUF *pAnyIn,
            DD_UINT8 cTag,
            DD_ANYBUF **ppAnyOut,
            DD_UINT8 **ppUint8Value);
    DD_INT DER_ItAsn1_Low_ReadTagAndLength(
            DD_ANYBUF *pAnyIn,
            DD_UINT8 **ppUint8Data,
            DD_ANYBUF **ppAnyOut,
            DD_UINT8 **ppUint8Value);
    DD_INT DER_ItAsn1_WriteCharString(
            DD_ANYBUF *pCharString,
            DD_ANYBUF **ppDerCharString);
    DD_INT DER_ItAsn1_ReadCharString(
            DD_ANYBUF *pDerCharString,
            DD_ANYBUF **ppCharString);
    DD_INT DER_ItAsn1_WriteBmpString(
            ITASN1_BMPSTRING *pBmpString,
            ITASN1_BMPSTRING **ppDerBmpString);
    DD_INT DER_ItAsn1_ReadBmpString(
            ITASN1_BMPSTRING *pDerBmpString,
            ITASN1_BMPSTRING **ppBmpString);
    void DER_DI_FreeAnybuf(DD_ANYBUF  * pAnyBuf);
    // 给一个DD_ANYBUF类型空节点指针分配存储空间
    int DER_CREATE_LOW_DD_ANYBUF(DD_ANYBUF *&point);
    // 内联函数
    inline void DER_DD_Free(void *memblock)
    {
        if(memblock)
        {
            free(memblock);
            memblock = NULL;
        }
    }
    // 计算数据类型对应的tag
    inline DD_UINT32 DER_ITASN1_LOW_IDENTIFIER(DD_UINT8 &cTag)
    {
        return cTag & DD_DER_SHORT_ID_MASK;
    }
    // 在堆上创建一个指定大小的数组, 使用指针的引用 == 使用指针的指针
    inline int DER_ITASN1_LOW_CREATEUINT8(DD_UINT8* &point, DD_UINT32 size)
    {
        point = (DD_UINT8*)malloc(size);
        if (point==NULL)
        {
            return 7002;
        }
        //memset(point,0,size)
        return 0;
    }
    // 在堆上创建一个 DD_UINT32 大小的内存
    // 原来的宏参数是指针, 所以此次应该使用指针的引用
    inline int DER_ITASN1_LOW_CREATEUINT32(DD_UINT32* &point)
    {
        point = (DD_UINT32*)malloc(sizeof(DD_UINT32));
        if (point==NULL)
            return 7002;
        memset(point, 0, sizeof(DD_UINT32));
        return 0;
    }
    // 通过检测iResult的值, 返回错误类型
    inline DerErrType DER_ITASN1_LOW_CHECKERR(DD_INT iResult, DerErrType iErrNumber)
    {
        if (iResult == iErrNumber)
            return iErrNumber;
        return NoErr;
    }
    // 释放节点内存
    inline void DER_DACERT_LOW_FREE_ANYBUF(DD_ANYBUF *point)
    {
        DER_DD_Free(point->pData);
        DER_DD_Free(point);
    }

private:
    DDLog m_log;       // log类

	const DD_UINT8 TRUE = 1;
	const DD_UINT8 FALSE = 0;
	/* 标记当前数据是原始数据还是编码数据 */
	const DD_UINT32 DD_DER_CONSTRUCTED = 0x20;
	const DD_UINT32 DD_DER_PRIMITIVE = 0x00;

	/* The encodings for the universal types */
	const DD_UINT32 DD_DER_UNIVERSAL = 0x00;
	const DD_UINT32 DD_DER_APPLICATION = 0x40;
	const DD_UINT32 DD_DER_CONTEXT_SPECIFIC = 0x80;
	const DD_UINT32 DD_DER_PRIVATE = 0xC0;

	const DD_UINT32 DD_DER_RESERVED = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_RESERVED);
	const DD_UINT32 DD_DER_BOOLEAN = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_BOOLEAN);
	const DD_UINT32 DD_DER_INTEGER = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_INTEGER);
	const DD_UINT32 DD_DER_BITSTRING = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_BITSTRING);
	const DD_UINT32 DD_DER_OCTETSTRING = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_OCTETSTRING);
	const DD_UINT32 DD_DER_NULL = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_NULL);
	const DD_UINT32 DD_DER_OBJECT_IDENTIFIER = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_OBJECT_IDENTIFIER);
	const DD_UINT32 DD_DER_OBJECT_DESCRIPTOR = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_OBJECT_DESCRIPTOR);
	const DD_UINT32 DD_DER_EXTERNAL = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_EXTERNAL);
	const DD_UINT32 DD_DER_REAL = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_REAL);
	const DD_UINT32 DD_DER_ENUMERATED = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_ENUMERATED);
	const DD_UINT32 DD_DER_EMBEDDLogED_PDV = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_EMBEDDLogED_PDV);
	const DD_UINT32 DD_DER_STRING_UTF8 = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_UTF8);
	const DD_UINT32 DD_DER_13 = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_13);
	const DD_UINT32 DD_DER_14 = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_14);
	const DD_UINT32 DD_DER_15 = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_15);
	const DD_UINT32 DD_DER_SEQUENCE = (DD_DER_UNIVERSAL | DD_DER_CONSTRUCTED | DD_DER_ID_SEQUENCE);
	const DD_UINT32 DD_DER_SET = (DD_DER_UNIVERSAL | DD_DER_CONSTRUCTED | DD_DER_ID_SET);
	const DD_UINT32 DD_DER_STRING_NUMERIC = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_NUMERIC);
	const DD_UINT32 DD_DER_STRING_PRINTABLE = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_PRINTABLE);
	const DD_UINT32 DD_DER_STRING_T61 = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_T61);
	const DD_UINT32 DD_DER_STRING_VIDEOTEX = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_VIDEOTEX);
	const DD_UINT32 DD_DER_STRING_IA5 = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_IA5);
	const DD_UINT32 DD_DER_TIME_UTC = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_TIME_UTC);
	const DD_UINT32 DD_DER_TIME_GENERALIZED = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_TIME_GENERALIZED);
	const DD_UINT32 DD_DER_STRING_GRAPHIC = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_GRAPHIC);
	const DD_UINT32 DD_DER_STRING_ISO646 = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_ISO646);
	const DD_UINT32 DD_DER_STRING_GENERAL = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_GENERAL);
	const DD_UINT32 DD_DER_STRING_UNIVERSAL = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_UNIVERSAL);
	const DD_UINT32 DD_DER_29 = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_29);
	const DD_UINT32 DD_DER_STRING_BMP = (DD_DER_UNIVERSAL | DD_DER_PRIMITIVE | DD_DER_ID_STRING_BMP);

	/* Masks to extract information from a tag number */
	const DD_UINT32 DD_DER_CLASS_MASK = 0xC0;
	const DD_UINT32 DD_DER_CONSTRUCTED_MASK = 0x20;
	const DD_UINT32 DD_DER_SHORT_ID_MASK = 0x1F;
	const DD_UINT32 DD_DER_FIRST_NOT_ID_MASK = 0x7F;     //xia
	const DD_UINT32 DD_DER_FIRST_YES_ID_MASK = 0x80;     //xia
	const DD_UINT32 DD_DER_ALL_YES_ID_MASK = 0xFF;     //xia
															   /* The maximum size for the short tag number encoding, and the magic value
															   which indicates that a long encoding of the number is being used */
	const DD_UINT32 ITASN1_MAX_SHORT_BER_ID = 30;
	const DD_UINT32 ITASN1_LONG_BER_ID = 0x1F;

};

#endif // BASEASN1_H
