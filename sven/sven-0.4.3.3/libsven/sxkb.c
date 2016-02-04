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

#include "sven.h"
#include "utils.h"
#include "sxkb.h"

static void destroy_all();
static void flag_menu_activated(GtkWidget *widget, gpointer data);
void descend_tree (Window w, unsigned long type);

void flag_menu_create(gpointer data)
{
	Sven *sven = (Sven *)data;
    int i;
    GdkPixbuf *flag;
    GtkWidget *mi, *img;
    

	sven->sxkb->flag_menu =  gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (sven->sxkb->submenu), sven->sxkb->flag_menu);
    
    for (i = 0; i < sven->sxkb->ngroups; i++) {
        mi = gtk_image_menu_item_new_with_label(sven->sxkb->group2info[i]->name);
        g_signal_connect(G_OBJECT(mi), "activate", (GCallback)flag_menu_activated, GINT_TO_POINTER(i));
        gtk_menu_shell_append (GTK_MENU_SHELL (sven->sxkb->flag_menu), mi);
        gtk_widget_show (mi);
        img = gtk_image_new_from_pixbuf( new_pixbuf_flag(sven->sxkb->group2info[i]->name,21,14));
	if(img==NULL)
				img = new_pixmap_flag("zz",21,14);
        gtk_widget_show(img);
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(mi), img);
    }
}

