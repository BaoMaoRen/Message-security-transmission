#include <iostream>

#include "TcpServer.h"

using namespace std;

//服务端
int main(void)
{
    int ret = -1;
    TcpServer *ts = NULL; 

    TcpSocket *s = NULL;

    char *recvData = NULL;
    int recvDataLen = 0;

    //1. 创建一个对象
    ts = new TcpServer;

    //2. 设置监听
    ts->setListen(10086);

    //3. 接受客户端连接
    s = ts->acceptConn(10);
    if (NULL == s)
    {
        cout << "acceptConn failed.." << endl; 
        return 1;
    }

    while(1)
    {
        //4. 接收数据
        ret = s->recvMsg(&recvData, recvDataLen); 
        if (0 != ret)
        {
            cout << "recvMsg failed.." << endl;
            break;
        }
        cout << "recvData: " << recvData <<  " recvDataLen: " << recvDataLen << endl;
        //printf("recvData:%s recvDataLen: %d\n", recvData, recvDataLen);


        //5. 发送数据
        ret = s->sendMsg(recvData, recvDataLen);

        //6. 释放内存
        s->freeMemory((char**)&recvData);
    }

    s->disConnect();
    //7. 关闭文件描述符
    ts->closefd();

    cout << "hello DD" << endl;

    return 0;
}
