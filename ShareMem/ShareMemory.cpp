#include "ShareMemory.h"

#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <iostream>

//打开共享内存
ShareMemory::ShareMemory(int key)
{
    mShmId = shmget(key, 0, 0);
    if (-1 == mShmId)
    {
        //写日志 
    }
}


//打开共享内存
ShareMemory::ShareMemory(char *pathName)
{
    key_t key = -1;

    //生成key
    key = ftok(pathName, 'X');
    if (-1 == key)
    {
        //写日志 
    }

    //打开已经存在共享内存
    mShmId = shmget(key, 0, 0);
    if (-1 == mShmId)
    {
        //写日志 
    }
}


//创建共享内存
ShareMemory::ShareMemory(int key, int shmSize)
{
    void *addr = NULL;

    //先打开共享内存, 如果打开失败就去创建并且清零
    mShmId = shmget(key, 0, 0);
    if (-1 == mShmId)
    {
        mShmId = shmget(key, shmSize, IPC_CREAT | 0644);
        if (-1 == mShmId)
        {
            //写日志 
            return;
        }

        //关联共享内存
        addr = mapShm();        
        if (NULL == addr)
        {
            //写日志 
            return;
        }

        //清零
        memset(addr, 0, shmSize);

        //解除关联共享内存
        unMapShm();
    }
}


//创建共享内存
ShareMemory::ShareMemory(char *pathName, int shmSize)
{
    key_t key = -1;

    void *addr = NULL;

    //生成key
    key = ftok(pathName, 'X');
    if (-1 == key)
    {
        //写日志 
    }

    mShmId = shmget(key, 0, 0);
    if (-1 == mShmId)
    {
        //创建共享内存 
        mShmId = shmget(key, shmSize, IPC_CREAT | 0644);
        if (-1 == mShmId)
        {
            //写日志 
            return;
        }

        //关联共享内存
        addr = mapShm();
        if (NULL == addr)
        {
            //写日志 
            return;
        }

        //清零
        memset(addr, 0, shmSize);

        //取消关联共享内存
        unMapShm();
    }
}


//析构函数
ShareMemory::~ShareMemory()
{

}


//关联共享内存
void *ShareMemory::mapShm()
{
    mShmAddr = shmat(mShmId, NULL, 0);
    if ((void *)-1 == mShmAddr)
    {
        //写日志 

        return NULL;
    }

    return mShmAddr;
}


//解除关联
int ShareMemory::unMapShm()
{
    int ret = -1;

    ret = shmdt(mShmAddr);
    if (-1 == ret)
    {
        //写日志 
        return -1;
    }

    return 0;
}


//删除共享内存
int ShareMemory::delShm()
{
    int ret = -1;
    
    ret = shmctl(mShmId, IPC_RMID, NULL);
    if (-1 == ret) 
    {
        //写日志 

        return -1;
    }

    return 0;
}


