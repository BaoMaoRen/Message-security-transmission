#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>

#include "ShareMemory.h"

using namespace std;

//测试读
int main()
{
	void *addr = NULL;
	ShareMemory *s = NULL;

	char buf[128];

	//1. 打开共享内存
	s = new ShareMemory(0x55);

	//2. 关联共享内存
	addr = s->mapShm();
	if (NULL == addr)
	{
		cout << "关联共享内存失败..." << endl;
		return 1;
	}

	//3. 读共享内存
	memset(buf, 0, 128);
	memcpy(buf, addr, 128);
	cout << "buf: " << buf << endl;

	//4. 取消关联共享内存
	s->unMapShm();

	//5. 释放对象
	delete s;
	s = NULL;

	return 0;
}
