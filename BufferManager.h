#ifndef _BUFFER_MANAGER_H
#define _BUFFER_MANAGER_H

#include <queue>

#include "BufferQueue.h"

class BufferManager
{
public:
    static BufferManager *instance;
    static BufferManager *getInstance();
public:
    BufferManager() = default;

    //禁用拷贝构造和复制函数
    BufferManager(const BufferManager&) = delete;
    BufferManager &operator=(const BufferManager &) = delete;

    //创建一个缓冲区对象
    BufferQueue *createBufferQueue();
    //查找指定id的缓冲区对象
    BufferQueue *findBufferQueue(uint32_t id);
    //删掉指定id的缓冲区
    errorType deleteBufferQueue(uint32_t id);
    //删掉所有缓冲区对象
    errorType removeAll();
private:
    std::queue<BufferQueue*> _bufferQueue;
    std::mutex _mutex;
};

#endif