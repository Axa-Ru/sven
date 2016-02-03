 /* Sven multimedia keyboard deamon
 * www.c module for control Firefox browser
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

#include "../../libsven/sven.h"
#include "../../libsven/plugin.h"
#include "../../libsven/utils.h"

static void command(gpointer sdata,gpointer data,char *function,char *title);
static void about(gpointer data);
static GSList *get_function_list(gpointer data);

static SvenPlugin plugin =
{
	about,	/*about*/
	NULL, 	/*configure*/
	NULL,	/*create*/
	command,	/*command*/
	NULL, /*send_event*/
	NULL, /*send_event_key*/
	NULL, 	/*destroy*/
	get_function_list,	/*get_command_name*/
	NULL, /*get menu widget*/
	NULL,	/*priv_data*/
};



static void about(gpointer data)
{
	show_info("\
<b>	 Sven multimedia keyboard deamon</b>\n\
module for control Firefox browser\n\
\n\
Copyright (C) 2005 Eugene Morenko(More)\n\
mailto: morenko@ukr.net\n\
web: http://sven.linux.kiev.ua/\n\
icq:325697438\n");
}

static GSList *get_function_list(gpointer data)
{
	GSList *l = NULL;

	l= g_slist_append(l,g_strdup("Browser-Firefox Back"));
	l= g_slist_append(l,g_strdup("Browser-Firefox Forward"));
	l= g_slist_append(l,g_strdup("Browser-Firefox Reload"));
	l= g_slist_append(l,g_strdup("Browser-Firefox Stop"));
	

	return l;
}

static void command(gpointer sdata,gpointer data,char *function,char *title)
{
	Sven *sven = (Sven *)sdata;
	gchar **www = g_strsplit(function,"-",2);
	
	if((www[0]==NULL)&&(www[1]==NULL))
		return;
	
	if(g_strcasecmp (www[0],"Browser")!=0)
		return;
	
	if(g_strcasecmp (www[1],"Firefox Back")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,www[1],-1);
		execute ("firefox --remote \"openURL(javascript: history.back())\"", FALSE);
	}
	else if(g_strcasecmp (www[1],"Firefox Forward")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,www[1],-1);
		execute ("firefox --remote \"openURL(javascript: history.forward())\"", FALSE);
	}
	else if(g_strcasecmp (www[1],"Firefox Reload")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,www[1],-1);
		execute ("firefox --remote \"openURL(javascript: history.reload())\"", FALSE);
	}
	else if(g_strcasecmp (www[1],"Firefox Stop")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,www[1],-1);
		execute ("firefox --remote \"openURL(javascript: history.stop())\"", FALSE);
	}

	g_strfreev(www);
}

G_MODULE_EXPORT SvenPlugin * init_plugin(void)
{
   return &plugin;
}

G_MODULE_EXPORT char * get_plugin_name(void)
{
   return "Firefox browser";
}

char * get_plugin_info(void)
{
   return "<b>Control Firefox browser</b>";
}
