
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef unsigned short	u16;
typedef unsigned int	u32;
typedef unsigned char	byte;

#define DELETED	0XE5
#define ENABLE	0

#define ATTR_GENERAL	0		// 普通科读写文件 
#define ATTR_READONLY	1		// 只读文件 
#define ATTR_HIDE		2		// 隐藏文件 
#define ATTR_SYSTEM		4		// 系统文件 
#define ATTR_LABEL		8		// 卷标 
#define ATTR_DIR		0X10	// 目录文件 

struct dir_item_t{
	byte filename[8];
	byte extname[3];
	byte attr;
	byte reserve[10];
	u16  time;
	u16  date;
	u16  cluster;
	u32  size;
};

typedef char assert_size[sizeof(struct dir_item_t)==32?1:-1];

struct date_t{
	u32 hour;
	u32 minute; 
	u32 second;
	
	u32 year;
	u32 month;
	u32 day;
};

byte fat[9*512];
FILE *fp;

static void get_date(struct dir_item_t *dir_item,struct date_t *date)
{
	date->hour = dir_item->time>>11;
	date->minute = (dir_item->time>>5)&0x3f;
	date->second = (dir_item->time&0x1f)<<1;
	
	date->year = (dir_item->date>>9) + 1980;
	date->month = (dir_item->date>>5)&0xf;
	date->day = dir_item->date&0x1f;
}

static void set_date(struct dir_item_t *dir_item,struct date_t *date)
{
	dir_item->time = (date->hour<<11) | (date->minute<<5) | (date->second>>1);
	dir_item->date = ((date->year-1980)<<9) | (date->month<<5) | date->day;
}

struct dir_item_t* get_free_item(struct dir_item_t *dir_item,int count)
{
	int i;
	
	for(i=0;i<count;i++)
	{
		if(dir_item[i].filename[0]==0 || dir_item[i].filename[0]==DELETED)
			return &dir_item[i];		
	}

	return NULL;
}

struct dir_item_t* search(struct dir_item_t *dir_item,int count,const char *filename)
{
	int i;
	
	for(i=0;i<count;i++)
	{
		if(0 == memcmp(dir_item[i].filename,filename,11)){
			return &dir_item[i];
		}
	}
	return NULL;
}

static void set_next(u32 cluster,u32 next_cluster)
{
	const int pos = cluster*1.5;

	if(cluster % 2){
		*(u16*)(&fat[pos]) &= 0x000f;
		*(u16*)(&fat[pos]) |= (next_cluster<<4);
	}else{
		
		*(u16*)(&fat[pos]) &= 0xf000;
		*(u16*)(&fat[pos]) |= next_cluster;
	}
}

static u32 get_next(u32 cluster)
{
	u32 next_cluster;
	const int pos = cluster*1.5;
	
	next_cluster = *(u16*)(&fat[pos]);
	if(cluster % 2){
		next_cluster >>= 4;
	}else{
		next_cluster &= 0x0fff;
	}
	
	return next_cluster;
}

// 得到可用簇的簇号 
static u32 get_free_cluster()
{
	int i;
	u32 next_cluster;
	for(i=2;i<9*512/1.5;i++)
	{
		next_cluster = get_next(i);
		if(next_cluster == 0)
			return i;
	}
	return 0;
}

// 将簇号转换成扇区号 
static u32 cluster_sector(u32 cluster)
{
	return 33 + (cluster-2)*1;
}

static void delete_file(struct dir_item_t *dir_item)
{
	int next_cluster;
	int cluster = dir_item->cluster; 
	
	memset(dir_item,0,sizeof(struct dir_item_t));
	dir_item->filename[0] = DELETED;
	
	while(cluster != 0xfff)
	{
		next_cluster = get_next(cluster);
		set_next(cluster,0);
		cluster = next_cluster;
	}
}

void write_file(struct dir_item_t *dir_item,void *buf,int offset,int size)
{
	u32 next_cluster = dir_item->cluster;
	u32 cur_sector;
	u32 cur_cluster = 0;
	void *next_buf = buf;
	
	dir_item->size = size;
	
	for(;size>0;size-=512)
	{
		if(next_cluster == 0xfff){
			u32 free_cluster = get_free_cluster();
			if(cur_cluster)
				set_next(cur_cluster,free_cluster);
			else
				dir_item->cluster = free_cluster;
			set_next(free_cluster,0xfff);
			cur_cluster = free_cluster;
		}else{
			cur_cluster = next_cluster;
		}
		
		cur_sector = cluster_sector(cur_cluster);
		
		fseek(fp,cur_sector*512,SEEK_SET);
		fwrite(next_buf,512,1,fp);
		
		next_buf += 512;
		next_cluster = get_next(cur_cluster);
	}
	
	if(next_cluster != 0xfff){
		if(cur_cluster){
			set_next(cur_cluster,0xfff);
			cur_cluster = next_cluster;
			while(cur_cluster != 0xfff)
			{
				next_cluster = get_next(cur_cluster);
				set_next(cur_cluster,0);
				cur_cluster = next_cluster;
			}
		}else{
			dir_item->cluster = 0xfff;
		}
	}
}

void print_dir_item(struct dir_item_t *dir_item,int count)
{
	int i;
	struct date_t date;
	
	for(i=0;i<224;i++)
	{
		if(dir_item[i].filename[0] != DELETED && dir_item[i].filename[0] != 0){
			printf("%11.11s  ",dir_item[i].filename,dir_item[i].extname);
			get_date(&dir_item[i],&date);
			printf("%4d-%02d-%02d  %02d:%02d:%02d",date.year,date.month,date.day,
				date.hour,date.minute,date.second);
			printf("   %d\n",dir_item[i].size);
		}else if(dir_item[i].filename[0] == 0){
			break;
		}
	}
}

void make_date(struct date_t *date)
{
	time_t t = time(NULL);
	struct tm *struct_tm;
	struct_tm = localtime(&t);
	date->hour = struct_tm->tm_hour;
	date->minute = struct_tm->tm_min;
	date->second = struct_tm->tm_sec;
	date->year = struct_tm->tm_year + 1900;
	date->month = struct_tm->tm_mon + 1;
	date->day = struct_tm->tm_mday;
}

int main()
{
	struct dir_item_t dir_item[224];
	struct dir_item_t *pdir_item;
	FILE *setup;
	int setup_size;
	struct date_t date;
	
	fp = fopen("tinix.img","rb+");
	
	fseek(fp,512,SEEK_SET);
	fread(fat,512,9,fp);
	fseek(fp,19*512,SEEK_SET);
	fread(dir_item,sizeof(struct dir_item_t),224,fp);
	
	make_date(&date);

	pdir_item = search(dir_item,224,"SETUP   BIN");
	if(pdir_item == NULL){
		pdir_item = get_free_item(dir_item,244);
		memcpy(pdir_item->filename,"SETUP   BIN",11);
		pdir_item->attr = 0;
		pdir_item->cluster = 0xfff;
	}
	set_date(pdir_item,&date);

	setup = fopen("setup.bin","rb");
	fseek(setup,0,SEEK_END);
	setup_size = ftell(setup);
	fseek(setup,0,SEEK_SET);
	byte s[setup_size];
	fread(s,setup_size,1,setup);
	write_file(pdir_item,s,0,setup_size);
	fclose(setup);

	fseek(fp,512,SEEK_SET);
	fwrite(fat,512,9,fp);
	fseek(fp,19*512,SEEK_SET);
	fwrite(dir_item,sizeof(struct dir_item_t),224,fp);
	fclose(fp);
	return 0;
}
