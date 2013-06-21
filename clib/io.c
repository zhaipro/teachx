


int read (int handle, void* buf, unsigned int size);
{
	msg_t msg;
	msg.iparam = handle;
	msg.pparam = buf;
	msg.uiparam = size;
	ipc_send(PID_IO,&msg,TRUE);
}
