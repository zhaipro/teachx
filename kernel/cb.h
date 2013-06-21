
#ifndef _CB_H_
#define _CB_H_

struct cb_cb_t{
	struct cb_t *free_list;
	void *buff_hdr;
};

struct cb_t{
	struct cb_t *next;
};

void create_cb(struct cb_cb_t *cb_cb,int size,int count)
{
	
}
void* alloc_cb(struct cb_cb_t *cb_cb)
{
	struct cb_t cb;
	cb = cb_cb->free_list;
	if(cb){
		cb_cb->free_list = cb->next;
	}
	return cb;
}
void free_cb(struct cb_cb_t *cb_cb,void *cb)
{
	struct cb_t _cb = (struct cb_t*)cb;
	_cb->next = cb_cb->free_list;
	cb_cb->free_list = _cb;
}

#endif
