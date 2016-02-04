/* Sven multimedia keyboard deamon
 * about.c
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: more@irpin.com
 * web: http://sven.linux.kiev.ua/
 * icq:325697438
 *
 * some part  code and idea take for xfce4
 * 
 *some part  code take for ACME,XMMS,XOSD,XFCE4.
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
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sven.h"
#include "about.h"
#define BORDER 6


static void add_page (GtkNotebook * notebook, const gchar * name, const gchar * buf,gboolean hscrolling)
{
	GtkTextBuffer *textbuffer;
	GtkWidget *textview;
	GtkWidget *label;
	GtkWidget *view;
	GtkWidget *sw;

	label = gtk_label_new (name);
	gtk_widget_show (label);

	view = gtk_frame_new (NULL);
	gtk_container_set_border_width (GTK_CONTAINER (view), BORDER);
	gtk_frame_set_shadow_type (GTK_FRAME (view), GTK_SHADOW_IN);
	gtk_widget_show (view);

	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
					hscrolling ? GTK_POLICY_AUTOMATIC :
					GTK_POLICY_NEVER,
					GTK_POLICY_AUTOMATIC);
	gtk_widget_show (sw);
	gtk_container_add (GTK_CONTAINER (view), sw);
	
	textbuffer = gtk_text_buffer_new (NULL);
	gtk_text_buffer_set_text (textbuffer, buf, strlen(buf));
	
	textview = gtk_text_view_new_with_buffer (textbuffer);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
	gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), BORDER);
	gtk_text_view_set_right_margin (GTK_TEXT_VIEW (textview), BORDER);

	gtk_widget_show (textview);
	gtk_container_add (GTK_CONTAINER (sw), textview);

	gtk_notebook_append_page (notebook, view, label);
}

void create_about_gui(Sven *sven,GtkWidget *vbox1)
{ 
	GtkWidget *vbox,*key_page_label;
	GtkWidget *notebook;

	

	DEBUG_MSG("Start create_about_gui\n");
	
	key_page_label = gtk_label_new (g_strdup_printf("<span size=\"xx-large\"><b><u><i>%s\n</i></u></b></span>",_("Info")));
	gtk_widget_show (key_page_label);
	gtk_box_pack_start (GTK_BOX (vbox1), key_page_label, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (key_page_label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (key_page_label), 0, 0);

	vbox =gtk_vbox_new (FALSE, BORDER);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), BORDER);
	gtk_widget_show (vbox);
	gtk_box_pack_start (GTK_BOX (vbox1), vbox, TRUE, TRUE, 0);
	
  
 
	/* the notebook */
	notebook = gtk_notebook_new ();
	gtk_widget_show (notebook);
	gtk_widget_set_size_request (notebook, -1, 300);
	gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);

	/* add pages */
	add_page (GTK_NOTEBOOK (notebook), _("Info"), INFO, FALSE);
	add_page (GTK_NOTEBOOK (notebook), _("Authors"), AUTHORS, FALSE);
	add_page (GTK_NOTEBOOK (notebook), _("Thanks To"), THANKS, TRUE);
	add_page (GTK_NOTEBOOK (notebook), _("Translators"), TRANSLATORS, TRUE);

	gtk_widget_show_all(vbox);
}
