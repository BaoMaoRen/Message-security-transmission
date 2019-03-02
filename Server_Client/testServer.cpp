#include <iostream>

#include <unistd.h>
#include <signal.h>

#include "ServerOperation.h"

using namespace std;

//信号处理函数
void fun(int signo, siginfo_t *pInfo, void *contex)
{
    cout << "捕捉到信号: " << signo << endl;
    cout << "密钥协商服务端准备退出..." << endl;
    ServerOperation::flag = 1;
}

//创建一个守护进程
int createDaemon(void)
{
    pid_t pid = -1;

    //1. 创建子进程 父进程退出
    pid = fork();
    if (-1 == pid)
    {
        perror("fork"); 
        return 1;
    }
    else if (pid > 0)
    {
        //父进程退出
        exit(0); 
    }

    //2. 创建一个会话
    if (-1 == setsid())
    {
        perror("setsid"); 
        return 1;
    }

    //3. 改变当前进程工作目录

    //4. 修改掩码

    //5. 关闭文件描述符

    //6. 执行核心的动作
}

//密钥协商服务端
int main()
{
	ServerInfo info;

    struct sigaction act;
    memset(&act, 0, sizeof act);
    act.sa_sigaction = fun;
    act.sa_flags = SA_SIGINFO;

    createDaemon();

    //注册信号
    sigaction(SIGUSR1, &act, NULL);

	//1. 从配置文件中读取服务端配置信息
	memset(&info, 0, sizeof info);
	strcpy(info.serverID, "1111");
	strcpy(info.dbUser, "SECMNG");
	strcpy(info.dbPasswd, "SECMNG");

	//数据库实例
	strcpy(info.dbSID, "orcl");
	//服务端端口
	info.sPort = 10086;
	//最大网点数
	info.maxNode = 20;
	//共享内存key
	info.shmkey = 0x11;

	//2. 创建密钥协商服务端类对象
	ServerOperation *server = new ServerOperation(&info);

	//3. 启动密钥协商服务端
	server->startWork();

	return 0;
}
