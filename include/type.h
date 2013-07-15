
#ifndef _TYPE_H_
#define _TYPE_H_

#define AssertSizeof(type,bytes) typedef char test##type[sizeof(type)==bytes?1:-1]

// 编译期断言 
typedef char test_ptr[sizeof(void*)==4?1:-1]; 

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef int				s32;
typedef unsigned int	uint;
typedef unsigned char	byte;
AssertSizeof(u8,1);
AssertSizeof(u16,2);
AssertSizeof(u32,4);

typedef uint	size_t;
typedef u16		port_t;
typedef int		bool_t;

typedef int handle_t;
#define INVALID_HANDLE -1

#define FALSE 0
#define TRUE  1

#define NULL 0

#endif
