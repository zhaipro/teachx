
#ifndef __SYS_CALL_
#define __SYS_CALL_
/*
   这里仅定义所有系统调用的类型 
   该文件的作用仅仅是为了一致性，在系统进程与接口之间
   然而，编译后该文件中的内容将统统无用 
*/


#define IPC_RECV		0X01	// 接收消息
#define IPC_SEND		0X02	// 发送消息 
#define IPC_WAIT		0X03	// 等待
#define IPC_SEND_WAIT	0X04	// 发送消息，并等待该消息返回 
#define IPC_FOR_WAIT	0X05	// 

#define IPC_ATTR_WAIT	0X10	// 只可以与 IPC_WAIT 掩码 


#define PID_MM	01	// Memory Manage
#define PID_IO	02	// Input Output
#define PID_HD	03	// Hard Disk
#define PID_PM	04	// Process Manage
#define PID_CLK	04	// Clock

// 模块内部使用的消息类型值必须大于此值 
// 模块对外开放的消息类型值必须小于此值 
#define SC_USER					1000 

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


#define SC_CLK_CLOCK			01
#define SC_CLK_INIT				02
#define SC_CLK_SLEEP			03
#define SC_CLK_TIME				04


#endif
