#include "album.h"


int main(int argc, char **argv) 
{
	if(argc > 2)
	{
		printf("invalid argument\n");
		exit(0);
	}

	char *target = argc==2 ? argv[1] : ".";

	struct stat fileinfo;
	bzero(&fileinfo, sizeof(fileinfo));
	stat(target, &fileinfo);

	struct fb_var_screeninfo vinfo;
	unsigned char *fbmem = init_lcd(&vinfo);
	/*设置缓存解压图片的参数*/
	linkshow manage = init_list_show(0);

	if(S_ISDIR(fileinfo.st_mode))
	{
		DIR *dp = opendir(target);
		chdir(target);

		// 将指定目录中的jpg/jpeg图片妥善地保存起来
		struct dirent *ep;
		while(1)
		{
			ep = readdir(dp);
			if(ep == NULL)
				break;

			bzero(&fileinfo, sizeof(fileinfo));
			stat(ep->d_name, &fileinfo);
			// 遇到一个非普通文件
			if(!S_ISREG(fileinfo.st_mode))
				continue;
			// 遇到一个非jpg/jpeg的普通文件，直接跳过
			if(!is_jpeg(ep->d_name))
				continue;
			// 现在就是一张jpg/jpeg图片
			// 将这张图片的相关信息、图像数据插入链表
			linkname new = new_node(ep->d_name);
			list_add_node(new, manage);
		}

		// 准备好触摸屏
		int ts = open("/dev/input/event0", O_RDONLY);
		// 使用触摸屏来回浏览这个链表中的图片，形成一个简单相册

		/*显示第一张图片，如果没有图片退出*/
		init_show_manage(manage);
		show_jpeg(fbmem, &vinfo, manage);
		while(1)
		{
			wait_for_touch(ts, &vinfo, manage); 
			show_jpeg(fbmem, &vinfo, manage);
		}

	}
	// 直接显示一张jpg/jpeg图片
	else if(S_ISREG(fileinfo.st_mode))
	{
		if(is_jpeg(target))
		{
			linkname new = new_node(target);
			list_add_node(new, manage);
			init_show_manage(manage);
			show_jpeg(fbmem, &vinfo, manage);
		}
		else
			printf("The argument no jpeg\n");
	}
	else
	{
		printf("The argument no jpeg\n");
	}

	return 0;
}
