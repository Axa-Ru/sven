 /* Sven multimedia keyboard deamon
 * time.c plugin time
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
#include <time.h>
#include "../../libsven/sven.h"
#include "../../libsven/utils.h"
#include "../../libsven/plugin.h"
#include "../../libsven/xosd.h"


static void create_plugin(Sven *sven,char *function);
static void configure(Sven *sven);
static GSList *add_function_list(void);
static GtkWidget *window;

static SvenPlugin plugin =
{
	0,
	"Time",
	NULL,
	NULL,
	configure,
	create_plugin,
	add_function_list,
};

char* time_read_config(void)
{
	ConfigFile *cfgfile;
	gchar *filename;
	char *format;
	
	filename= g_strconcat(g_get_home_dir(),"/.sven/plugin_config", NULL);
	cfgfile = sven_cfg_open_file(filename);
	if (cfgfile)
	{
		sven_cfg_read_string(cfgfile, "time", "format",&format);
	}
	else
		format="%A %d %H:%M:%S";

	sven_cfg_free(cfgfile);
	g_free(filename);
	
	return format;
}

/* Save current settings to the time config file */
void time_save_config(char *format)
{
	ConfigFile *cfgfile;
	gchar *filename;

	filename= g_strconcat(g_get_home_dir(),"/.sven/plugin_config", NULL);
	cfgfile = sven_cfg_open_file(filename);
	if (!cfgfile)
		cfgfile = sven_cfg_new();
	
	sven_cfg_write_string(cfgfile,"time", "format",format);

	sven_cfg_write_file(cfgfile, filename);
	sven_cfg_free(cfgfile);
	g_free(filename);
	g_print("Save");
}

static void time_conf_ok(GtkWidget * w,GtkWidget *entry)
{
	time_save_config(g_strdup (gtk_entry_get_text (GTK_ENTRY (entry))));
	gtk_widget_destroy(window);
}

void configure(Sven *sven)
{
  GtkWidget *vbox;
  GtkWidget *label;
 static  GtkWidget *entry;
  GtkWidget *hbuttonbox;
 static  GtkWidget *button;
 static  GtkWidget *button1;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), _("Configure time format"));
  gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), &window);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (window), TRUE);
  gtk_window_set_type_hint (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_DIALOG);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  label = gtk_label_new (_("\t\t<b> <span size=\"large\" color=\"#032aa5\">Time output format</span></b>\n\t\t<span size=\"large\" style=\"italic\" color=\"#8732ef\"> Allowed options :\\n </span>\n<b>%a</b> - Abbr. Day of week abbr\n<b>%A</b> - Day of week name\n<b>%b</b> - Month name abbr\n<b>%B</b> - Month nome\n<b>%c</b> - Standart Str date and time\n<b>%d</b> - day of month (01..31)\n<b>%H</b> - hour (00..23)\n<b>%I</b> - hour (01..12)\n<b>%j</b> - day of year (001..366)\n<b>%m</b> - month (01..12)\n<b>%M</b> - minute (00..59)\n<b>%p</b> - locale's upper case AM or PM indicator (blank in many locales)\n<b>%S</b> - second (00..60); the 60 is necessary to accommodate a leap second\n<b>%y</b> - last two digits of year (00..99) \n<b>%Y</b> - year (1970...) \n<b>%Z</b> - time zone (e.g., EDT), or nothing if no time zone is determinable \n"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  GTK_WIDGET_SET_FLAGS (label, GTK_CAN_FOCUS);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_label_set_selectable (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0);

  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  gtk_entry_set_text (GTK_ENTRY (entry),time_read_config());
  gtk_box_pack_start (GTK_BOX (vbox), entry, FALSE, FALSE, 0);

  hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, TRUE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);

  button = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (button);
  g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(time_conf_ok), entry);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), button);

  button1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (button1);
  gtk_signal_connect(GTK_OBJECT(button1), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroyed),&window);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), button1);
  
   gtk_widget_show_all (window);
  
}

static void create_plugin(Sven *sven,char *function)
{
	time_t         *t = NULL;
	char            display[255];
	
	time_t curr_time = time(t);
	
	
	strftime(display, 255,time_read_config(), localtime(&curr_time));

	xosd_display(sven->osd->osd, 0, XOSD_string, display);
	xosd_display(sven->osd->osd, 1, XOSD_string,"");
}

static GSList *add_function_list(void)
{
	GSList *l = NULL;

	l= g_slist_append(l,g_strdup("Show time"));

	return l;
}

SvenPlugin * init_plugin(void)
{
   return &plugin;
}
