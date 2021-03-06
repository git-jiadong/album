#include "album.h"

bool is_jpeg(char *name)
{
	int i;
	for(i=0; name[i] != '\0'; i++)
	{
		if(name[i] == '.')
		{	
			if(name[i+1] == 'j' && name[i+2] == 'p' && name[i+3] == 'g')
				return true;
		}
	}

	return false;
}



linkname new_node(char *name)
{
	linkname new = malloc(sizeof(listname));
	if(new != NULL)
	{
		new->name = name;
		INIT_LIST_HEAD(&new->list);
		new->width = 0;
		new->height = 0;
		new->pixel_size = 0;
		new->rgb = NULL;
	}

	return new;
}

void list_add_node(linkname new, linkshow manage)
{
	if(manage->head == NULL)
		manage->head = new;
	list_add(&new->list, &(manage->head->list));
}

void wait_for_touch(int ts, struct fb_var_screeninfo *vinfo, linkshow manage)
{
	struct input_event buf;
	bzero(&buf, sizeof(buf));
	
	int where;
	while(1)
	{
		read(ts, &buf, sizeof(buf));

		if(buf.type == EV_ABS)
		{
			if(buf.code == ABS_X)
				where = buf.value;
		}

		if(buf.type == EV_KEY)
		{
			if(buf.code == BTN_TOUCH && buf.value == 0)
				break;
		}
	}
	linkname node = manage->pos_show;
	if(where > vinfo->xres/2)
	{
		manage->pos_show = list_entry(node->list.next, listname, list);
	}
	else
	{
		manage->pos_show = list_entry(node->list.prev, listname, list);
		
	}
}
/*清除节点里的缓存数据*/
void clean_listname(listname *node)
{
	free(node->rgb);
	node->width = 0;
	node->height = 0;
	node->pixel_size = 0;
	node->rgb = NULL;
}
/*初始化管理结构体里面的数据*/
void init_show_manage(linkshow manage)
{
	if(manage->head == NULL)
	{
		printf("The dirtctory NOT JPG\n");
		exit(0);
	}

	manage->pos_show = manage->head;
	manage->frist_cache = manage->head;
}

void show_jpeg(unsigned char *fbmem, struct fb_var_screeninfo *vinfo, linkshow manage)
{
	struct image iminfo;
	/*当最大缓存数为零时，解压图片并显示后释放图片缓存*/
	if(manage->cache_max_num == 0)
	{
		manage->pos_show->rgb = load_jpeg(manage->pos_show->name, &iminfo);
		write_lcd(fbmem, vinfo, manage->pos_show->rgb, &iminfo);
		free(manage->pos_show->rgb);
	}
	/*当缓存数不为零，切该图片没有缓存时进行解压并显示及缓存*/
	else if(manage->pos_show->rgb == NULL)
	{
		manage->pos_show->rgb = load_jpeg(manage->pos_show->name, &iminfo);
		/*当缓存满了的时候，释放第一张缓存的图片*/
		if(manage->cache_num == manage->cache_max_num)
		{
			linkname tmp = manage->frist_cache;
			manage->frist_cache = list_entry(tmp->list.next, listname, list);
			clean_listname(tmp);
		}
		else
		{
			manage->cache_num++;
		}

		write_lcd(fbmem, vinfo, manage->pos_show->rgb, &iminfo);
	}
	/*有缓存的图片直接显示*/
	else
		write_lcd(fbmem, vinfo, manage->pos_show->rgb, &iminfo);
}

/*创建一个初始化管理结构体*/
linkshow init_list_show(int cache_max_num)
{
	linkshow manage = malloc(sizeof(listshow));
	if(manage != NULL)
	{
		manage->cache_num = 0;
		manage->cache_max_num = cache_max_num;
		manage->head = NULL;
		manage->pos_show = NULL;
		manage->frist_cache = NULL;
	}
	
	return manage;
}
/******************************
  调试打印
void show_manage(linkshow manage)
{
	printf("cache_num %d\n", manage->cache_num);
	printf("cache_max_num %d\n", manage->cache_max_num);
	printf("head %p\n", manage->head);
	printf("pos_show %p\n", manage->pos_show);
	printf("frist_cache %p\n", manage->frist_cache);
}

void show_name(linkname head)
{
	struct list_head *pos;
	linkname tmp;
	pos = &head->list;
	tmp = list_entry(pos, listname, list);
	printf("%s\n", tmp->name);
	
	for(pos=head->list.next; pos!=&head->list; pos=pos->next)
	{
		tmp = list_entry(pos, listname, list);
		printf("%s\n", tmp->name);
	}
}
************************************/
