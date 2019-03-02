#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/sha.h>



int main(void)
{
	int ret = 0;
	char *data = "hello DD";

	unsigned char md[SHA_DIGEST_LENGTH];
	unsigned char buf[SHA_DIGEST_LENGTH * 2 + 1] = { 0 };

	SHA_CTX c;

	//1. 初始化
	ret = SHA1_Init(&c);
	if (0 == ret)
	{
		printf("SHA1_Init failed....\n");
		return 1;
	}

	//2. 添加数据
	ret = SHA1_Update(&c, data, strlen(data));
	if (0 == ret)
	{
		printf("SHA1_Update failed....\n");
		return 1;
	}

	//3. 获取结果
	ret = SHA1_Final(md, &c);
	if (0 == ret)
	{
		printf("SHA1_Final failed...\n");
		return 1;
	}

	printf("md: %s\n", md);

	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&buf[i * 2], "%02X", md[i]);
	}
	printf("buf: %s\n", buf);

	printf("===================================\n");
	memset(md, 0, SHA_DIGEST_LENGTH);
	SHA1(data, strlen(data), md);

	memset(buf, 0, SHA_DIGEST_LENGTH * 2 + 1);
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&buf[i * 2], "%02X", md[i]);
	}
	printf("buf: %s\n", buf);

	printf("hello world\n");
	return 0;
}