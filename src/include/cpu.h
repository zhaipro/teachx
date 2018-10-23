
#ifndef _CPU_H_
#define _CPU_H_

#include <assert.h>
#include <stdint.h>

#define PAGE_PRESENT    0X01    // 存在标志
#define PAGE_WRITE      0X03    // 可写标志

#pragma pack (2)
struct idtptr_t{
    uint16_t limit; // 边界
    void *addr;     // 基址
};
#pragma pack ()
#define assert_sizeof(type, bytes) \
    static_assert(sizeof(type) == bytes, "sizeof error")
assert_sizeof(struct idtptr_t, 6);

#endif
