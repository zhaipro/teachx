
#define IsDigital(c) ((c)>='0' && (c)<='9')
typedef int BOOL;
#define TRUE	1
#define FALSE	0

double atof( const char *str );

int atoi( const char *str )
{
	int result = 0;
	int sign = 1;	//默认为正号 
//跳过空白符 
	while(*(str)==' ' || *(str)=='\t' || *(str)=='\n' || *(str)=='\r')
		++str;
//检测是否有正负号
	if(*str == '-'){
		++str;
		sign = -1;
	}else if(*str == '+'){
		++str;
	}
	
	while(IsDigital(*str))
	{
		result *= 10;
		result += (*str - '0');
		++str;
	}
	
	return sign*result;
}

long atol( const char *str )
{
	typedef char test_long[sizeof(long)==sizeof(int)? 1:-1];
	return atoi(str);
}

double strtod( const char *start, char **end );

long strtol( const char *start, char **end, int base );

unsigned long strtoul( const char *start, char **end, int base );

