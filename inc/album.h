#ifndef __ALBUM_H
#define __ALBUM_H

#include <linux/input.h>
#include <dirent.h>
#include "kernel_list.h"
#include "jpeg.h"

typedef struct list_name
{
	char *name;
	struct list_head list;
}listname, *linkname;


bool is_jpeg(char *name);
linkname init_list(void);
linkname new_node(char *name);
void list_add_node(linkname new, linkname head);
linkname wait_for_touch(int ts, struct fb_var_screeninfo *vinfo, linkname node);
void show_jpeg(unsigned char *fbmem, struct fb_var_screeninfo *vinfo, linkname node);


#endif
