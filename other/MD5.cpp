#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/md5.h>


int main(void)
{
	int ret = 0;

	MD5_CTX c;

	unsigned char *data = "hello DD";
	unsigned char md[MD5_DIGEST_LENGTH + 1];
	unsigned char buf[MD5_DIGEST_LENGTH * 2 + 1];

	//MD5第一种方式
	//初始化
	ret =  MD5_Init(&c);
	if (0 == ret)
	{
		printf("MD5_Init failed... \n");
		return 1;
	}

	//添加源数据
	ret = MD5_Update(&c, data, strlen(data));
	if (!ret)
	{
		printf("MD5_Update failed....\n");
		return 1;
	}

	//计算哈希值  释放MD5_CTX
	ret = MD5_Final(md, &c);
	if (0 == ret)
	{
		printf("MD5_Final failed.....\n");
		return 1;
	}

	//printf("md5: %s\n", md);

	//将字节转换为对应的字符串
	memset(buf, 0, MD5_DIGEST_LENGTH * 2 + 1);
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
	{
		sprintf(&buf[2 * i], "%02X", (unsigned char)md[i]);
		//printf("%02X ", (unsigned char)md[i]);
	}

	printf("md5: %s\n", buf);


	printf("=============================\n");
	//MD5 第二种方式
	memset(md, 0, MD5_DIGEST_LENGTH + 1);
	MD5(data, strlen(data), md);
	//将字节转换为对应的字符串
	memset(buf, 0, MD5_DIGEST_LENGTH * 2 + 1);
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
	{
		sprintf(&buf[2 * i], "%02X", (unsigned char)md[i]);
		//printf("%02X ", (unsigned char)md[i]);
	}

	printf("md5: %s\n", buf);


	printf("hello world\n");

	return 0;
}
