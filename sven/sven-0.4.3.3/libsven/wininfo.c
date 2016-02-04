/* Sven multimedia keyboard deamon
 * wininfo.c
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
#include <X11/Xlib.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "sven.h"
#include "wininfo.h"
#include "utils.h"
#include "sxkb.h"

int dragx,dragy;


gboolean win_flag_press(GtkWidget *widget, GdkEventButton *event,Sven *sven)
{
	if (event->type != GDK_BUTTON_PRESS)
		return;

	if (event->button == 1) 
	{
		int no;
	
		no =  (sven->sxkb->cur_group + 1) % sven->sxkb->ngroups;
		XkbLockGroup(GDK_DISPLAY(), XkbUseCoreKbd, no);
	}  
	else if (event->button == 3) 
	{
		gtk_menu_popup(GTK_MENU(sven->sxkb->flag_menu), NULL, NULL, NULL, NULL, event->button, event->time);
	}
	
	return TRUE;
}

gboolean win_time_press(GtkWidget *widget, GdkEventButton *geb,Sven *sven)
{
	
	if (geb->button == 1 && geb->type == GDK_2BUTTON_PRESS)
	{	/* double click  */
		if(sven->wininfo->timer_stat==0)
		{
			sven->wininfo->timer_h=sven->wininfo->timer_m=sven->wininfo->timer_s=0;
			sven->wininfo->timer_stat=1;
			return 1;
		}
		else if(sven->wininfo->timer_stat==1)
		{
			sven->wininfo->timer_h=sven->wininfo->timer_m=sven->wininfo->timer_s=0;
			sven->wininfo->timer_stat=0;
			return 1;
		}
		else if(sven->wininfo->timer_stat==2)
		{
			sven->wininfo->timer_h=sven->wininfo->timer_m=sven->wininfo->timer_s=0;
			sven->wininfo->timer_stat=0;
			return 1;
		}
		else
		{
			sven->wininfo->timer_h=sven->wininfo->timer_m=sven->wininfo->timer_s=0;
			sven->wininfo->timer_stat=1;
		}
		return 1;
	}
	else if (geb->button == 1 && sven->wininfo->timer_stat>0)
	{	/* double click  */
		
		if(sven->wininfo->timer_stat==2)
		{
			sven->wininfo->timer_stat=0;
			return 1;
		}
		sven->wininfo->timer_stat=2;
		return 1;
	}
	else
		icon_press_all (widget,geb,sven);
	
	return TRUE;
}

gboolean win_press(GtkWidget *w,GdkEventButton *event,Sven *sven)
{
	if (event==NULL)
		return FALSE;

	if (event->button == 1) 
	{
		dragx = (int) event->x;
		dragy = (int) event->y;
	}  
	if (event->button == 1 && event->type == GDK_2BUTTON_PRESS) 
	{
		prefswin_dialog_create (sven);
	}  
	else 
		icon_press_all (w,event,sven);
	
	return TRUE;
}

gboolean win_motion(GtkWidget *w,GdkEventMotion *geb,gpointer data)
{
	gint xp, yp;
	GdkModifierType mask;
	
	if (geb==NULL)
		return TRUE;

	gdk_window_get_pointer (NULL, &xp, &yp, &mask);
	gtk_widget_set_uposition (w, xp - dragx, yp - dragy);
	
	return FALSE;
}

gchar* locale_from_utf8 (gchar *s)
{
  if (! s)
     return NULL; 
  gsize bytes_written;
  return g_locale_from_utf8 (s, strlen (s), NULL, &bytes_written, NULL);
} 

gchar* locale_to_utf8 (gchar *s)
{
	if (! s)
		return NULL; 

 	 gsize bytes_written;
	return g_locale_to_utf8 (s, strlen (s), NULL, &bytes_written, NULL);
} 

