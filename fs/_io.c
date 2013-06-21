
#include "_process.h"
#include "_sys_call.h"

int close(handle_t h_file)
{
	struct msg_t msg;
	
	msg.type = SC_CLOSE;
	msg.p0.iparam = h_file;
	return ipc_send_wait(PID_IO,&msg);
}

int eof(handle_t h_file)
{
	struct msg_t msg;
	
	msg.type = SC_EOF;
	msg.p0.iparam = h_file;
	return ipc_send_wait(PID_IO,&msg);
}

long filelength(handle_t h_file)
{
	struct msg_t msg;
	
	msg.type = SC_FILELENGTH;
	msg.p0.iparam = h_file;
	return ipc_send_wait(PID_IO,&msg);
}

long lseek(handle_t h_file,long offset,int origin)
{
	struct msg_t msg;

	msg.type = SC_LSEEK;
	msg.p0.iparam = h_file;
	msg.p1.lparam = offset;
	msg.p2.iparam = origin;
	return ipc_send_wait(PID_IO,&msg);
}

int read(handle_t h_file,void *buf,size_t count)
{
	struct msg_t msg;

	msg.type = SC_LSEEK;
	msg.p0.iparam = h_file;
	msg.p1.pparam = buf;
	msg.p2.uiparam = count;
	return ipc_send_wait(PID_IO,&msg);
}

long tell(handle_t h_file)
{
	struct msg_t msg;
	
	msg.type = SC_TELL;
	msg.p0.iparam = h_file;
	return ipc_send_wait(PID_IO,&msg);
}

int write(handle_t h_file,const void *buf,size_t count)
{
	struct msg_t msg;

	msg.type = SC_LSEEK;
	msg.p0.iparam = h_file;
	msg.p1.pparam = (void*)buf;
	msg.p2.uiparam = count;
	return ipc_send_wait(PID_IO,&msg);
}
