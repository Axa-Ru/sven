/* Sven multimedia keyboard deamon
 * sven.c
 *
 *Copyright (C) 2005 Eugene Morenko(More)
 *mailto: more@irpin.com
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
#include "../libsven/mouse.h"
#include "../libsven/utils.h"
#include "../libsven/prefswin.h"
#include "../libsven/plugin.h"
#include "../libsven/mixer.h"
#include "../libsven/cdpl.h"
#include "../libsven/clipman.h"
#include "../libsven/grabkeyboard.h"
#include "../libsven/wininfo.h"
#include "../libsven/load_symbol.h"

static void show_help (void)
{
	printf(BOLD RED"Sven multimedia keyboard %s \n",VERSION);
	printf (LIME"usage: sven [options]"NORMAL"\n");
	printf (BOLD "  where options are:\n");
	printf (NORMAL);
	printf (RED"  -v, --version           Print version and exit"NORMAL"\n");
	printf (BLUE"  -h, --help              This help!"NORMAL"\n");
	printf (FIOLETE"  -a, --about              About for Sven"NORMAL"\n");
	printf (LIME"  -c, --configure        Configure Sven"NORMAL"\n");
	printf (RED"  -i, --icon        Set icon Sven"NORMAL"\n");
}
static void get_options (int argc, char **argv,Sven *sven)
{
	int i;
	for (i = 1; i < argc; i++)
    	{
  		if (strcmp (argv[i], "-v") == 0 || strcmp (argv[i], "--version") == 0)
		{
			printf(BOLD RED"Sven multimedia keyboard %s \n",VERSION);
			printf(BLUE"Copyright 2005 by Eugene Morenko(More)\n");
			printf(GREEN"mailto: more@irpin.com\n");
			printf(LIME"web: http://sven.linux.kiev.ua/\n");
			printf(YELLOW"icq:325697438\n"NORMAL);
				exit (1);
	}
		else if (strcmp (argv[i], "-a") == 0 || strcmp (argv[i], "--about") == 0)
		{
			printf(BOLD RED"About dialog window \n"NORMAL);
			printf(BOLD RED"Sven multimedia keyboard %s \n",VERSION);
			printf(BLUE"Copyright 2005 by Eugene Morenko(More)\n");
			printf(GREEN"mailto: more@irpin.com\n");
			printf(LIME"web: http://sven.linux.kiev.ua/\n");
			printf(YELLOW"icq:325697438\n"NORMAL);
			
			if(sven==NULL)
			{
				gtk_main_quit();
				exit (1);
			}
		
			prefswin_dialog_create (sven);
			change_category_num (8);
		}
		else if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0)
		{
			printf(BOLD RED"Show help text"NORMAL"\n");
			show_help ();
			exit (1);
		}
		else if (strcmp (argv[i], "-c") == 0 || strcmp (argv[i], "--configure") == 0)
		{
			printf(BOLD RED"Configure programm parameters dialog window"NORMAL"\n");
			if(sven==NULL)
			{
				gtk_main_quit();
				exit (1);
			}
		
			prefswin_dialog_create (sven);
		}
		else if (strcmp (argv[i], "-i") == 0 || strcmp (argv[i], "--icon") == 0)
		{
			printf(BOLD RED"Set icon Sven"NORMAL"\n");
			if(sven==NULL)
			{
				gtk_main_quit();
				exit (1);
			}
		
			sven->mix->icon_check=1;
		}
		else
		{
			printf(BOLD RED"Show help text"NORMAL"\n");
			show_help();
				exit (1);
		}
	}
}

gboolean progress_volume(gpointer data)
{
	Sven *sven = (Sven *)data;
	if(sven->vol_timer==get_volume(sven,0))
		return TRUE;

		sven->vol_timer=get_volume(sven,0);
		xosd_display (sven->osd->osd,0, XOSD_string,g_strdup_printf ("%s:%d",_("Volume"),sven->vol_timer));
		xosd_display (sven->osd->osd,1, XOSD_percentage,sven->vol_timer);
  
	return TRUE;
} 

gboolean progress_key(gpointer data)
{
	Sven *sven = (Sven *)data;
	
	get_key_win(sven);
  
	return TRUE;
} 

int main( int   argc,char *argv[] )
{
	Sven *sven;
	GList *l;
	int timer;
	int clock=0;

#ifdef ENABLE_NLS
	setlocale(LC_ALL,"");
	bindtextdomain(PACKAGE,LOCALEDIR);
	//DEBUG_MSG("set bindtextdomain for %s to %s\n",PACKAGE,LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif

	gtk_set_locale ();
	gtk_init (&argc, &argv);
	
	 sven = g_new0 (Sven, 1);
	 
	if (sven_get_lock (sven) == FALSE)
	{
		g_print(RED); 
		g_print (_("Daemon already running, exiting...\n"));
		g_print(NORMAL);
		show_error(_("Daemon already running, exiting..."));
		exit (1);
	}
	
	/*Start*/
	file_check_directory();
	
	sven->osd= g_malloc0(sizeof (MXosd));
	
	sven_read_config(sven);
	if(sven->mix->enable_xosd==1)
	{
		sven->osd=greate_osd(sven->osd,1);
		apply_xosd_config(sven->osd);
	}
	
	init_kbd (sven);
	init_mouse (sven);
	
	if(sven->mix->enable_plugin==1)
	{
		sven_plugin_start(sven);
	}
	
	clipman_new(sven);
	
	sven->wininfo= g_malloc0(sizeof (Wininfo));
	sven->sxkb= g_malloc0(sizeof (Sxkb));
	sven->wininfo=create_wininfo(sven);
	init_tray (sven);
	sxkb_init(sven);
	grab_keyboard_init (argc, argv,sven);
	get_options (argc,argv,sven);
	init_icon (sven);
	
	sven->screen->current_screen = gdk_screen_get_default ();

   	/* Start filtering the events */
	for (l = sven->screen->screens; l != NULL; l = l->next)
	{
		GdkScreen *screen;
		GdkWindow *window;

		screen = (GdkScreen *) l->data;
		window = gdk_screen_get_root_window (screen);
		gdk_window_add_filter (window,
				sven_filter_events,
				(gpointer) sven);
	}

	
	load_startup_prog(sven);
	sven->vol_timer=get_volume(sven,0);
	timer = g_timeout_add (150,progress_volume, sven);
	
	//while(1)
	//{
	//	get_key_win(sven);
		/*while (gtk_events_pending ())
		{
			gtk_main_iteration ();
		}*/

	//}
	g_timeout_add (10,progress_key, sven);
	//get_key_win(sven);
	gtk_main ();
	
	printf("Sorry\n");
	return 0;
}

