#include "album.h"


int main(int argc, char **argv) // ./album  jpg/ ,  ./album  ,  ./album 1.jpg
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

	linkname head = init_list();

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
			list_add_node(new, head);
		}

		// 准备好触摸屏
		int ts = open("/dev/input/event0", O_RDONLY);
		// 使用触摸屏来回浏览这个链表中的图片，形成一个简单相册

		//链表保存是图片名字
		if(head->list.next == &head->list)
		{
			printf("There is no pictrue!\n");
			exit(0);
		}
		linkname node = list_entry(head->list.next, listname, list);
		show_jpeg(fbmem, &vinfo, node);
		while(1)
		{
			node = wait_for_touch(ts, &vinfo, node); 
			show_jpeg(fbmem, &vinfo, node);
		}

	}
	// 直接显示一张jpg/jpeg图片
	else if(S_ISREG(fileinfo.st_mode))
	{
		linkname one = new_node(target);
		show_jpeg(fbmem, &vinfo, one);
	}
	else
	{
		printf("The argument no jpeg\n");
	}

	return 0;
}
