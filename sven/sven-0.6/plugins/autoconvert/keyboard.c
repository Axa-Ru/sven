
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xlibint.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "autoconvert.h"
#include "convert.h"
#include "keyboard.h"
#include "wlist.h"
#include "glist.h"
#include "flag.h"
#include "xutil.h"

#include "../../libsven/sven.h"
#include "../../libsven/plugin.h"
#include "../../libsven/xlib.h"
#include "../../libsven/grabkeyboard.h"

gboolean get_key_win(gpointer data);

static int timer_id=0;

void grab_keyboard_init (gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	
	if(aconv==NULL)
		return;
	  
	aconv->keyboard = g_slice_new0(Mkeyboard);
//	aconv->keyboard = g_new0(Mkeyboard, 1);

	aconv->keyboard->dpy = XOpenDisplay(XDisplayName(NULL));
	aconv->keyboard->scr = DefaultScreen(aconv->keyboard->dpy);
	aconv->keyboard->root = RootWindow(aconv->keyboard->dpy,aconv->keyboard->scr);
	aconv->keyboard->gstr=g_string_new (NULL);
	aconv->keyboard->iterator=0;

	XSelectInput(aconv->keyboard->dpy, aconv->keyboard->root, SubstructureNotifyMask);

	timer_id=g_timeout_add (10,update_window_key_event, aconv);
}
void grab_keyboard_destroy (gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	
	if(aconv==NULL)
		return;

	XCloseDisplay(aconv->keyboard->dpy);

	g_source_remove(timer_id);
}


int grab_wind_event(AConvert *aconv)
{
	gboolean isSameScreen;
	int dummy;
	unsigned int dummyU;
	Window rootW = 0, childW = 0;

	WInfo *info = win_find(aconv->keyboard->win,aconv);
	if(info)
	{
		childW = aconv->keyboard->win;
		XGetInputFocus (aconv->keyboard->dpy,&childW, &(aconv->keyboard->cur_status));
	
		if ( (childW == None) || (childW == PointerRoot))
			return -1;
	
		//printf ("window: ID %#x \n",(unsigned int)childW);
		
		XSelectInput(aconv->keyboard->dpy, childW,  SubstructureNotifyMask|KeyPressMask);
			
		isSameScreen = XQueryPointer(aconv->keyboard->dpy,childW, &rootW, &childW, &dummy,&dummy, &(dummy), &(dummy), &dummyU);
	
		if ( (childW == None) || (childW == PointerRoot))
			return -1;
	
		XSelectInput(aconv->keyboard->dpy, childW, KeyPressMask);
		
		if (isSameScreen == TRUE) 
		{
			while (childW != 0) 
			{
				if ( (childW == None) || (childW == PointerRoot))
					break;
		
				isSameScreen = XQueryPointer (aconv->keyboard->dpy,childW, &rootW,&childW, &dummy,&dummy, &(dummy), &(dummy), &dummyU);
	
				if ( (childW == None) || (childW == PointerRoot))
					break;
		
				XSelectInput(aconv->keyboard->dpy, childW, KeyPressMask);
			}
		}
	}
	return 1;
}

int get_find_name_to_grab(AConvert *aconv,WInfo *info)
{
	ConfigSection *sect;
	
	Sven *sven = (Sven *)aconv->sven;
	
	g_return_val_if_fail(sven->config->all != NULL, FALSE);
	
	//printf("Name:%s\n",info->name);
		
	if ((g_strcasecmp(info->name,"sven")==0)||(g_strcasecmp(info->name,"Xterm")==0))
		return 2;
		
	if (!(sect = sven_cfg_find_section(sven->config->all, "win_list")))
		return -1;
	if (!sven_cfg_find_string(sect,(info->name)))
		return -1;
	
	return 2;
}


gboolean  update_window_key_event(gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	
	aconv->keyboard->win=get_active_window(aconv->keyboard->dpy);
	
	if((aconv->keyboard->win == None) || (aconv->keyboard->win == PointerRoot))
		return TRUE;
		
	WInfo *info = AddWindow(aconv->keyboard->win,aconv);
		
	if(info)
	{
		switch (get_find_name_to_grab(aconv,info)) 
		{     
			case -1:
				grab_wind_event(aconv);
				get_key_win(aconv);
			break;
			case 0:
				grab_wind_event(aconv);
				get_key_win(aconv);
			break;
			case 1:
				get_key_win(aconv);
			break;
			case 2:
				return TRUE;
			default:
			break;
		}
	}
	//get_key_win(aconv);
	return TRUE;
}

/****--------------------------------------------------------------------***/

