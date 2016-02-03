 /* Sven multimedia keyboard deamon
 * cdrom.c module for control CD player
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

#include "cdpl.h"


#include "../../libsven/sven.h"
#include "../../libsven/plugin.h"

static void command(gpointer sdata,gpointer data,char *function,char *title);
static GSList *get_function_list(gpointer data);


static SvenPlugin plugin =
{
	NULL,	/*about*/
	NULL, 	/*configure*/
	NULL,	/*create*/
	command,/*command*/
	NULL, /*send_event*/
	NULL, /*send_event_key*/
	NULL, 	/*destroy*/
	get_function_list,	/*get_command_name*/
	NULL, /*get menu widget*/
	NULL,	/*priv_data*/
};


static void command(gpointer sdata,gpointer data,char *function,char *title)
{
	gchar **mixer = g_strsplit(function,"-",2);

	if(g_strcasecmp (mixer[0],"CD")==0)
	{
		select_cd(sdata,mixer[1],title);
	}
	g_strfreev(mixer);
}


static GSList *get_function_list(gpointer data)
{
	GSList *l = NULL;

	l= g_slist_append(l,g_strdup("CD-Play"));
	l= g_slist_append(l,g_strdup("CD-Pause"));
	l= g_slist_append(l,g_strdup("CD-Stop"));
	l= g_slist_append(l,g_strdup("CD-Prev"));
	l= g_slist_append(l,g_strdup("CD-Next"));
	l= g_slist_append(l,g_strdup("CD-Eject"));
	

	return l;
}

SvenPlugin * init_plugin(void)
{
   return &plugin;
}

char * get_plugin_name(void)
{
   return "control CD player";
}

