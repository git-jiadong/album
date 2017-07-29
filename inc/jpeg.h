#ifndef __JPEG_H
#define __JPEG_H

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <stdbool.h>
#include "jpeglib.h"
#include "album.h"

struct image
{
	int width;
	int height;
	int pixel_size;
};


unsigned char *init_lcd(struct fb_var_screeninfo *pvinfo);
void read_image_from_file(int fd, unsigned char *jpg_buffer, unsigned long image_size);
unsigned char *load_jpeg(char *arg, struct image *imageinfo);

void write_lcd(unsigned char *FB, struct fb_var_screeninfo *vinfo,
		unsigned char *bmp_buf, struct image *info);

#endif

