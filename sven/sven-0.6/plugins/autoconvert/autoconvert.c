 /* Sven multimedia keyboard deamon
 * autoconvert.c plugin for automatic convert text for russian and english layout
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
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pcre.h>

#include "autoconvert.h"
#include "convert.h"
#include "sxkb.h"
#include "configure.h"
#include "../../libsven/sven.h"
#include "../../libsven/pixmap.h"
#include "../../libsven/plugin.h"
#include "../../libsven/utils.h"
#include "../../libsven/grabkeyboard.h"

static gpointer create(gpointer sdata);
static void destroy(gpointer sven,gpointer data);
static void command(gpointer sdata,gpointer data,char *function,char *title);
static gpointer get_menu(gpointer sdata,gpointer data);
static GSList *get_function_list(gpointer data);

static SvenPlugin plugin =
{
	NULL,	/*about*/
	configure, 	/*configure*/
	create,	/*create*/
	command,	/*command*/
	NULL, /*send_event*/
	NULL, /*send_event_key*/
	destroy, 	/*destroy*/
	get_function_list,	/*get_command_name*/
	get_menu, /*get menu widget*/
	NULL,	/*priv_data*/
};

static gpointer create(gpointer sdata)
{
	Sven *sven = (Sven *)sdata;
	AConvert *aconv= g_slice_new0(AConvert);
	//aconv = g_new0 (AConvert, 1);
	
	aconv->sven=sven;
	init_sxkb(sven,aconv);
	dictionary_init(aconv);
	grab_keyboard_init(aconv);
	
	 return aconv;
}

static void  destroy(gpointer sven,gpointer data)
{
	grab_keyboard_destroy (data);
	sxkb_destroy();
	flag_menu_destroy(data);
}

static gpointer get_menu(gpointer sdata,gpointer data)
{
	 return flag_menu_create(data);
}

static GSList *get_function_list(gpointer data)
{
	GSList *l = NULL;

	l= g_slist_append(l,g_strdup("AutoConvert-Undo|Forced switch"));
	l= g_slist_append(l,g_strdup("AutoConvert-Invert case"));

	return l;
}

static void command(gpointer sdata,gpointer data,char *function,char *title)
{
	gchar **atc = g_strsplit(function,"-",2);
	
	if((atc[0]==NULL)&&(atc[1]==NULL))
		return;
	
	if(g_strcasecmp (atc[0],"AutoConvert")!=0)
		return;
	
	if(g_strcasecmp (atc[1],"Undo|Forced switch")==0)
	{
		xConvert_text (data);
	}
	else if(g_strcasecmp (atc[1],"Invert case")==0)
	{
		xConvert_text_case (data);
	}

	g_strfreev(atc);
}

SvenPlugin * init_plugin(void)
{
   return &plugin;
}

char * get_plugin_name(void)
{
   return "Auto Convert text";
}

char * get_plugin_info(void)
{
   return "<b>Automatic convert text for russian and english layout</b>";
}

