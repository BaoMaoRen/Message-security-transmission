#include "SecKeyShm.h"

#include <iostream>
#include <cstring>

using namespace std;

//打开共享内存
SecKeyShm::SecKeyShm(int key):ShareMemory(key)
{
    mMaxNode = 10;
}

//创建共享内存
SecKeyShm::SecKeyShm(int key, int maxNode):ShareMemory(key, maxNode * sizeof(NodeShmInfo)),mMaxNode(maxNode)
{

}

//打开共享内存
SecKeyShm::SecKeyShm(char* pathName):ShareMemory(pathName)
{

    mMaxNode = 10;
}

//创建共享内存
SecKeyShm::SecKeyShm(char* pathName, int maxNode):ShareMemory(pathName, maxNode * sizeof(NodeShmInfo)), mMaxNode(maxNode)
{

}

//析构函数
SecKeyShm::~SecKeyShm()
{

}


//将网点密钥信息写入共享内存
int SecKeyShm::shmWrite(NodeShmInfo* pNodeInfo)
{
    int i = 0;

    void *addr = NULL;

    NodeShmInfo *tmp = NULL;

    NodeShmInfo info;
    memset(&info, 0, sizeof(NodeShmInfo));

    //1. 关联共享内存
    addr = mapShm();
    if (NULL == addr)
    {
        cout << "mapShm failed..." << endl;
        return -1;
    }

    //2. 判断共享内存中是否存在对应的网点, 如果存在就直接覆盖
    for (i = 0; i < mMaxNode; i++)
    {
        tmp = static_cast<NodeShmInfo*>(addr) + i; 
        if ((strcmp(tmp->clientID, pNodeInfo->clientID) == 0) &&
            (strcmp(tmp->serverID, pNodeInfo->serverID) == 0))
            {
                cout << "共享内存中存在对应的网点密钥信息, 现在覆盖" << endl;
                //直接覆盖 
                memcpy(tmp, pNodeInfo, sizeof(NodeShmInfo)); 
                break;
            }
    }

    //3. 共享内存中不存在对应的网点信息, 找一个空白的区域写网点密钥信息到共享内存
    if (i == mMaxNode)
    {
        for (i = 0; i < mMaxNode; i++)    
        {
            tmp = static_cast<NodeShmInfo*>(addr) + i;
            if (memcmp(&info, tmp, sizeof(NodeShmInfo)) == 0)        
            {
                cout << "找到空白区域, 现在将网点密钥信息写入共享内存" << endl; 
                memcpy(tmp, pNodeInfo, sizeof(NodeShmInfo));
                break;
            }
        }
    }

    if (i == mMaxNode)
    {
        cout << "共享内存已经被写满了, 请联系负责人" << endl; 
    }

    //4. 解除关联
    unMapShm();

    return 0;
}

//从共享内存中读取网点密钥信息
int SecKeyShm::shmRead(const char* clientID, const char* serverID, NodeShmInfo* pNodeInfo)
{
    int i = 0;

    void *addr = NULL;

    NodeShmInfo *tmp = NULL;

    //1. 关联共享内存
    addr = mapShm();    
    if (NULL == addr)
    {
        cout << "mapShm failed.." << endl; 
        return -1;
    }

    //2. 查找共享内存是否存在对应的网点密钥信息
    for (i = 0; i < mMaxNode; i++)
    {
        tmp = static_cast<NodeShmInfo*>(addr) + i; 
        if ((strcmp(clientID, tmp->clientID) == 0) &&
            (strcmp(serverID, tmp->serverID) == 0))
            {
                cout << "找到对应的网点的密钥信息" << endl; 
                memcpy(pNodeInfo, tmp, sizeof(NodeShmInfo));
                break;
            }
    }

    if (i == mMaxNode)
    {
        cout << "没有找到对应的网点信息" << endl; 
    }

    //3. 解除关联
    unMapShm();

    return 0;
}