gboolean show_time(gpointer data)
{
	Wininfo *wininfo = (Wininfo *)data;
	
	if (wininfo->timer_stat==0)
	{
		time_t         *t = NULL;
		char   display[255];
		
		wininfo->timer_h=wininfo->timer_m=wininfo->timer_s=0;
	
		time_t curr_time = time(t);

		strftime(display, 255,wininfo->time_format, localtime(&curr_time));
		gtk_label_set_markup(GTK_LABEL(wininfo->time),g_strdup_printf("<span  color=\"blue\" font_desc=\"10\">%s</span>",locale_to_utf8(display)));
	}
	else if(wininfo->timer_stat==1)
	{
		wininfo->timer_s++;
		if(wininfo->timer_s==60)
		{
			wininfo->timer_m++;
			wininfo->timer_s=0;
		}
		if(wininfo->timer_m==60)
		{
			wininfo->timer_h++;
			wininfo->timer_m=0;
		}	
		gtk_label_set_markup(GTK_LABEL(wininfo->time),g_strdup_printf("<span  color=\"red\" font_desc=\"10\">%0.2d:%0.2d:%0.2d</span>",wininfo->timer_h,wininfo->timer_m,wininfo->timer_s));

	}
	else if(wininfo->timer_stat==2)
	{
		return TRUE;
	}
	else
	{
		time_t         *t = NULL;
		char   display[255];
		
		wininfo->timer_h=wininfo->timer_m=wininfo->timer_s=0;
	
		time_t curr_time = time(t);

		strftime(display, 255,wininfo->time_format, localtime(&curr_time));
		gtk_label_set_markup(GTK_LABEL(wininfo->time),g_strdup_printf("<span font_desc=\"10\"  color=\"blue\">%s</span>",locale_to_utf8(display)));
	}
	
	return TRUE;
} 

