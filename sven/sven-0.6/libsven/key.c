/* Sven multimedia keyboard deamon
 * key.c
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
#include "key.h"
#include "utils.h"
#include "xlib.h"
#include "grabkeyboard.h"


void glist_grab_ungrab_key (Sven *sven,GList* list,int i)
{
	if(sven == NULL)
		return;
		
	GList *p = g_list_first (list);
	
	while (p)
	{
		int modifer=0;
		
		ConfigSection *section = (ConfigSection *) p->data;

		gchar **key_modifer = g_strsplit(section->name,":",2);
		if(key_modifer[1]==NULL)
			modifer=0;
		else
			modifer=atoi(key_modifer[1]);
		if(i==0)
		{
			gint y=0;
			sven_cfg_read_int(sven->config->key,section->name, "enable",&y);
			if(y)
			{
				grab_key (atoi(key_modifer[0]),modifer);
			}
		}
		else
			ungrab_key(atoi(key_modifer[0]));

		g_strfreev(key_modifer);
			
		p = g_list_next (p);
	}
}


void ungrab_key (int key_code)
{
	GdkWindow *root=gdk_get_default_root_window();
	
	gdk_error_trap_push ();
	XUngrabKey (GDK_DISPLAY (), key_code, AnyModifier,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())));
	XUngrabKey (GDK_DISPLAY (), AnyKey, AnyModifier, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())));


	gdk_flush ();
	if (gdk_error_trap_pop ()) 
	{
		gchar *error;
		gchar *key;

		key=g_strdup_printf ("%s",(XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), key_code, 0)) != NULL)? 
		XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),key_code, 0)):
		g_strdup_printf ("%d",key_code));
		
		error = g_strdup_printf
			(_("It seems that another application already has"
			   " access to the multimedia keys.\n"
			   "Key %s couldn't be bound.\n"
			   "Is another daemon already running ?\n"),
			 key);
		show_error(error);
		printf("[Sven][ERROR]:%s",error);
		g_free (key);
		g_free (error);
	}
}

void update_kbd (Sven *sven)
{
	if(sven == NULL)
		return;
	
	glist_grab_ungrab_key(sven,sven->config->key->sections,1);
}

void grab_key (int key_code,int state)
{
	GdkWindow *root=gdk_get_default_root_window();
	gdk_error_trap_push ();

	if(state>0)
	{
		XGrabKey (GDK_DISPLAY (), key_code,
			state,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			DellModifier(state),
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			AddCapsModifier(state),
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			AddNumModifier(state),
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			AddCapsNumModifier(state),
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
	}
	else
	{
		XGrabKey (GDK_DISPLAY (), key_code,
			0,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod2Mask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod5Mask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			LockMask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod2Mask | LockMask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod5Mask | LockMask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod2Mask | Mod5Mask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod2Mask | Mod5Mask | LockMask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
	}

	gdk_flush ();
	if (gdk_error_trap_pop ())
	{
		gchar *error;
		gchar *str;
		gchar *key;

		str = g_new (gchar,513);
		 
		ModifierToString (state, str);
		key=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
		(XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), key_code, 0)) != NULL)? 
		XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),key_code, 0)):
		g_strdup_printf ("%d",key_code));
		
		error = g_strdup_printf
			(_("It seems that another application already has"
			   " access to the multimedia keys.\n"
			   "Key %s couldn't be bound.\n"
			   "Is another daemon already running ?\n"),
			 key);
		show_error(error);
		printf("[Sven][ERROR]:%s",error);
		g_free (key);
		g_free (str);
		g_free (error);
	}
}


void init_kbd (Sven *sven)
{
	if(sven == NULL)
		return;
		
	if(sven->config->key!=NULL)
		glist_grab_ungrab_key(sven,sven->config->key->sections,0);
	else
	{
		char *error;
		error = g_strdup_printf
		(_("Sorry file key configuration not found.\nPlease greate new configuration keyboard.\n"));
		show_error (error);
		g_free (error);
	}
	
	gdk_window_add_filter (gdk_get_default_root_window(),window_event_filter,(gpointer) sven);
}
