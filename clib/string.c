
#include "../inc/string.h"

#define NULL 0

// 功能：函数在buffer指向的数组的count个字符的字符串里查找ch 首次出现的位置。
// 返回一个指针，指向ch 在字符串中首次出现的位置, 
// 如果ch 没有在字符串中找到，返回NULL。
void *memchr( const void *buffer, int ch, size_t count )
{
	while(1)
	{
		if(count == 0){
			return NULL;
		}
		if(*(char*)buffer == ch){
			return (void*)buffer;
		}
		++buffer;
		--count;
	}
}

int memcmp( const void *buffer1, const void *buffer2, size_t count )
{
	if(count==0)
		return 0;
	--count;
	while(count && *(char*)buffer1 == *(char*)buffer2)
	{
		--count;
		++buffer1;
		++buffer2;
	}
	return *(char*)buffer1 - *(char*)buffer2;
}

void *memcpy( void *to, const void *from, size_t count )
{
	void *ret = to;
	while(count--)
	{
		*(char*)to = *(char*)from;
		++to;
		++from;
	}
	return ret;
}

void *memmove( void *to, const void *from, size_t count );

void *memset(void *buffer,int ch,size_t count)
{
	char *temp = buffer;
	while(count--)
	{
		*temp = (char)ch;
		++temp;
	}
	return buffer;
}

char *strcat( char *str1, const char *str2 )
{
	char *temp = str1;
	temp += strlen(str1);
	strcpy(temp,str2);
	return str1;
}

char *strchr( const char *str, int ch );

int strcmp( const char *str1, const char *str2 )
{
	while(*str1 == *str2 && *str1!='\0')
	{
		++str1;
		++str2; 
	}
	return *str1 - *str2;
}

int strcoll( const char *str1, const char *str2 );

char *strcpy( char *to, const char *from )
{
	char *ret = to;
	do{
		*to = *from;
		++to;
	}while(*(from++) != '\0');
	return ret;
}

size_t strcspn( const char *str1, const char *str2 );

char *strerror( int num );

size_t strlen(const char *str)
{
	const char *p = str;
	while(*p != '\0')
		++p;
	return p - str;
}

char *strncat( char *str1, const char *str2, size_t count );

int strncmp( const char *str1, const char *str2, size_t count )
{
	if(count == 0)
		return 0;
	while(count>0 && *str1 == *str2 && *str1!='\0')
	{
		++str1;
		++str2; 
		--count;
	}
	return *str1 - *str2;
}

char *strncpy( char *to, const char *from, size_t count )
{
	char *ret = to;
	
	while(count && *from != '\0')
	{
		*to = *from;
		++to;
		++from;
		--count;
	}
	
	return to;
}

char *strpbrk( const char *str1, const char *str2 );

char *strrchr( const char *str, int ch );

size_t strspn( const char *str1, const char *str2 );

char *strstr( const char *str1, const char *str2 );

double strtod( const char *start, char **end );

char *strtok( char *str1, const char *str2 );

long strtol( const char *start, char **end, int base );

unsigned long strtoul( const char *start, char **end, int base );

size_t strxfrm( char *str1, const char *str2, size_t num );
