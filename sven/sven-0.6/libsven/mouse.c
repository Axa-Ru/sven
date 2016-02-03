/* Sven multimedia keyboard deamon
 * mouse.c
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: morenko@ukr.net
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
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "sven.h"
#include "utils.h"
#include "xlib.h"
#include "mouse.h"

void glist_grab_ungrab_mouse (Sven *sven,GList* list,int i)
{
	if(sven == NULL)
		return;
	gchar **mouse_modifer=NULL;

	GList *p = g_list_first (list);
	
	while (p)
	{
		int modifer=0;
		
		ConfigSection *section = (ConfigSection *) p->data;

		mouse_modifer = g_strsplit(section->name,":",2);
		if(mouse_modifer[1]==NULL)
			modifer=0;
		else
			modifer=atoi(mouse_modifer[1]);
		if(i==0)
		{
			gint y=0;
			sven_cfg_read_int(sven->config->mouse,section->name, "enable",&y);
			if(y)
				grab_mouse (atoi(mouse_modifer[0]),modifer);
		}
		else
			ungrab_mouse(atoi(mouse_modifer[0]));

		g_strfreev(mouse_modifer);

		p = g_list_next (p);
	}
}


void ungrab_mouse (int key_code)
{
	GdkWindow *root=gdk_get_default_root_window();
	gdk_error_trap_push ();
	XUngrabButton (GDK_DISPLAY (), key_code, AnyModifier,(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())));
	XUngrabButton (GDK_DISPLAY (), AnyButton, AnyModifier, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())));


	gdk_flush ();
	if (gdk_error_trap_pop ()) 
	{
		char *error;

		error = g_strdup_printf
			(_("There was an error removing access to the "
			   "mouse buttons.\n Buttons %d couldn't be unbound."),
			 key_code);
		show_error (error);
		printf("[Sven][ERROR]:%s",error);
		g_free (error);
	}
}

void update_mouse (Sven *sven)
{
	if(sven == NULL)
		return;
		
	if(sven->config->mouse!=NULL)
		glist_grab_ungrab_mouse(sven,sven->config->mouse->sections,1);
}


void grab_mouse (int key_code,int state)
{
	GdkWindow *root=gdk_get_default_root_window();
	gdk_error_trap_push ();

	if(state>0)
	{
		XGrabButton(GDK_DISPLAY (),key_code,state, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
		XGrabButton(GDK_DISPLAY (),key_code,DellModifier(state),(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
		XGrabButton(GDK_DISPLAY (),key_code,AddCapsModifier(state),(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
	
		XGrabButton(GDK_DISPLAY (),key_code,AddNumModifier(state),(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
		XGrabButton(GDK_DISPLAY (),key_code,AddCapsNumModifier(state),(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
	}
	else
	{
		XGrabButton(GDK_DISPLAY (),key_code,AnyModifier,(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
	}

	gdk_flush ();
	if (gdk_error_trap_pop ())
	{
		char *error;
		char str[1000];
		char *but="";

		ModifierToStringMouse (state, str);
		but=g_strdup_printf ("%s%s%d",str,str[0] ? "+" : "", key_code);
		
		error = g_strdup_printf
			(_("It seems that another application already has"
			   " access to the you mouse.\n"
			   "Buttons combination %s couldn't be bound.\n"
			   "Is another daemon already running ?\n"),
			 but);
		show_error(error);
		printf("[Sven][ERROR]:%s",error);
		g_free (error);
	}
}

void init_mouse (Sven *sven)
{
	if(sven == NULL)
		return;
		
	if(sven->config->mouse!=NULL)
		glist_grab_ungrab_mouse(sven,sven->config->mouse->sections,0);
	else
	{
		char *error;
		error = g_strdup_printf
		(_("Sorry file mouse configuration not found.\nPlease greate new mouse configuration.\n"));
		show_error (error);
		g_free (error);
	}
}
