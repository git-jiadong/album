#ifndef __ALBUM_H
#define __ALBUM_H

#include <linux/input.h>
#include <dirent.h>
#include "kernel_list.h"
#include "jpeg.h"

typedef struct list_name
{
	char *name;

	int width;
	int height;
	int pixel_size;

	unsigned char *rgb;

	struct list_head list;
}listname, *linkname;

typedef struct list_show
{
	int cache_num;
	int cache_max_num;
	struct list_name *head;
	struct list_name *pos_show;
	struct list_name *frist_cache;
}listshow, *linkshow;


bool is_jpeg(char *name);
linkname init_list(void);
linkname new_node(char *name);
void list_add_node(linkname new, linkshow manage);
void wait_for_touch(int ts, struct fb_var_screeninfo *vinfo, linkshow manage);
void show_jpeg(unsigned char *fbmem, struct fb_var_screeninfo *vinfo, 
		linkshow manage);

void clean_listname(listname *node);
void init_show_manage(linkshow manage);
linkshow init_list_show(int cache_max_num);

void show_manage(linkshow manage);
void show_name(linkname head);

#endif
