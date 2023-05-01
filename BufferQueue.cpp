#include "BufferQueue.h"


BufferQueue::BufferQueue(uint32_t uid) 
    : _unique_id(uid)
    , _available(false)
    , _nodeHead(nullptr)
{
    _nodeHead = new Node_head_t();
    if (_nodeHead == nullptr) {
        LOG("buffer queue init failed\n");
        return ;
    }
    memset(_nodeHead, 0, sizeof(Node_head_t));
    LOG("id = %lld\n", _unique_id);
    _available = true;
}

BufferQueue::~BufferQueue()
{
    Node_head_t *h = _nodeHead;
    if (nullptr == h || h->next == nullptr) {
        LOG("empty list\n");
        return ;
    }
    while(deleteFirstNode(h) == errorType::SUCCESS);

    delete h;
    h = nullptr;
    _nodeHead = nullptr;
}

bool BufferQueue::bufferAvailable(uint32_t id) const
{
    if (_unique_id != id) {
        LOG("id is not equal to %d.\n", _unique_id);
        return false;
    }
    return _available;
}

int BufferQueue::bufferIn(uint32_t id, const char *b, size_t size)
{
    if (_unique_id != id || !_available) {
        LOG("id is not equal to %d.\n", _unique_id);
        return errorType::ERROR;
    }
    Node_data_t node_data;
    Node_head_t *head = _nodeHead;

    if (b == nullptr || size <= 0) {
        LOG("error params\n");
        return errorType::ERROR;
    }

    memset(&node_data, 0, sizeof(Node_data_t));
    node_data.len_used = 0;
    node_data.len_data = size;
    node_data.data = (uint8_t*)b;

    int ret = appendTail(_nodeHead, &node_data);
    memset(&node_data, 0, sizeof(Node_data_t));

    return ret;
}

int BufferQueue::bufferOut(uint32_t id, char *b, size_t size)
{
    if (_unique_id != id || !_available) {
        LOG("id is not equal to %d.\n", _unique_id);
        return errorType::ERROR;
    }
    Node_t *i = nullptr;
    uint32_t curr_copy_len = 0;
    uint32_t curr_node_bytes = 0;
    uint32_t already_copy_len = 0;
    uint32_t left_copy_len = size;
    
    Node_head_t *h = _nodeHead;

    if (h == nullptr || b == nullptr) {
        LOG("wrong params\n");
        return errorType::ERROR;
    }
    if (h->next == nullptr || size == 0) {
        LOG("empty data\n");
        return errorType::SUCCESS;
    }
    /* 遍历链表 */
    for (i = h->next; i != nullptr;) {
        if (i->data.len_data <= i->data.len_used) {
            i = i->next;
            //删除已经拿完数据的节点
            deleteFirstNode(h);
            continue;
        }

        left_copy_len = size - already_copy_len;
        curr_node_bytes = i->data.len_data - i->data.len_used;
        curr_copy_len = (curr_node_bytes > left_copy_len) ? left_copy_len : curr_node_bytes;

        memcpy((void *)(b + already_copy_len), i->data.data + i->data.len_used, curr_copy_len);

        i->data.len_used += curr_copy_len;
        if (i->data.len_data <= i->data.len_used) {
            i = i->next;
            deleteFirstNode(h);
        }
        already_copy_len += curr_copy_len;
        if (curr_copy_len >= size) {
            break;
        }
    }

    return already_copy_len;
}

errorType BufferQueue::deleteFirstNode(Node_head_t *h)
{
    Node_t *first = nullptr;
    //lock
    _mutex.lock();
    //
    if (h == nullptr || nullptr == h->next) {
        LOG("empty list\n");
        _mutex.unlock();
        return  errorType::ERROR;
    }
    first = h->next;
    h->next = first->next; // 通过摘掉节点

    first->next = nullptr;
    first->data.len_data = 0;
    first->data.len_used = 0;
    if (nullptr != first->data.data) {
        delete first->data.data;
        first->data.data = nullptr;
    }
    delete first;
    first = nullptr;

    //unlock
    _mutex.unlock();
    return errorType::SUCCESS;
}

int BufferQueue::appendTail(Node_head_t *h, Node_data_t *node)
{
    Node_t *newNode = NULL;
    Node_t *i = (Node_t *)h;

    // std::lock_guard<std::mutex> lock(_mutex);
    _mutex.lock();
    if (h == nullptr || node == nullptr) {
        LOG("wrong params\n");
        _mutex.unlock();
        return errorType::ERROR;
    }
    /* 第一步：分配新节点空间 */
    newNode = createNode();
    if (nullptr == newNode) {
        LOG("create new node failed\n");
        _mutex.unlock();
        return errorType::ERROR;
    }
    /* 第二步：赋值给新节点 */
    memcpy(&newNode->data, (void *)node, sizeof(Node_data_t));

    /* 第三步：找到链表尾 */
    while (i->next != nullptr) {
        i = i->next;
    }
    /* 第四步：把新节点插入到链表尾 */
    i->next = newNode;          // 尾节点指向新节点
    newNode->next = nullptr;    // 新节点指向NULL，作为链表尾

    _mutex.unlock();
    return newNode->data.len_data;
}

Node_t *BufferQueue::createNode(void)
{
    Node_t *newNode = nullptr;

    /* 第一步：分配新节点空间 */
    newNode = new Node_t();
    if (nullptr == newNode) {
        LOG("create memory for new node failed\n");
        return nullptr;
    }
    memset((void *)newNode, 0, sizeof(Node_t));
    return newNode;
}

