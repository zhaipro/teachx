
/* We should be able to include this file multiple times to allow the assert
   macro to be enabled/disabled for different parts of code.  So don't add a
   header guard.  */ 

#undef assert

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef NDEBUG
/*
 * If not debugging, assert does nothing.
 */
#define assert(x)	((void)0)

#else /* debugging enabled */

void _assert (const char*, const char*, int);

#define assert(e)       ((e) ? (void)0 : _assert(#e, __FILE__, __LINE__))

#endif	/* NDEBUG */

#ifdef	__cplusplus
}
#endif