void add_char_to_win(AConvert *aconv,XEvent * xev,WInfo *info)
{
	int slen;
	
	if((aconv->keyboard->gstr->str[aconv->keyboard->gstr->len-1] ==' '))
	{
		free_keysym_to_list(aconv);
	}

	slen = aconv->keyboard->gstr->len;
		
	if(slen>STRING_SIZE)
	{
		free_keysym_to_list(aconv);
		return;
		
	}

	if (aconv->keyboard->iterator <= 0) 
	{
		if (info->mode == KLB_INSERT_MODE) 
		{
			if(slen >0)
				add_keysym_to_list_num(xev,aconv,slen);
			else
				add_keysym_to_list(xev,aconv);
		}
		else
			add_keysym_to_list(xev,aconv);
	}
	else if (aconv->keyboard->iterator > 0) 
	{
		if (info->mode == KLB_INSERT_MODE) 
		{
			if(slen >0)
			{
				add_keysym_to_list_num_insert(xev,aconv,slen-aconv->keyboard->iterator);
				if(aconv->keyboard->iterator > 0)
					aconv->keyboard->iterator--;
				else
					aconv->keyboard->iterator=0;
			}
			else
				add_keysym_to_list(xev,aconv);
		}
		else
			add_keysym_to_list_num(xev,aconv,slen-aconv->keyboard->iterator);
	}
	else 
	{
		free_keysym_to_list(aconv);
	}
}

void get_key_pres(gpointer data,XEvent *event)
{
	AConvert *aconv = (AConvert *)data;
	WInfo *info = win_find(aconv->keyboard->win,aconv);
	if(info)
	{
		int action=0;
		int slen;
	
	
		if(!info)
		{
			return ;
		}
	
		action=get_key_action (event);
		slen = aconv->keyboard->gstr->len;//g_list_length(aconv->keyboard->ks);
		
		switch (action) 
		{
			case KLB_NO_ACTION:
				free_keysym_to_list(aconv);
				return ;
			case KLB_CLEAR:
				free_keysym_to_list(aconv);
				return ;
			case KLB_ENTER:
				free_keysym_to_list(aconv);
				return ;
			case KLB_FLUSH:
				add_char_to_win(aconv,event,info);
			break;
			case KLB_SWITCH_MODE:
				if (KLB_INSERT_MODE == info->mode)
					info->mode = KLB_REPLACE_MODE;
				else
					info->mode = KLB_INSERT_MODE;
				return ;
			case KLB_MOVE_LEFT:
				if ((slen >0) && (aconv->keyboard->iterator <= slen))
					aconv->keyboard->iterator++;
				else 
				{
					free_keysym_to_list(aconv);
				}
				return ;
			case KLB_MOVE_RIGHT:
				if (aconv->keyboard->iterator <= slen)
					aconv->keyboard->iterator--;
				else 
				{
					free_keysym_to_list(aconv);
				}
				return;
			case KLB_DEL_SYM_LEFT:
				if (aconv->keyboard->iterator <= 0) 
				{
					if(slen>0)
					{
						dell_keysym_to_list(aconv,slen);
					}
				}
				else if (aconv->keyboard->iterator >= 1) 
				{
					if(slen>0)
					{
						dell_keysym_to_list(aconv,slen-aconv->keyboard->iterator);
					}
				}
				else 
				{
					free_keysym_to_list(aconv);
				}
			break;
			case KLB_DEL_SYM_RIGHT:
				if (aconv->keyboard->iterator <= 0) 
				{
					free_keysym_to_list(aconv);
				}
				else if (aconv->keyboard->iterator >= 1) 
				{
					dell_keysym_to_list(aconv,(slen-aconv->keyboard->iterator)+1);
					aconv->keyboard->iterator--;
				}
			break;
			case KLB_ADD_SYM:
				add_char_to_win(aconv,event,info);
			break;
			default:
			break;
		}
	}
}


gboolean get_key_win(gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	XEvent xev;
	WInfo *info;
	Window w;
 	
		
	info = win_find(aconv->keyboard->win,aconv);

	if(info)
	{
		if(aconv->sxkb->cur_group!=info->cur_group)
		{
			XkbLockGroup(aconv->keyboard->dpy, XkbUseCoreKbd, info->cur_group);
			update_flag(aconv->sxkb->group2info[aconv->sxkb->cur_group],aconv);
		}	
		
		w=get_active_window(aconv->keyboard->dpy);
		if(info->win!=w)
			free_keysym_to_list(aconv);
	}
	{
		if (XPending(aconv->keyboard->dpy))
		{
			XNextEvent (aconv->keyboard->dpy, &xev);
			switch (xev.type) 
			{       
				case KeyPress:
					if (xev.xkey.send_event == TRUE)
						return False;
						
					get_key_pres(aconv,&xev);
					info = win_find(aconv->keyboard->win,aconv);
					if(info)
						auto_convert_text(aconv,info);
				break;
				case DestroyNotify:
				
				if((xev.xdestroywindow.event == aconv->keyboard->root)) break; 
			/*	XGetInputFocus (aconv->keyboard->dpy,&w, &(aconv->keyboard->cur_status));
				DEBUG_MSG("DestroyNotify ID %#x == ID %#x ++ ID %#x == ID %#x == ID %#x \n",(unsigned int) aconv->keyboard->win,(unsigned int)info->win,(unsigned int)w,(unsigned int)xev.xdestroywindow.window,(unsigned int)xev.xdestroywindow.event);*/
				
				info = win_find(xev.xdestroywindow.event,aconv);
				if (info) 
				{
					//printf ("Dest window: ID %#x \n",(unsigned int)info->win);
					win_free(info,aconv);
				}
				break;
			}
		}
	}
	return TRUE;
}


