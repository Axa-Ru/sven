/* Sven multimedia keyboard deamon
 * wlist.c
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: more@irpin.com
 * web: http://sven.linux.kiev.ua/
 * icq:325697438
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
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
#include "flag.h"
#include "../../libsven/xlib.h"
#include "../../libsven/emulate_input.h"
#include "xutil.h"
#include "wlist.h"

WInfo* win_add(Window w, AConvert *aconv)
{
	WInfo *winf;
	winf= g_slice_new0(WInfo);
	//winf= g_malloc0(sizeof (WInfo));
	//winf = (WInfo *)calloc(1, sizeof(WInfo));
	
	if (winf) 
	{
		//winf->dpy = XOpenDisplay(XDisplayName(NULL));
		winf->win = w;
		winf->name =  xObject_getWMClass_class (aconv->keyboard->dpy,w);
		winf->cur_group = aconv->sxkb->def_group;
		winf->mode=11;
		aconv->keyboard->list = g_slist_append(aconv->keyboard->list,(gpointer ) winf);
		
		return winf;
	}
	return NULL;
}

WInfo* win_find(Window w,AConvert *aconv)
{
	GSList *l = NULL;

	//printf("Lenght:%d\n",g_slist_length(aconv->keyboard->list));
	
	for (l = aconv->keyboard->list; l; l=l->next)
	{
		WInfo *winf = (WInfo *)l->data;
		if (winf->win == w)
			return winf;
	}
	return NULL;
 }
 
void   win_free(WInfo *winf,AConvert *aconv)
{
/*	DEBUG_MSG("Window Free\n");
	DEBUG_MSG(" ID %#x \n",(unsigned int) winf->win);*/
	aconv->keyboard->list = g_slist_remove (aconv->keyboard->list, (gpointer ) winf);
	g_free(winf->name);
	g_slice_free (WInfo, winf);
//	g_free (winf);
	//printf("Name^:%s\n",winf->name);
/*	winf->win = w;
	winf->name =  xObject_getWMClass_class (aconv->keyboard->dpy,w);
	winf->cur_group = aconv->sxkb->def_group;
	winf->mode=11;
	winf->iterator=0;
	aconv->keyboard->list = g_slist_append(aconv->keyboard->list,(gpointer ) winf);*/
	//g_free (winf);
}

WInfo* AddWindow(Window win,AConvert *aconv)
{
	WInfo *info;
	XWindowAttributes attr;
	if((win == aconv->keyboard->root) || (win == None) || (win == PointerRoot))
		return (WInfo*) 0;
	
	info = win_find(win,aconv);
	if (!info) 
	{
		info = win_add(win,aconv);
		if (!info)
		{
			return (WInfo*) 0;
		}
		else
		{
			//printf("Add Windows ID %#x \n",(unsigned int)win);
			XSelectInput(aconv->keyboard->dpy, win,StructureNotifyMask |  PropertyChangeMask |KeyPressMask);
			
			info->cur_group = aconv->sxkb->def_group;
			XkbLockGroup(aconv->keyboard->dpy, XkbUseCoreKbd, info->cur_group);
			update_flag(aconv->sxkb->group2info[aconv->sxkb->cur_group],aconv);
		}
	}

	if(!XGetWindowAttributes(aconv->keyboard->dpy, win, &attr)) 
	{
		printf("[Sven] XGetWindowAttributes=0; Free Windows ID %#x \n",(unsigned int)win);
		win_free(info,aconv);
		return (WInfo*) 0;
	}
	
	return info;
}

