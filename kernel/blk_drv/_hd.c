
#include "_hd.h"
#include "_process.h"
#include "_sys_call.h"

int _hd_write_msg(const void *buf,u32 sector,u8 nsect)
{
	struct msg_t msg;
	msg.type = SC_HD_WRITE;
	msg.p0.cpparam = buf;
	msg.p1.uiparam = sector;
	msg.p2.uiparam = nsect;

	return ipc_send(PID_HD,&msg,TRUE);
}

bool_t hd_write_msg(const void *buf,u32 sector,u8 nsect)
{
	struct msg_t msg;
	msg.type = SC_HD_WRITE;
	msg.p0.cpparam = buf;
	msg.p1.uiparam = sector;
	msg.p2.uiparam = nsect;

	return ipc_send_wait(PID_HD,&msg);
}

int _hd_read_msg(void *buf,u32 sector,u8 nsect)
{
	struct msg_t msg;
	msg.type = SC_HD_READ;
	msg.p0.pparam = buf;
	msg.p1.uiparam = sector;
	msg.p2.uiparam = nsect;

	return ipc_send(PID_HD,&msg,TRUE);
}

bool_t hd_read_msg(void *buf,u32 sector,u8 nsect)
{
	struct msg_t msg;
	msg.type = SC_HD_READ;
	msg.p0.pparam = buf;
	msg.p1.uiparam = sector;
	msg.p2.uiparam = nsect;
	
	return ipc_send_wait(PID_HD,&msg);
}
