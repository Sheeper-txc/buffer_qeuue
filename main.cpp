#include <iostream>
#include <sstream>
#include <string>
#include <deque>
#include <thread>
#include <assert.h>
#include <unistd.h>

#include "BufferPublicDef.h"

#include "BufferManager.h"

using std::cout;
using std::cin;
using std::endl;

#define SINGLE_TEST 0

unsigned char rand_byte()
{
//  return rand() % 26 + 0x41;
    return rand() % 0xff;
}

int rand_bytes(unsigned char **p_in_buf, int *p_rand_len)
{
    int index = 0;
    int rand_len = 0;
    unsigned char *p = NULL;

    rand_len = rand() % 1024 + 1;
    p = (unsigned char *)malloc(rand_len + 1);
    if (NULL == p)
    {
        return -1;
    }

    memset((void *)p, 0, rand_len + 1);

    for (index = 0; index < rand_len; index++) {
        p[index] = rand_byte();
    }

    *p_in_buf = p;
    *p_rand_len = rand_len;
    return 0;
}
#if SINGLE_TEST
void thread_func(void *arg)
{
    BufferQueue *que = (BufferQueue *)arg;
    assert(BufferManager::getInstance()->findBufferQueue(que->getUniqueID()));
    char buff[1024] = { 0 };
    int r_len = 0;
    while (true)
    {
        LOG("-----------------------read\n");
        r_len = que->bufferOut(que->getUniqueID(), buff, 1024);
        LOG("id=%lld: read %d bytes buffer\n", que->getUniqueID(), r_len);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
}
#else
void thread_write(void *arg)
{
    BufferQueue *que = (BufferQueue *)arg;
    assert(BufferManager::getInstance()->findBufferQueue(que->getUniqueID()));

    char *buff = nullptr;
    uint32_t buf_len = 0;
    while (true)
    {
        // buf_len = genRandBytes(&buff);
        rand_bytes((unsigned char**)&buff, (int *)&buf_len);
        int w_len = que->bufferIn(que->getUniqueID(), buff, buf_len);
        std::stringstream out;
        out << std::this_thread::get_id();
        LOG("id=%lld, tid=%s: write %d bytes buffer\n", que->getUniqueID(), out.str().c_str(), buf_len);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void thread_read(void *arg)
{
    BufferQueue *que = (BufferQueue *)arg;
    assert(BufferManager::getInstance()->findBufferQueue(que->getUniqueID()));

    char buff[1024] = { 0 };
    int r_len = 0;
    while (true)
    {
        // LOG("-----------------------read\n");
        r_len = que->bufferOut(que->getUniqueID(), buff, 1024);
        std::stringstream out;
        out << std::this_thread::get_id();
        LOG("id=%lld, tid=%s: read %lld bytes buffer\n", que->getUniqueID(), out.str().c_str(), r_len);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
#endif

int main(int argc, char *argv[])
{
    BufferManager *manager = BufferManager::getInstance();
    assert(manager != nullptr);
#if SINGLE_TEST
    //单线程测试
    BufferQueue *que = manager->createBufferQueue();
    std::thread thread(thread_func, que);
    
    char *buff = nullptr;
    uint32_t buf_len = 0;
    while (true)
    {
        // buf_len = genRandBytes(&buff);
        rand_bytes((unsigned char**)&buff, (int *)&buf_len);
        int w_len = que->bufferIn(que->getUniqueID(), buff, buf_len);
        LOG("id=%lld: write %d bytes buffer\n", que->getUniqueID(), buf_len);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    thread.join();
#else
    //多线程测试
    BufferQueue *que = manager->createBufferQueue();
    assert(que);
    std::vector<std::thread> wThreadList(2); //写线程
    std::vector<std::thread> rThreadList(2); //读线程
    for (int i = 0; i < wThreadList.size(); i++) {
        wThreadList[i] = std::thread(thread_write, que);
    }
    for (int i = 0; i < rThreadList.size(); i++) {
       rThreadList[i] = std::thread(thread_read, que);
    }
    for (int i = 0; i < wThreadList.size(); i++) {
        wThreadList[i].join();
    }
    for (int i = 0; i < rThreadList.size(); i++) {
        rThreadList[i].join();
    }
#endif


    manager->removeAll();
    delete manager;
    manager = nullptr;
    return 0;
}