void flag_menu_destroy(gpointer data)
{
	Sven *sven = (Sven *)data;
    if (sven->sxkb->flag_menu) {
        gtk_widget_destroy(sven->sxkb->flag_menu);
        sven->sxkb->flag_menu = NULL;
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
	Sven *sven = (Sven *)data;
	char *name = NULL;
	int i;
	
	if (! (kb = XkbAllocKeyboard())) 
	{
		//printf("XkbAllocKeyboard()\n");
		if (kb) XkbFreeKeyboard(kb, XkbAllComponentsMask, True);
		return;
	}
	
	if (XkbGetControls(GDK_DISPLAY(), XkbAllControlsMask, kb) == Success) 
	{
        	sven->sxkb->ngroups = kb->ctrls->num_groups;
        	XkbFreeControls(kb, XkbAllControlsMask, True);
	}
	else
	{
		if (kb) XkbFreeKeyboard(kb, XkbAllComponentsMask, True);
		return;
	}
	
	if (XkbGetNames(GDK_DISPLAY(), XkbGroupNamesMask, kb) != Success) 
	{
		//printf("XkbGetNames()\n");
		if (kb) XkbFreeKeyboard(kb, XkbAllComponentsMask, True);
		return ;
	}
	
		for (i = 0; i < sven->sxkb->ngroups; i++) 
		{
			if (kb->names->groups[i])
			{
				if ((name = XGetAtomName(GDK_DISPLAY(), kb->names->groups[i])))
				{
					xkb_info *info;
					
					info = g_new(xkb_info, 1);
					info->name = g_strdup(name);
				/*	info->flag_img = new_pixbuf_flag(name,25,18);
					if(info->flag_img==NULL)
						info->flag_img = new_pixbuf_flag("zz",25,18);*/
			
					sven->sxkb->group2info[i] = info;
					//groups[i].name = name;
					//printf("name:%s\n",name);
				}
				else 
				{
					//printf("XGetAtomName()\n");
					//groups[i].name = NULL; 
				}
			}
			else 
			{
				//groups[i].name = NULL; 
			}
		}

     XkbFreeNames(kb, XkbGroupNamesMask, True);



	if (XkbGetState(GDK_DISPLAY(), XkbUseCoreKbd, &state) == Success) 
	{
		sven->sxkb->cur_group = state.group;        
	}
}

gboolean timer_hide_win(gpointer data)
{
	Sven *sven = (Sven *)data;
	
	//printf("timer_hide_win\n");
	if(sven->sxkb->timeId>0)
		gtk_widget_hide_all (sven->sxkb->flag_win);
  
	return FALSE;
} 

void greate_win_flag(xkb_info *k,Sven *sven)
{
	//int timer;
	int revert_to;
	Window focuswin;
	XWindowAttributes win_attributes;
	Window junkwin;
	int rx, ry, pos_x,pos_y=0;
	
	if(sven->sxkb->view_flag==0)
		return;

	XGetInputFocus(GDK_DISPLAY(), &focuswin, &revert_to);
	if (focuswin) 
	{
		XGetWindowAttributes(GDK_DISPLAY(), focuswin, &win_attributes);

		XTranslateCoordinates (GDK_DISPLAY(), focuswin, win_attributes.root, 
				-win_attributes.border_width,-win_attributes.border_width,
				&rx, &ry, &junkwin);
		
		pos_x=rx;//((win_attributes.width/2)+rx);
		pos_y=ry;//((win_attributes.height/2)+ry);
	}
	else
	{
		pos_x=0;//200;
		pos_y=0;//200;
	}

	if(!sven->sxkb->flag_win)
	{
		sven->sxkb->flag_win = gtk_window_new (GTK_WINDOW_POPUP);
		gtk_widget_set_uposition(GTK_WIDGET(sven->sxkb->flag_win),pos_x,pos_y);
		//gtk_window_set_position (GTK_WINDOW (sven->sxkb->flag_win), GTK_WIN_POS_CENTER);
		
		sven->sxkb->flag_image = new_pixmap_flag(k->name,32,24);
		gtk_widget_show (sven->sxkb->flag_image);
		gtk_container_add (GTK_CONTAINER (sven->sxkb->flag_win), sven->sxkb->flag_image);
		
		gtk_widget_show_all (sven->sxkb->flag_win);
	}
	else
	{
		GdkPixbuf *flag_img;
		flag_img=new_pixbuf_flag(k->name,32,24);
		
		if (flag_img)
			gtk_image_set_from_pixbuf(GTK_IMAGE(sven->sxkb->flag_image), new_pixbuf_flag(k->name,32,24));
		else
			gtk_image_set_from_pixbuf(GTK_IMAGE(sven->sxkb->flag_image), new_pixbuf_flag("zz",32,24));
		gtk_widget_set_uposition(GTK_WIDGET(sven->sxkb->flag_win),pos_x,pos_y);
		gtk_widget_show_all (sven->sxkb->flag_win);
	}
	
	 if (sven->sxkb->timeId != 0)
            g_source_remove(sven->sxkb->timeId);
	    
	 sven->sxkb->timeId = g_timeout_add (800,timer_hide_win, sven);
	
	//timer = g_timeout_add (800,timer_hide_win, sven);
}

void update_flag(xkb_info *k,gpointer data)
{
	Sven *sven = (Sven *)data;
	GdkPixbuf *flag_img;
	
	gtk_tooltips_set_tip (sven->wininfo->flag_tooltip,GTK_WIDGET (sven->wininfo->eventbox),k->name,NULL);
	gtk_tooltips_set_tip (sven->tray_icon_tooltip,GTK_WIDGET (sven->tray_icon),k->name,NULL);
	flag_img=new_pixbuf_flag(k->name,25,20);
	if (flag_img)
	{
		gtk_image_set_from_pixbuf(GTK_IMAGE(sven->tray_flag),flag_img);
		gtk_image_set_from_pixbuf(GTK_IMAGE(sven->wininfo->flag_image),flag_img);
	}
	else
	{
		gtk_image_set_from_pixbuf(GTK_IMAGE(sven->tray_flag),flag_img);
		gtk_image_set_from_pixbuf(GTK_IMAGE(sven->wininfo->flag_image), new_pixbuf_flag("zz",25,20));
	}	
		greate_win_flag(k,sven);
}

static GdkFilterReturn
filter( XEvent *xev, GdkEvent *event, gpointer data)
{
	Sven *sven = (Sven *)data;
	
	if (xev->type ==  sven->sxkb->xkb_event_type) 
	{
		XkbEvent *xkbev = (XkbEvent *) xev;

		if (xkbev->any.xkb_type == XkbNewKeyboardNotify) 
		{        
			printf("New Keyboard\n"); 
			read_kbd_description(sven);
			sven->sxkb->cur_group = sven->sxkb->def_group;
			update_flag(sven->sxkb->group2info[sven->sxkb->cur_group],sven);
			flag_menu_destroy(sven);
			flag_menu_create(sven); 
		}
			return GDK_FILTER_REMOVE;
	}
	
	return (GDK_FILTER_CONTINUE);
}

static void
destroy_all()
{
	gdk_window_remove_filter(NULL, (GdkFilterFunc)filter, NULL);
	XkbSelectEventDetails(GDK_DISPLAY(), XkbUseCoreKbd, XkbStateNotify,
          XkbAllStateComponentsMask, 0UL);
}

static int init(Sven *sven)
{
	int dummy;
	
	if (!XkbQueryExtension(GDK_DISPLAY(), &dummy, &sven->sxkb->xkb_event_type, &dummy, &dummy, &dummy))
		return 0;
	XkbSelectEventDetails(GDK_DISPLAY(), XkbUseCoreKbd, XkbStateNotify,
		XkbAllStateComponentsMask, XkbGroupStateMask);
	gdk_window_add_filter(NULL, (GdkFilterFunc)filter, sven);
	return 1;
}
void save_sxkb_init(gpointer data) 
{
	gchar *filename;
	Sven *sven = (Sven *)data;
	ConfigFile *cfgfile;

	sven->sxkb->def_group=0;
	sven->sxkb->view_flag=1;

	cfgfile = sven_cfg_open_file(g_strconcat(g_get_home_dir(),"/.sven/config", NULL));
	if (!cfgfile)
		cfgfile = sven_cfg_new();
	
	if (cfgfile!= NULL)
	{
		sven_cfg_read_int (cfgfile, "xkb", "def_group", &sven->sxkb->def_group);
		sven_cfg_read_int (cfgfile, "xkb", "view_flag", &sven->sxkb->view_flag);
		sven_cfg_free (cfgfile);
	}
}

void sxkb_init(gpointer data)
{
	Sven *sven = (Sven *)data;
	int dummy;
	
	save_sxkb_init(sven); 
	
	if (!init(sven))
           printf("can't init. exiting\n");
	
	read_kbd_description(sven);
	sven->sxkb->cur_group = sven->sxkb->def_group;
	update_flag(sven->sxkb->group2info[sven->sxkb->cur_group],sven);
	flag_menu_create(sven);
}