Wininfo *create_wininfo (gpointer data)
{
	Sven *sven = (Sven *)data;
	GtkWidget *vbox,*hbox;
	GtkWidget *eventbox,*eventbox1,*eventbox2;
	GtkWidget *label;
	GdkColor color;
	Wininfo *wininfo;

	wininfo = g_new(Wininfo, 1);
	
	wininfo->char_count=wininfo->words_count=wininfo->chars_edits_count=wininfo->chars_time_count=wininfo->chars_min_count=wininfo->words_min_count=wininfo->edits_count=0;
	
	wininfo->info_win = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_widget_set_size_request (wininfo->info_win, 150, 85);
	gtk_widget_set_app_paintable (wininfo->info_win, TRUE);
	gtk_window_set_resizable (GTK_WINDOW (wininfo->info_win), TRUE);
	gtk_widget_set_uposition(GTK_WIDGET(wininfo->info_win),sven->mix->x_icon,sven->mix->y_icon);
	gtk_widget_set_name (wininfo->info_win, "Sven-window info ");
	gtk_container_set_border_width (GTK_CONTAINER (wininfo->info_win), 2);

 	gtk_signal_connect(GTK_OBJECT(wininfo->info_win),"motion_notify_event",GTK_SIGNAL_FUNC(win_motion),NULL);
  
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (wininfo->info_win), vbox);

	eventbox = gtk_event_box_new ();
	gdk_color_parse ("#F3FFFF",&color);
	gtk_widget_modify_bg (eventbox, GTK_STATE_NORMAL,&color);
	gtk_widget_show (eventbox);
	gtk_box_pack_start (GTK_BOX (vbox), eventbox, TRUE, TRUE, 0);
	gtk_signal_connect(GTK_OBJECT(eventbox),"button_press_event",GTK_SIGNAL_FUNC(win_press),sven);

	wininfo->table = gtk_table_new (3, 2, TRUE);
	gtk_widget_show (wininfo->table);
	gtk_container_add (GTK_CONTAINER (eventbox), wininfo->table);
	gtk_container_set_border_width (GTK_CONTAINER (wininfo->table), 8);
	
	label = gtk_label_new (_("   Characters:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (wininfo->table), label, 0, 1, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

	label = gtk_label_new (_("Words:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (wininfo->table), label, 0, 1, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

	label = gtk_label_new (_("Edits:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (wininfo->table), label, 0, 1, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.51);

	/*label = gtk_label_new (_("Time:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (wininfo->table), label, 0, 1, 3, 4,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

	label = gtk_label_new (_("Chars/min:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (wininfo->table), label, 0, 1, 4, 5,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

	label = gtk_label_new (_("Words/min:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (wininfo->table), label, 0, 1, 5, 6,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

	label = gtk_label_new (_("% of edits:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (wininfo->table), label, 0, 1, 6, 7,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);*/
	
	
	wininfo->characters = gtk_label_new (_("0"));
	gtk_widget_show (wininfo->characters);
	gtk_table_attach (GTK_TABLE (wininfo->table), wininfo->characters, 1, 2, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);

	wininfo->words = gtk_label_new (_("0"));
	gtk_widget_show (wininfo->words);
	gtk_table_attach (GTK_TABLE (wininfo->table), wininfo->words, 1, 2, 1, 2,
			(GtkAttachOptions) (0),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (wininfo->words), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment (GTK_MISC (wininfo->words), 1, 0.5);

	wininfo->chars_edits = gtk_label_new (_("0"));
	gtk_widget_show (wininfo->chars_edits);
	gtk_table_attach (GTK_TABLE (wininfo->table), wininfo->chars_edits, 1, 2, 2, 3,
			(GtkAttachOptions) (0),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (wininfo->chars_edits), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment (GTK_MISC (wininfo->chars_edits), 1, 0.5);

	/*wininfo->chars_time = gtk_label_new (_("0"));
	gtk_widget_show (wininfo->chars_time);
	gtk_table_attach (GTK_TABLE (wininfo->table), wininfo->chars_time, 1, 2, 3, 4,
			(GtkAttachOptions) (0),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (wininfo->chars_time), 1, 0.5);

	wininfo->chars_min = gtk_label_new (_("0"));
	gtk_widget_show (wininfo->chars_min);
	gtk_table_attach (GTK_TABLE (wininfo->table), wininfo->chars_min, 1, 2,4, 5,
			(GtkAttachOptions) (0),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (wininfo->chars_min), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (wininfo->chars_min), 1, 0.5);
	
	wininfo->words_min = gtk_label_new (_("0"));
	gtk_widget_show (wininfo->words_min);
	gtk_table_attach (GTK_TABLE (wininfo->table), wininfo->words_min, 1, 2, 5, 6,
			(GtkAttachOptions) (0),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (wininfo->words_min), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (wininfo->words_min), 1, 0.5);
	
	wininfo->edits = gtk_label_new (_("0"));
	gtk_widget_show (wininfo->edits);
	gtk_table_attach (GTK_TABLE (wininfo->table), wininfo->edits, 1, 2, 6, 7,
			(GtkAttachOptions) (0),
			(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify (GTK_LABEL (wininfo->edits), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (wininfo->edits), 1, 0.5);*/
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

	wininfo->eventbox = gtk_event_box_new ();
	g_signal_connect(G_OBJECT(wininfo->eventbox), "button_press_event", G_CALLBACK(win_flag_press),sven);
	wininfo->flag_tooltip = gtk_tooltips_new ();
	gtk_tooltips_set_tip (wininfo->flag_tooltip,GTK_WIDGET (wininfo->eventbox),"Not flag",NULL);
	gtk_widget_show (wininfo->eventbox);
	gtk_box_pack_start (GTK_BOX (hbox), wininfo->eventbox, FALSE, FALSE, 0);
  

	wininfo->flag_image = new_pixmap_flag("zz",25,20);

	if (wininfo->flag_image)
	{
		gtk_widget_show (wininfo->flag_image);
		gtk_container_add (GTK_CONTAINER (wininfo->eventbox), wininfo->flag_image);
	}

	eventbox2 = gtk_event_box_new ();
	g_signal_connect ((gpointer) eventbox2, "button_press_event",G_CALLBACK (win_time_press),sven);
	gdk_color_parse ("#FFFFEB",&color);
	gtk_widget_modify_bg (eventbox2, GTK_STATE_NORMAL,&color);
	gtk_widget_show (eventbox2);
	gtk_box_pack_start (GTK_BOX (hbox), eventbox2, TRUE, TRUE, 0);
  
	wininfo->time = gtk_label_new (_("Time"));
	gtk_widget_show (wininfo->time);
	wininfo->time_format="%b %d, %a - %H:%M:%S  ";
	wininfo->timer_h=wininfo->timer_m=wininfo->timer_s=0;
	wininfo->timer_stat=0;
	gtk_container_add (GTK_CONTAINER (eventbox2), wininfo->time);

	gtk_label_set_justify (GTK_LABEL (wininfo->time), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC (wininfo->time), 1, 0.5);
	
	wininfo->timer = g_timeout_add (1000,show_time, wininfo);
	
	//if(sven->mix->icon_check_desk==1)
		gtk_widget_show_all (wininfo->info_win);

	 return wininfo;
}



