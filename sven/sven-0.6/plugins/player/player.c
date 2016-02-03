 /* Sven multimedia keyboard deamon
 * player.c module for control XMMS/BMP player
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
module for control XMMS/BMP/Audacious player \n\
\n\
Copyright (C) 2005 Eugene Morenko(More)\n\
mailto: morenko@ukr.net\n\
web: http://sven.linux.kiev.ua/\n\
icq:325697438\n");
}

static GSList *get_function_list(gpointer data)
{
	GSList *l = NULL;

	l= g_slist_append(l,g_strdup("Player-XMMS Play"));
	l= g_slist_append(l,g_strdup("Player-XMMS Play|Pause"));
	l= g_slist_append(l,g_strdup("Player-XMMS Pause"));
	l= g_slist_append(l,g_strdup("Player-XMMS Stop"));
	l= g_slist_append(l,g_strdup("Player-XMMS Prev"));
	l= g_slist_append(l,g_strdup("Player-XMMS Next"));
	l= g_slist_append(l,g_strdup("Player-BMP Play"));
	l= g_slist_append(l,g_strdup("Player-BMP Play|Pause"));
	l= g_slist_append(l,g_strdup("Player-BMP Pause"));
	l= g_slist_append(l,g_strdup("Player-BMP Stop"));
	l= g_slist_append(l,g_strdup("Player-BMP Prev"));
	l= g_slist_append(l,g_strdup("Player-BMP Next"));
	l= g_slist_append(l,g_strdup("Player-Audacious Play"));
	l= g_slist_append(l,g_strdup("Player-Audacious Play|Pause"));
	l= g_slist_append(l,g_strdup("Player-Audacious Pause"));
	l= g_slist_append(l,g_strdup("Player-Audacious Stop"));
	l= g_slist_append(l,g_strdup("Player-Audacious Prev"));
	l= g_slist_append(l,g_strdup("Player-Audacious Next"));

	return l;
}

static void command(gpointer sdata,gpointer data,char *function,char *title)
{
	Sven *sven = (Sven *)sdata;
	gchar **player = g_strsplit(function,"-",2);
	
	if((player[0]==NULL)&&(player[1]==NULL))
		return;
	
	if(g_strcasecmp (player[0],"Player")!=0)
		return;
	
	if(g_strcasecmp (player[1],"XMMS Play")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("xmms -p", FALSE);
	}
	else if(g_strcasecmp (player[1],"XMMS Play|Pause")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("xmms -t", FALSE);
	}
	else if(g_strcasecmp (player[1],"XMMS Pause")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("xmms -u", FALSE);
	}
	else if(g_strcasecmp (player[1],"XMMS Stop")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("xmms -s", FALSE);
	}
	else if(g_strcasecmp (player[1],"XMMS Prev")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("xmms -r", FALSE);
	}
	else if(g_strcasecmp (player[1],"XMMS Next")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("xmms -f", FALSE);
	}
	else if(g_strcasecmp (player[1],"BMP Play")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("beep-media-player -p", FALSE);
	}
	else if(g_strcasecmp (player[1],"BMP Play|Pause")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("beep-media-player -t", FALSE);
	}
	else if(g_strcasecmp (player[1],"BMP Pause")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("beep-media-player -u", FALSE);
	}
	else if(g_strcasecmp (player[1],"BMP Stop")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("beep-media-player -s", FALSE);
	}
	else if(g_strcasecmp (player[1],"BMP Prev")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("beep-media-player -r", FALSE);
	}
	else if(g_strcasecmp (player[1],"BMP Next")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,player[1],-1);
		execute ("beep-media-player -f", FALSE);
	} else if (g_strcasecmp(player[1], "Audacious Play") == 0) {
		gtk_osd_print(sven->osd, GTK_OSD_TEXT, player[1], -1);
		execute("audacious -p", FALSE);
	} else if (g_strcasecmp(player[1], "Audacious Play|Pause") == 0) {
		gtk_osd_print(sven->osd, GTK_OSD_TEXT, player[1], -1);
		execute("audacious -t", FALSE);
	} else if (g_strcasecmp(player[1], "Audacious Pause") == 0) {
		gtk_osd_print(sven->osd, GTK_OSD_TEXT, player[1], -1);
		execute("audacious -u", FALSE);
	} else if (g_strcasecmp(player[1], "Audacious Stop") == 0) {
		gtk_osd_print(sven->osd, GTK_OSD_TEXT, player[1], -1);
		execute("audacious -s", FALSE);
	} else if (g_strcasecmp(player[1], "Audacious Prev") == 0) {
		gtk_osd_print(sven->osd, GTK_OSD_TEXT, player[1], -1);
		execute("audacious -r", FALSE);
	} else if (g_strcasecmp(player[1], "Audacious Next") == 0) {
		gtk_osd_print(sven->osd, GTK_OSD_TEXT, player[1], -1);
		execute("audacious -f", FALSE);
	}

	g_strfreev(player);
}

SvenPlugin * init_plugin(void)
{
   return &plugin;
}

char * get_plugin_name(void)
{
   return "control XMMS/BMP player";
}

char * get_plugin_info(void)
{
   return "<b>Control XMMS/BMP player</b>";
}

