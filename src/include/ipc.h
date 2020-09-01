
#ifndef _IPC_H_
#define _IPC_H_

struct msg_t{
    int p0;     // param
    int p1;
    int p2;
};

#define IPC_SEND    0X01    // 发送消息 or 接收消息

#endif
