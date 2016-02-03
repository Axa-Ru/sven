/* Sven multimedia keyboard deamon
 * sven.c
 *
 *Copyright (C) 2005 Eugene Morenko(More)
 *mailto: morenko@ukr.net
 *web: http://sven.linux.kiev.ua/
 *icq:325697438
 *
 *some part  code take for ACME
 * Bastien Nocera <hadess@hadess.net>
 *
 *
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../libsven/sven.h"
#include "../libsven/key.h"
#include "../libsven/xlib.h"
#include "../libsven/utils.h"
//#include "../libsven/plugin.h"
//#include "../libsven/pref/prefswin.h"

XErrorHandler DefErrHandler;
static int conf_enable=0;


static gchar	*usage_string =
("usage: sven [options]\n"
"options:\n"
"   -v, --version            Print Sven version number and exit.\n"
"   -h, --help               This help.\n"
"   -c, --configure          Configure Sven GUI dialog.\n"
"   -p, --plugin plugin.so   While developing, load your plugin under test.\n"
"   -k, --kill               Kill Sven children and exit.\n\n"
);

static void usage(void)
{
	g_print(_(usage_string));
}

static void get_options (int argc, char **argv,Sven *sven)
{
	int i;
	for (i = 1; i < argc; i++)
    	{
  		if (strcmp (argv[i], "-v") == 0 || strcmp (argv[i], "--version") == 0)
		{
			printf("Sven multimedia keyboard %s \n",VERSION);
			printf("Copyright 2004-2006 by Eugene Morenko(More)\n");
			printf("mailto: morenko@ukr.net\n");
			printf("web: http://sven.linux.kiev.ua/\n");
			printf("icq:325697438\n");
			exit (1);
		}
		else if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0)
		{
			usage ();
			exit (1);
		}
		else if (strcmp (argv[i], "-c") == 0 || strcmp (argv[i], "--configure") == 0)
		{
			printf("Configure programm parameters dialog window \n");
			conf_enable=1;
			if(sven==NULL)
			{
				gtk_main_quit();
				exit (1);
			}
		}
		else if (strcmp (argv[i], "-p") == 0 || strcmp (argv[i], "--plugin") == 0)
		{
			if(sven==NULL)
			{
				exit (1);
			}
			if(sven->plugin==NULL)
				return;
			sven->plugin->arg_plugin_file = g_strdup(argv[++i]);
		}
		else if (strcmp (argv[i], "-k") == 0 || strcmp (argv[i], "--kill") == 0)
		{
			if(sven==NULL)
			{
				exit (1);
			}
			if (sven_get_lock (sven) == FALSE)
			{
				sven_kill_lock (sven);
			}
			exit (1);
		}
		else
		{
			usage();
			exit (1);
		}
	}
}

void Xerror_handler(Display * d, XErrorEvent * ev)
{
	char buf[256];
	
	XGetErrorText(GDK_DISPLAY(), ev->error_code, buf, 256);
	printf( "[Sven] : X error: %s\n", buf);
}

GtkOSD * read_gtk_osd_config(gpointer data)
{
	Sven *sven = (Sven *)data;
	GtkOSD *osd=NULL;

	gchar *font=NULL;
	gchar *color=NULL;
	char *bg_color=NULL;
	int bg_color_on;
	gboolean shadow;
	gboolean frame;
	int timeout;
	int  pos;
	int align;
	int voffset;
	int hoffset;
	int osd_on;

	if(sven->config->all==NULL)
		return NULL;

	sven_cfg_read_string (sven->config->all, "osd", "font",&font);
	sven_cfg_read_string (sven->config->all, "osd", "color",&color);
	sven_cfg_read_string (sven->config->all, "osd", "bg_color",&bg_color);
	sven_cfg_read_int (sven->config->all, "osd", "bg_enable",&bg_color_on);
	sven_cfg_read_int (sven->config->all, "osd", "shadow",&shadow);
	sven_cfg_read_int (sven->config->all, "osd", "frame",&frame);
	sven_cfg_read_int (sven->config->all, "osd", "timeout",&timeout);
	sven_cfg_read_int (sven->config->all, "osd", "pos",&pos);
	sven_cfg_read_int (sven->config->all, "osd", "align",&align);
	sven_cfg_read_int (sven->config->all, "osd", "voffset",&voffset);
	sven_cfg_read_int (sven->config->all, "osd", "hoffset",&hoffset);
	sven_cfg_read_int (sven->config->all, "osd", "osd_on",&osd_on);

	osd=gtk_osd_new_from_data (font,color,bg_color,bg_color_on,shadow,frame,timeout,pos,align,voffset,hoffset,osd_on);

	g_free(font);
	g_free(color);
	g_free(bg_color);

	return osd;
}

int main( int   argc,char *argv[] )
{
	Sven *sven;

#ifdef ENABLE_NLS
	setlocale(LC_ALL,"");
	bindtextdomain(PACKAGE,LOCALEDIR);
	//DEBUG_MSG("set bindtextdomain for %s to %s\n",PACKAGE,LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif
	gtk_init (&argc, &argv);

	DefErrHandler = XSetErrorHandler((XErrorHandler) Xerror_handler);
	
	sven =g_slice_new0 (Sven);
	sven->plugin = g_slice_new0 (SPlugin);

	get_options (argc,argv,sven);
	 
	if (sven_get_lock (sven) == FALSE)
	{
		sven_kill_lock (sven);
	}
	
	/*Start programm function*/
	file_check_directory("/.sven");
	
	sven_read_config(sven);

	init_tray_icon(sven);
	sven->osd=read_gtk_osd_config(sven);

	init_kbd (sven);
	init_mouse (sven);

	sven_init_plugin(sven);

	
	if(conf_enable==1)
		prefswin_dialog_create(sven);
	
	gtk_main ();

	return 0;
}

