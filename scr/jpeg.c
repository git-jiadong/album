#include "jpeg.h"

unsigned char *load_jpeg(char *arg, struct image *imageinfo)
{
	int jpeg = open(arg, O_RDWR);
	if(jpeg == -1)
	{
		printf("open jpeg failed %s\n", strerror(errno));
		exit(0);
	}
	
	struct stat file_info;
	stat(arg, &file_info);
	unsigned char *jpeg_buffer = calloc(1, file_info.st_size);
	read_image_from_file(jpeg, jpeg_buffer, file_info.st_size);

	
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, jpeg_buffer, file_info.st_size);

	int ret = jpeg_read_header(&cinfo, true);
	if(ret != 1)
	{
		printf("jpeg_read_header failed: %s\n", strerror(errno));
		exit(0);
	}

	jpeg_start_decompress(&cinfo);
	
	imageinfo->width = cinfo.output_width;
	imageinfo->height = cinfo.output_height;
	imageinfo->pixel_size = cinfo.output_components;

	int row_stride = imageinfo->width * imageinfo->pixel_size;
	unsigned long jpeg_size = imageinfo->width * imageinfo->height * imageinfo->pixel_size;
	unsigned char *rgb_buffer = calloc(1, jpeg_size);

	while(cinfo.output_scanline < cinfo.output_height)
	{
		unsigned char *buffer_array[1];
		buffer_array[0] = rgb_buffer + (cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpeg_buffer);


	return rgb_buffer;
}

void read_image_from_file(int fd, unsigned char *jpg_buffer, unsigned long image_size)
{
	int nread = 0;
	unsigned long read_size = image_size;
	unsigned char *buf = jpg_buffer;
	while(read_size > 0)
	{
		nread = read(fd, buf, read_size);
		if(nread == -1)
		{
			printf("read jpg failed %s\n", strerror(errno));
			exit(0);
		}
		read_size -= nread;
		buf += nread;
	}
}

void write_lcd(unsigned char *FB, struct fb_var_screeninfo *vinfo,
		unsigned char *bmp_buf, struct image *info)
{
	int scr_xoffset = 0, scr_yoffset = 0;
	int im_xoffset = 0, im_yoffset = 0;
	if(vinfo->xres > info->width)
	{
		scr_xoffset = (vinfo->xres - info->width) / 2; 
	}
	else
	{
		im_xoffset = (info->width - vinfo->xres) / 2;
	}
	if(vinfo->yres > info->height)
	{
		scr_yoffset = (vinfo->yres - info->height) / 2; 
	}
	else
	{
		im_yoffset = (info->height - vinfo->yres) / 2;
	}


	FB += (scr_xoffset + scr_yoffset * vinfo->xres) * vinfo->bits_per_pixel/8;
	bmp_buf += (im_xoffset + im_yoffset * info->width) * info->pixel_size;
	int x, y;
	for(y=0; y<info->height - im_yoffset && y<vinfo->yres - scr_yoffset; y++)
	{
		for(x=0; x<info->width - im_xoffset && x<vinfo->xres - scr_xoffset; x++)
		{
			int lcd_offset = (x + y * vinfo->xres) 
					* vinfo->bits_per_pixel/8;
			int bmp_offset = (x + y * info->width) 
					* info->pixel_size;

			memcpy(FB + lcd_offset + vinfo->red.offset/8, 
					bmp_buf + bmp_offset + 0, 1);
			memcpy(FB + lcd_offset + vinfo->green.offset/8, 
					bmp_buf + bmp_offset + 1, 1);
			memcpy(FB + lcd_offset + vinfo->blue.offset/8, 
					bmp_buf + bmp_offset + 2, 1);
		}
	}
	
}

unsigned char *init_lcd(struct fb_var_screeninfo *pvinfo)
{
	
	int lcd = open("/dev/fb0", O_RDWR);
	if(lcd == -1)
	{
		printf("open() lcd failed %s\n", strerror(errno));
		exit(0);
	}

	ioctl(lcd, FBIOGET_VSCREENINFO, pvinfo);
	int bpp = pvinfo->bits_per_pixel;
	unsigned char *FB = mmap(NULL, pvinfo->xres * pvinfo->yres * bpp/8, 
				PROT_WRITE|PROT_READ, MAP_SHARED, lcd, 0);
	if(FB == MAP_FAILED)
	{
		printf("mmap() failed %s\n", strerror(errno));
		exit(0);
	}

	return FB;
}




