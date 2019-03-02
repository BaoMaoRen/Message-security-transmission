#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>

#include "ShareMemory.h"

using namespace std;

//测试写
int main()
{
	ShareMemory *s = NULL;

	void *addr = NULL;

	const char *data = "这是一个寂寞的天, 下着有些伤心的雨";

	//1. 构建共享内存对象(创建共享内存)
	s = new ShareMemory(0x55, 1024);

	//2. 关联共享内存
	addr = s->mapShm();
	if (NULL == addr)
	{
		cout << "关联共享内存失败..." << endl;
		return 1;
	}
	
	cout << "关联共享内存成功..." << endl;

	//3. 写共享内存
	memcpy(addr, data, strlen(data));

	//4. 取消关联共享内存
	s->unMapShm();

	cout << "取消关联共享内存成功" << endl;

    //5. 释放对象
    delete s; 
    s = NULL;

	return 0;
}
