
#ifndef _CTYPE_H_
#define _CTYPE_H_

/*
 * The following flags are used to tell iswctype and _isctype what character
 * types you are looking for.
 */
#define	_UPPER		0x0001
#define	_LOWER		0x0002
#define	_DIGIT		0x0004
#define	_SPACE		0x0008 /* HT  LF  VT  FF  CR  SP */
#define	_PUNCT		0x0010
#define	_CONTROL	0x0020
/* _BLANK is set for SP and non-ASCII horizontal space chars (eg,
   "no-break space", 0xA0, in CP1250) but not for HT.  */
#define	_BLANK		0x0040
#define	_HEX		0x0080
#define	_LEADBYTE	0x8000

#define	_ALPHA		0x0103

extern unsigned short _pctype[];

#define __ISCTYPE(c, mask)  (_pctype[c] & mask)
int isalnum(int c) {return __ISCTYPE(c, (_ALPHA|_DIGIT));}
int isalpha(int c) {return __ISCTYPE(c, _ALPHA);}
int iscntrl(int c) {return __ISCTYPE(c, _CONTROL);}
int isdigit(int c) {return __ISCTYPE(c, _DIGIT);}
int isgraph(int c) {return __ISCTYPE(c, (_PUNCT|_ALPHA|_DIGIT));}
int islower(int c) {return __ISCTYPE(c, _LOWER);}
int isprint(int c) {return __ISCTYPE(c, (_BLANK|_PUNCT|_ALPHA|_DIGIT));}
int ispunct(int c) {return __ISCTYPE(c, _PUNCT);}
int isspace(int c) {return __ISCTYPE(c, _SPACE);}
int isupper(int c) {return __ISCTYPE(c, _UPPER);}
int isxdigit(int c) {return __ISCTYPE(c, _HEX);}

#endif
