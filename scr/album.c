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


linkname init_list(void)
{
	linkname head = malloc(sizeof(listname));
	if(head != NULL)
	{
		head->name = NULL;
		INIT_LIST_HEAD(&head->list);	
	}

	return head;
}

linkname new_node(char *name)
{
	linkname new = malloc(sizeof(listname));
	if(new != NULL)
	{
		new->name = name;
		INIT_LIST_HEAD(&new->list);
	}

	return new;
}

void list_add_node(linkname new, linkname head)
{
	list_add(&new->list, &head->list);
}

linkname wait_for_touch(int ts, struct fb_var_screeninfo *vinfo, linkname node)
{
	linkname show_node = node;
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

	if(where > vinfo->xres/2)
	{
		show_node = list_entry(node->list.next, listname, list);
		if(show_node->name != NULL)
		{
			return show_node;
		}
		else
		{
			show_node = list_entry(show_node->list.next, listname, list);
			return show_node;
		}
	}
	else
	{
		show_node = list_entry(node->list.prev, listname, list);
		if(show_node->name != NULL)
		{
			return show_node;
		}
		else
		{
			show_node = list_entry(show_node->list.prev, listname, list);
			return show_node;
		}
		
	}
}

void show_jpeg(unsigned char *fbmem, struct fb_var_screeninfo *vinfo, linkname node)
{
	struct image iminfo;
	unsigned char *show_buf = load_jpeg(node->name, &iminfo);

	write_lcd(fbmem, vinfo, show_buf, &iminfo);
	free(show_buf);
}

