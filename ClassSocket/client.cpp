#include <iostream>
#include <cstring>
#include <unistd.h>

#include "TcpSocket.h"

using namespace std;


int main(void)
{
    int ret = -1;

    TcpSocket *tcpSocket = NULL;

    const char *data = "hello DD";
    char *recvData = NULL;
    int recvDataLen = 0;

    //1. 创建一个对象
    tcpSocket = new TcpSocket;

    //2. 连接到服务端
    ret = tcpSocket->connectToHost("192.168.16.55", 10086);
    if (0 != ret)
    {
        cout << "connectToHost failed..." << endl; 
        return 1;
    }

    while(1)
    {
        //3. 发送数据
        ret = tcpSocket->sendMsg(data, strlen(data));
        if (0 != ret)
        {
            cout << "sendMsg failed..." << endl;
            break;
        }

        //4. 接收数据
        ret = tcpSocket->recvMsg(&recvData, recvDataLen);    
        if (0 != ret)
        {
            cout << "recvMsg failed.." << endl; 
            break;
        }

        cout << "recvData: " << recvData << " recvDataLen: " << recvDataLen << endl;

        //6. 释放内存
        tcpSocket->freeMemory((char**)&recvData);

        sleep(1);
    }


    //5. 断开连接
    tcpSocket->disConnect();

    cout << "hello DD" << endl;
    return 0;
}

