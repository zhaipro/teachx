
#include "stdlib.h"

// for printf
char *itoa( int value, char *string, int radix )
{
	char *tstring = string;
	char *first;
	unsigned int tvalue;
	int temp;
// 
	if(radix==10 && value<0){	// 只考虑十进制数的正负 
		*tstring = '-';
		++tstring;
		tvalue = -value;
	}else{
		tvalue = (unsigned int)value;
	}
	first = tstring;
	
	do{
		temp = tvalue%radix;
		tvalue /= radix;
		if(temp<10){
			*tstring = temp + '0';
		}else{
			*tstring = temp - 10 + 'a';
		}
		++tstring;
	}while(tvalue);
	*tstring = '\0';
	tstring--;
	while(tstring > first){
		temp = *first;
		*first = *tstring;
		*tstring = temp;
		first++;
		tstring--;
	}
	
	return string;
}

void exit(int exit_code)
{
	
}
