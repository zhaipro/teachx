
#ifndef _TYPE_H_
#define _TYPE_H_

#define AssertSizeof(type,bytes) typedef char test##type[sizeof(type)==bytes?1:-1]

// 编译期断言 编译环境是否为32位 
typedef char test_ptr[sizeof(void*)==4?1:-1]; 

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef int				s32;
AssertSizeof(u8,1);
AssertSizeof(u16,2);
AssertSizeof(u32,4);
AssertSizeof(s32,4);
typedef unsigned int	uint;
typedef u8				byte;

typedef uint	size_t;
typedef u16		port_t;
typedef int		bool_t;
#define FALSE 0
#define TRUE  1

typedef int handle_t;
#define INVALID_HANDLE -1

#define NULL 0

#endif
