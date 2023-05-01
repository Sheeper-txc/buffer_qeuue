#include "BufferManager.h"

BufferManager *BufferManager::instance = nullptr;

BufferManager *BufferManager::getInstance()
{
    if (instance == nullptr) {
        instance = new BufferManager();
    }
    return instance;
}

BufferQueue *BufferManager::createBufferQueue()
{
    //lock
    std::lock_guard<std::mutex> lock(_mutex);
    BufferQueue *que = new BufferQueue(create_random_num());
    if (que == nullptr) {
        LOG("create buffer queue failed\n");
        return nullptr;
    }
    _bufferQueue.push(que);
    return que;
}

BufferQueue *BufferManager::findBufferQueue(uint32_t id)
{
    BufferQueue *que = nullptr;
    if (id <= 0 || _bufferQueue.size() == 0) {
        LOG("wrong params\n");
        return nullptr;
    }
    //lock
    std::lock_guard<std::mutex> lock(_mutex);
    for (;;)
    {
        que = _bufferQueue.front();
        _bufferQueue.pop();
        _bufferQueue.push(que);
        if (id == que->getUniqueID())
            break;
    }
    return que;
}

errorType BufferManager::deleteBufferQueue(uint32_t id)
{
    if (id <= 0 || _bufferQueue.size() == 0) {
        LOG("wrong params\n");
        return errorType::ERROR;
    }
    //lock
    std::lock_guard<std::mutex> lock(_mutex);
    BufferQueue *que = nullptr;
    bool find = false;
    for (;;)
    {
        que = _bufferQueue.front();
        _bufferQueue.pop();

        if (id == que->getUniqueID()) {
            find = true;
            break;
        }
        _bufferQueue.push(que);
    }
    if (!find) {
        LOG("can not find buffer queue id=%d\n", id);
    }
    return errorType::SUCCESS;
}

errorType BufferManager::removeAll()
{
    if (_bufferQueue.size() == 0) {
        return errorType::SUCCESS;
    }
    BufferQueue *que = nullptr;
    for(;;)
    {
        que = _bufferQueue.front();
        _bufferQueue.pop();
        delete que;
        que = nullptr;
    }
    return errorType::SUCCESS;
}