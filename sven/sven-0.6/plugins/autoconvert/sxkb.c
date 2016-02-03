/* Sven multimedia keyboard deamon
 * sxkb.c
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
 
 #include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xlibint.h>
#include <X11/Xatom.h>
 
#include <X11/X.h>
#include <gdk/gdkx.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <locale.h>

#include "autoconvert.h"
#include "sxkb.h"
#include "flag.h"
#include "xutil.h"
#include "wlist.h"

#include "../../libsven/xlib.h"
#include "../../libsven/sven.h"
#include "../../libsven/plugin.h"
#include "../../libsven/pixmap.h"
#include "../../libsven/utils.h"
#include "../../libsven/grabkeyboard.h"

static void flag_menu_activated(GtkWidget *widget, gpointer data);

gpointer flag_menu_create(gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	int i;
	GtkWidget *mi, *img;
	GtkWidget *image;
	
	aconv->sxkb->submenu = gtk_image_menu_item_new_with_mnemonic (_("Layout XKB"));
	gtk_widget_show (aconv->sxkb->submenu);
	
	image = create_pixmap_type(20,16,16);
	gtk_widget_show (image);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (aconv->sxkb->submenu), image);
	
	aconv->sxkb->flag_menu =  gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (aconv->sxkb->submenu), aconv->sxkb->flag_menu);
    
	for (i = 0; i < aconv->sxkb->ngroups; i++) 
	{
		mi = gtk_image_menu_item_new_with_label(aconv->sxkb->group2info[i]->name);
		g_signal_connect(G_OBJECT(mi), "activate", (GCallback)flag_menu_activated, GINT_TO_POINTER(i));
		gtk_menu_shell_append (GTK_MENU_SHELL (aconv->sxkb->flag_menu), mi);
		gtk_widget_show (mi);
		img = gtk_image_new_from_pixbuf(aconv->sxkb->group2info[i]->flag_img);
		if(img==NULL)
					img = create_pixmap_flag("zz",21,14);
		gtk_widget_show(img);
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(mi), img);
	}
	return aconv->sxkb->submenu;
}

void flag_menu_destroy(gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	if (aconv->sxkb->flag_menu) 
	{
		gtk_widget_destroy(aconv->sxkb->flag_menu);
		aconv->sxkb->flag_menu = NULL;
	}
}


void flag_menu_activated(GtkWidget *widget, gpointer data)
{
    int i;

    i = GPOINTER_TO_INT(data);
    XkbLockGroup(GDK_DISPLAY(), XkbUseCoreKbd, i);
}


void read_kbd_description(gpointer data)
{
	XkbDescPtr kb;
	XkbStateRec state;
	AConvert *aconv = (AConvert *)data;
	char *name = NULL;
	int i;
	
	if(aconv==NULL)
		return;
	
	if (! (kb = XkbAllocKeyboard())) 
	{
		if (kb) XkbFreeKeyboard(kb, XkbAllComponentsMask, True);
		return;
	}
	
	if (XkbGetControls(GDK_DISPLAY(), XkbAllControlsMask, kb) == Success) 
	{
        	aconv->sxkb->ngroups = kb->ctrls->num_groups;
        	XkbFreeControls(kb, XkbAllControlsMask, True);
	}
	else
	{
		if (kb) XkbFreeKeyboard(kb, XkbAllComponentsMask, True);
		return;
	}
	
	if (XkbGetNames(GDK_DISPLAY(), XkbGroupNamesMask, kb) != Success) 
	{
		if (kb) XkbFreeKeyboard(kb, XkbAllComponentsMask, True);
		return ;
	}
		for (i = 0; i < aconv->sxkb->ngroups; i++) 
		{
			if (kb->names->groups[i])
			{
				if ((name = XGetAtomName(GDK_DISPLAY(), kb->names->groups[i])))
				{
					xkb_info *info;
					
					info = g_new(xkb_info, 1);
					info->name = g_strdup(name);
					info->flag_img=create_pixbuf_flag(name,25,20);
					aconv->sxkb->group2info[i] = info;
				}
				free(name);name=NULL;
			}
		}

     XkbFreeNames(kb, XkbGroupNamesMask, True);
     if (kb) XkbFreeKeyboard(kb, XkbAllComponentsMask, True);

	if (XkbGetState(GDK_DISPLAY(), XkbUseCoreKbd, &state) == Success) 
	{
		aconv->sxkb->cur_group = state.group;        
	}
}

static GdkFilterReturn
sxkb_filter( XEvent *xev, GdkEvent *event, gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	
	if (xev->type ==  aconv->sxkb->xkb_event_type) 
	{
		XkbEvent *xkbev = (XkbEvent *) xev;
		if (xkbev->any.xkb_type == XkbStateNotify) 
		{
			if(aconv->sxkb->cur_group!=xkbev->state.group)
			{
				aconv->sxkb->cur_group = xkbev->state.group;
				update_flag(aconv->sxkb->group2info[aconv->sxkb->cur_group],aconv);
				
				DEBUG_MSG("SXKB:%d\n",xkbev->state.group);
				
				aconv->keyboard->win=get_active_window(aconv->keyboard->dpy);
				if((aconv->keyboard->win == None) || (aconv->keyboard->win == PointerRoot))
					return 0;
				
				
				WInfo *info = win_find(aconv->keyboard->win,aconv);
				if(info)
					info->cur_group = aconv->sxkb->cur_group;
			}
		}
		else if (xkbev->any.xkb_type == XkbNewKeyboardNotify) 
		{         
			DEBUG_MSG("New Keyboard\n"); 
			read_kbd_description(aconv);
			aconv->sxkb->cur_group = aconv->sxkb->def_group;
			update_flag(aconv->sxkb->group2info[aconv->sxkb->cur_group],aconv);
			flag_menu_destroy(aconv);
			flag_menu_create(aconv); 
		}
		return(GDK_FILTER_REMOVE);
	}
	return(GDK_FILTER_CONTINUE);
}

void sxkb_destroy()
{
	gdk_window_remove_filter(NULL, (GdkFilterFunc)sxkb_filter, NULL);
	XkbSelectEventDetails(GDK_DISPLAY(), XkbUseCoreKbd, XkbStateNotify,
          XkbAllStateComponentsMask, 0UL);
}


int sxkb_init_event(gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	int dummy;
	
	if (!XkbQueryExtension(GDK_DISPLAY (), &dummy, &aconv->sxkb->xkb_event_type, &dummy, &dummy, &dummy))
		return 0;
	XkbSelectEventDetails(GDK_DISPLAY (), XkbUseCoreKbd, XkbStateNotify,
		XkbAllStateComponentsMask, XkbGroupStateMask);
	gdk_window_add_filter(NULL, (GdkFilterFunc)sxkb_filter, aconv);
	
	return 1;
}

static void icon_activated (GtkStatusIcon *icon,gpointer data)
{
	AConvert *aconv = (AConvert *)data;
//	printf("Ok click icon\n");
	int no;
	
	no =  (aconv->sxkb->cur_group + 1) % aconv->sxkb->ngroups;
	XkbLockGroup(GDK_DISPLAY(), XkbUseCoreKbd, no);
}

void init_sxkb(gpointer sdata,gpointer data)
{
	Sven *sven = (Sven *)sdata;
	AConvert *aconv = (AConvert *)data;
	
	if(aconv==NULL)
		return;
	
	aconv->sxkb = g_new0 (Sxkb, 1);

	if (!sxkb_init_event(aconv))
           printf("can't sxkb init. exiting\n");
	
	read_kbd_description(aconv);
	aconv->sxkb->view_flag=1;
	aconv->sxkb->view_flag_tray=1;
	
	sven_cfg_read_int (sven->config->all, "xkb", "def_group",&aconv->sxkb->cur_group);
	sven_cfg_read_int (sven->config->all, "xkb", "view_flag",&aconv->sxkb->view_flag);
	sven_cfg_read_int (sven->config->all, "xkb", "view_flag_tray",&aconv->sxkb->view_flag_tray);
	aconv->sxkb->def_group=aconv->sxkb->cur_group;
	update_flag(aconv->sxkb->group2info[aconv->sxkb->cur_group],aconv);

	g_signal_connect (sven->tray_icon, "activate",G_CALLBACK (icon_activated), aconv);

}


