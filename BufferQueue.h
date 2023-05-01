#pragma once

#include <mutex>

#include "BufferPublicDef.h"

typedef struct node_data
{
    unsigned int len_used;   // 节点数据已使用长度
    unsigned int len_data;   // 节点数据长度
    unsigned char *data; // 节点数据指定为整型
}Node_data_t;

/* 封装链表节点 */
typedef struct node
{
    Node_data_t data;  // 节点数据
    struct node *next; // 下一个节点
} Node_t;

/* 封装链表头节点 */
typedef struct _node_head
{
    Node_data_t data;  // 节点数据
    struct node *next; // 下一个节点
} Node_head_t;

//FIFO数据缓冲区队列
class BufferQueue
{
public:
    BufferQueue(uint32_t uid);
    ~BufferQueue();

    //禁用默认构造
    BufferQueue() = delete;
    //禁用拷贝构造和复制
    BufferQueue(const BufferQueue &) = delete;
    BufferQueue &operator=(const BufferQueue &) = delete;

    //写数据缓冲区: 返回实际写入的长度
    int bufferIn(uint32_t id, const char *b, size_t size);
    //读数据缓冲区: 返回实际读出来的长度
    int bufferOut(uint32_t id, char *b, size_t size);
    //
    bool bufferAvailable(uint32_t id) const;
    //
    uint32_t getUniqueID() { return _unique_id; }
private:
    int appendTail(Node_head_t *h, Node_data_t *node);
    Node_t *createNode(void);
    errorType deleteFirstNode(Node_head_t *h);
private:
    Node_head_t *_nodeHead;
    std::mutex _mutex;
    bool _available;
    uint32_t _unique_id; //该数据缓冲区唯一的ID,由BufferManager管理
};