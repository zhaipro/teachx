
#ifndef __SYS_CALL_
#define __SYS_CALL_
/*
   这里仅定义所有系统调用的类型 
   该文件的作用仅仅是为了一致性，在系统进程与接口之间
   然而，编译后该文件中的内容将统统无用 
*/

#define PID_MM	01	// Memory Manage
#define PID_IO	02	// Input Output
#define PID_PM	03	// Process Manage
#define PID_HD	04	// Hard Disk

// 进程管理进程的消息类型 
#define SC_PTHREAD_KEY_CREATE	01
#define SC_PTHREAD_KEY_DELETE	02
#define SC_FORK					03

#define SC_CLOSE				01
#define SC_EOF					02
#define SC_FILELENGTH			03
#define SC_LSEEK				04
#define SC_READ					05
#define SC_TELL					06
#define SC_WRITE				07

#define SC_HD_READ				01
#define SC_HD_WRITE				02

#endif
