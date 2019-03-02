#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include "ClientOperation.h"

#include <iostream>

using namespace std;

//显示菜单
int showMenu(void)
{
	int choice = 0;

	cout << "================================" << endl;
	cout << "==========1. 密钥协商============" << endl;
	cout << "==========2. 密钥校验============" << endl;
	cout << "==========3. 密钥注销============" << endl;
	cout << "==========4. 密钥查看============" << endl;
	cout << "==========5. 退出系统============" << endl;
	cout << "================================" << endl;
	cout << "请选择: " << endl;
	cin >> choice;

	//清空输入缓冲区
	//cin.clear();
	while ('\n' != getchar())
		/*do nothing*/;

	return choice;
}

//主函数
int main()
{
	int ret = -1;

	ClientInfo info;

	//从配置文件中读取客户端信息
	memset(&info, 0, sizeof(info));
	strcpy(info.clientID, "1111");
	strcpy(info.serverID, "0001");
	strcpy(info.authCode, "1111");
	strcpy(info.serverIP, "192.168.16.49");

	info.serverPort = 10086;
	info.maxNode = 1;
	info.shmKey = 0x55;


	//1. 创建一个密钥协商客户端对象
	ClientOperation clientOperation(&info);

	//2. 循环 等待用户选择
	while (1)
	{
		//清屏
		system("clear");

		ret = showMenu();

		//退出系统
		if (5 == ret)
		{
			break;
		}

		switch (ret)
		{
			//密钥协商
		case 1:
			ret = clientOperation.secKeyAgree();
			cout << "密钥协商" << endl;
			break;

			//密钥校验
		case 2:
			ret = clientOperation.secKeyCheck();
			cout << "密钥校验" << endl;
			break;
			
			//密钥注销
		case 3:
			ret = clientOperation.secKeyRevoke();
			cout << "密钥注销" << endl;
			break;

			//密钥查看
		case 4:
			ret = clientOperation.secKeyView();
			cout << "密钥查看" << endl;
			break;
		default:
			ret = -1;
			cout << "无效的选择" << endl;
		}

		if (0 != ret)
		{
			cout << "做对应的业务失败..." << endl;
		}
		else
		{
			cout << "做对应的业务成功..." << endl;
		}

		cout << "按下任意键继续" << endl;
		getchar();
	}

	return 0;
}
