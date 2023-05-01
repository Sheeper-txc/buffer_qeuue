#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <random>

#define LOG(fmt, ...) \
    do {    \
        printf("[%s|%d]: ", __func__, __LINE__); \
        printf(fmt, ##__VA_ARGS__); \
    } while(0);

//生成随机数
inline uint32_t create_random_num()
{
    std::seed_seq seq;
    std::vector<std::uint32_t> v(1);
    seq.generate(v.begin(), v.end());
    return v.at(0);
}

typedef enum _ERROR_TYPE
{
    ERROR = -1, //操作失败
    SUCCESS = 0, //操作成功
    ID_ERROR = 1, //id不正确
}errorType;