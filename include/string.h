
#ifndef _STRING_H_
#define _STRING_H_
#include "type.h"

void *memchr( const void *buffer, int ch, size_t count );
int memcmp( const void *buffer1, const void *buffer2, size_t count );
void *memcpy( void *to, const void *from, size_t count );
void *memmove( void *to, const void *from, size_t count );
void *memset(void *buffer,int ch,size_t count);
char *strcat( char *str1, const char *str2 );
char *strchr( const char *str, int ch );
int strcmp( const char *str1, const char *str2 );
int strcoll( const char *str1, const char *str2 );
char *strcpy( char *to, const char *from );
size_t strcspn( const char *str1, const char *str2 );
char *strerror( int num );
size_t strlen( const char *str );
char *strncat( char *str1, const char *str2, size_t count );
int strncmp( const char *str1, const char *str2, size_t count );
char *strncpy( char *to, const char *from, size_t count );
char *strpbrk( const char *str1, const char *str2 );
char *strrchr( const char *str, int ch );
size_t strspn( const char *str1, const char *str2 );
char *strstr( const char *str1, const char *str2 );
double strtod( const char *start, char **end );
char *strtok( char *str1, const char *str2 );
long strtol( const char *start, char **end, int base );
unsigned long strtoul( const char *start, char **end, int base );
size_t strxfrm( char *str1, const char *str2, size_t num );

#endif
