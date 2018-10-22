
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

struct descriptor_t{
    uint32_t d0;
    uint32_t d1;
};

#define set_gate(gate_addr, attr, func, selector)  \
do{     \
    (gate_addr)->d0 = ((uint32_t)(func) & 0xffff) | ((selector) << 16); \
    (gate_addr)->d1 = ((uint32_t)(func) & 0xffff0000) | (attr);         \
}while(0)

// 系统段描述符类型值说明
#define DA_386IGate 0x8e00  // 386 中断门类型值

// 中断向量
#define INT_VECTOR_PAGE_FAULT 0xe

#endif
