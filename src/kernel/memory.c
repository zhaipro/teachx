
#include <string.h>

#include "asm.h"
#include "bios.h"
#include "cpu.h"
#include "kernel.h"
#include "settings.h"
#include "stdio.h"

static uint32_t s_total;

void* kmalloc()
{
    static uint32_t s_start = KERNEL_SEGMENT / _4K;
    return (void*)((s_start++) * _4K);
}

#define get_pte(vaddr) (((uint32_t*)PDT) + ((uint32_t)(vaddr) >> 12))

// 如果页表不存在就会产生异常。
#define get_paddr(vaddr) ((*get_pte(vaddr)) & 0xfffff000)

uint32_t create_pd()
{
    uint32_t *new_pd = kmalloc();
    memset(new_pd, 0, _4K);
    new_pd[0] = ((uint32_t*)PDT)[PDT / _4K];
    new_pd[PDT / _4M] = get_paddr(new_pd) | PAGE_PRESENT;
    return get_paddr(new_pd);
}

// 获取内存总量，单位：K
static uint32_t get_total_memory()
{
    uint32_t total = cmos_read(0x18);
    total <<= 8;
    total |= cmos_read(0x17);
    return total;
}

static uint32_t get_free_page()
{
    // 内存管理员只负责管理 1M 以后的内存，之前的交给内核
    static uint32_t s_start = _1M / _4K;
    return (s_start++) * _4K;
}

// https://gcc.gnu.org/onlinedocs/gcc/x86-Function-Attributes.html
__attribute__((interrupt)) static void page_fault(void *_, uint32_t error_code)
{
    uint32_t cr2 = scr2();
    printk("page fault, error: %d, addr: %d K\n", error_code, cr2 / _1K);
    if(error_code) {
        ((uint32_t*)PDT)[cr2 / _4K] = get_free_page() | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
    } else {
         ((uint32_t*)PDT)[cr2 / _4K] = get_free_page() | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
    }
}

uint32_t do_memory_fork()
{
    uint32_t *new_pd = kmalloc();
    uint32_t *new_pt = NULL;
    void *new_page = NULL;
    uint32_t *pd = &((uint32_t*)PDT)[PDT / _4K];
    uint32_t *pt = NULL;
    void *page = NULL;
    int i, j;
    memcpy(new_pd, pd, _4K);
    // 页目录也是页表
    new_pd[PDT / _4M] = get_paddr(new_pd) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
    for(i=_4M/_4M; i<PDT/_4M; i++)
    {
        if(pd[i] & PAGE_PRESENT) {
            printk("pd: %dM\n", i * 4);
            new_pt = kmalloc();
            pt = &((uint32_t*)PDT)[i * 1024];
            memcpy(new_pt, pt, _4K);
            for(j=0; j<1024; j++)
            {
                if(pt[j] & PAGE_PRESENT) {
                    printk("pt: %dK\n", i * 4 * 1024 + j * 4);
                    new_page = kmalloc();
                    page = (void*)(_4M * i + _4K * j);
                    memcpy(new_page, page, _4K);
                    new_pt[j] = get_paddr(new_page) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
                } else {
                    new_pt[j] = 0;
                }
            }
            new_pd[i] = get_paddr(new_pt) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
        }
    }
    return get_paddr(new_pd);
}

static void init_pdt()
{
    // 页目录(Page Directory)
    uint32_t pd = get_free_page();
    // 页表(page table)
    uint32_t pt0 = get_free_page();
    printk("pd: %dk, pt0: %dk\n", pd / _1K, pt0 / _1K);
    // 先清空
    memset((void*)pd, 0, 2 * sizeof(uint32_t) * 1024);
    // 前 4M 的映射
    ((uint32_t*)pd)[0] = pt0 | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
    // [0, 4K) 不可用，用于声明空指针
    // [4K, 1M) 指向真实的地址，用于书写 vga
    // [1M, 4M) 不用
    for (int i=1; i<0xb8000/_4K; i++) {
        ((uint32_t*)pt0)[i] = (i * _4K) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
    }
    for (int i=0xb8000/_4K; i<_1M/_4K; i++) {
        ((uint32_t*)pt0)[i] = (i * _4K) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
    }
    // 页目录也是页表
    ((uint32_t*)pd)[PDT / _4M] = pd | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
    // 设置页目录的物理地址
    lcr3(pd);
}

void init_memory()
{
    s_total = get_total_memory();
    printk("Total memory: %d K\n", s_total);
    init_pdt();
    // 加载页故障修理工
    set_trap_gate(INT_VECTOR_PAGE_FAULT, page_fault);
    printk("Init memory ok!\n");
}
