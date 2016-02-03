 /* Sven multimedia keyboard deamon
 * trayicon.c 
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
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sven.h"
#include "pixmap.h"
#include "trayicon.h"
#include "utils.h"
#include "plugin.h"

#include "pref/prefswin.h"

static void prefswin_dialog (GtkMenuItem   *item, gpointer data)
{
	prefswin_dialog_create(data);
}

static void popup_menu (GtkStatusIcon *icon, guint    button,guint32   activate_time,gpointer data)
{
	Sven *sven = (Sven *)data;
	GtkWidget *menu, *menuitem,*submenu;
	GSList *l = NULL;

	menu = gtk_menu_new ();

	for (l = sven->plugin->list; l; l=l->next)
	{
		SvenPluginPriv *plug = (SvenPluginPriv *)l->data;
		if((plug!=NULL)&&(plug->func!=NULL))
		{
			if ((plug->func->get_menu!=NULL)&&(plug->enable==1))
			{
				submenu=(GtkWidget*)plug->func->get_menu(sven,plug->func->private_data);
//				printf("Add menu;%s\n",plug->name);
				gtk_container_add (GTK_CONTAINER (menu),submenu);
			}
//			else
//				printf("Not Add menu;%s\n",plug->name);
		}
	}

	menuitem = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
	gtk_widget_show (menuitem);
	gtk_widget_set_sensitive (menuitem, FALSE);

	menuitem = gtk_image_menu_item_new_from_stock ("gtk-preferences", NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
	gtk_widget_show (menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(prefswin_dialog),data);

	menuitem = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
	gtk_widget_show (menuitem);
	gtk_widget_set_sensitive (menuitem, FALSE);
	
	menuitem = gtk_image_menu_item_new_from_stock ("gtk-quit", NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
	gtk_widget_show (menuitem);
	g_signal_connect ((gpointer) menuitem, "activate",G_CALLBACK (sven_exit),data);

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, button, activate_time);
}

void init_tray_icon(gpointer data)
{
	Sven *sven = (Sven *)data;

	GdkPixbuf *pixbuf=NULL;
	
	pixbuf=create_pixbuf_type (17,20,20);
	sven->tray_icon = gtk_status_icon_new_from_pixbuf (pixbuf);
	g_object_unref(pixbuf);
	gtk_status_icon_set_tooltip (sven->tray_icon, _("Sven Multimedia Keyboard daemon"));

	g_signal_connect (sven->tray_icon, "popup-menu", G_CALLBACK (popup_menu), sven);
 
}

