/* XOSD

Copyright (c) 2001 Andre Renaud (andre@ignavus.net)

 * configxosd.c
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: more@irpin.com
 * web: http://sven.linux.kiev.ua/
 * icq:325697438
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pango/pango-font.h>
#include <glib-object.h>
#include "sven.h"
#include "utils.h"


static GtkObject *timeout_obj, *offset_obj, *h_offset_obj, *shadow_obj,*outline_obj;
static GtkWidget *timeout_spin, *offset_spin, *h_offset_spin, *shadow_spin,*outline_spin;
static GtkWidget *positions[3][3];
static GtkWidget *colour_entry,*font_entry;

void save_config_xosd (Sven *sven)
{
	if (sven->osd->colour)
		g_free (sven->osd->colour);
	if (sven->osd->font)
		g_free (sven->osd->font);
	
	sven->osd->colour = g_strdup (gtk_entry_get_text (GTK_ENTRY (colour_entry)));
	sven->osd->font = g_strdup (gtk_entry_get_text (GTK_ENTRY (font_entry)));
	sven->osd->timeout = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (timeout_spin));
	sven->osd->offset = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (offset_spin));
	sven->osd->h_offset =gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (h_offset_spin));
	sven->osd->shadow_offset =gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (shadow_spin));
	sven->osd->outline_offset =gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (outline_spin));

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[XOSD_top][XOSD_left])))
	{
		sven->osd->pos = XOSD_top;
		sven->osd->align = XOSD_left;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[XOSD_top][XOSD_center])))
	{
		sven->osd->pos = XOSD_top;
		sven->osd->align = XOSD_center;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[XOSD_top][XOSD_right])))
	{
		sven->osd->pos = XOSD_top;
		sven->osd->align = XOSD_right;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[XOSD_middle][XOSD_left])))
	{
		sven->osd->pos = XOSD_middle;
		sven->osd->align = XOSD_left;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[XOSD_middle][XOSD_center])))
	{
		sven->osd->pos = XOSD_middle;
		sven->osd->align = XOSD_center;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[XOSD_middle][XOSD_right])))
	{
		sven->osd->pos = XOSD_middle;
		sven->osd->align = XOSD_right;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[XOSD_bottom][XOSD_left])))
	{
		sven->osd->pos = XOSD_bottom;
		sven->osd->align = XOSD_left;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[XOSD_bottom][XOSD_center])))
	{
		sven->osd->pos = XOSD_bottom;
		sven->osd->align = XOSD_center;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[XOSD_bottom][XOSD_right])))
	{
		sven->osd->pos = XOSD_bottom;
		sven->osd->align = XOSD_right;
	}

	apply_xosd_config (sven->osd);
	sven_write_config (sven);
}

static int colour_dialog_ok (GtkButton * button, gpointer user_data)
{
	GtkWidget *colour_dialog = user_data;
	char tmp_colour[8];
	double colour[4];

	DEBUG_MSG ("colour_dialog_ok");
	assert (GTK_IS_COLOR_SELECTION_DIALOG (colour_dialog));
	
	gtk_color_selection_get_color
	(GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG
				(colour_dialog)->colorsel), colour);
	//printf("COLOR: %f\n %f\n %f\n",colour[0]* 0xffff,colour[1]* 0xffff,colour[2]* 0xffff);
	
	sprintf (tmp_colour, "#%2.2x%2.2x%2.2x",
		(int) (colour[0] * 255),
		(int) (colour[1] * 255), (int) (colour[2] * 255));
	
	gtk_entry_set_text (GTK_ENTRY (colour_entry), tmp_colour);
	
	gtk_widget_destroy (colour_dialog);
	
	return 0;
}

/*
 * Create dialog window for colour selection.
 */
int colour_dialog_window (GtkButton * button,Sven *sven)
{
	GtkWidget *colour_dialog;
	GtkWidget *cancel_button, *ok_button, *colour_widget;
	gdouble colour[4];
	int red, green, blue;
		
	DEBUG_MSG ("colour_dialog_window");
	colour_dialog = gtk_color_selection_dialog_new ("XOSD Colour");
	
	assert (colour_dialog);
	
	colour_widget = GTK_COLOR_SELECTION_DIALOG (colour_dialog)->colorsel;
	gtk_color_selection_set_has_opacity_control (GTK_COLOR_SELECTION (colour_widget), FALSE);
	gtk_color_selection_set_has_palette (GTK_COLOR_SELECTION (colour_widget), TRUE);
	if (sven->osd->osd)
	{
		xosd_get_colour (sven->osd->osd, &red, &green, &blue);
		
		colour[0] = (float) red / (float) USHRT_MAX;
		colour[1] = (float) green / (float) USHRT_MAX;
		colour[2] = (float) blue / (float) USHRT_MAX;
	
		gtk_color_selection_set_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(colour_dialog)->colorsel),colour);
	}
    
	ok_button = GTK_COLOR_SELECTION_DIALOG (colour_dialog)->ok_button;
	cancel_button = GTK_COLOR_SELECTION_DIALOG (colour_dialog)->cancel_button;

	gtk_signal_connect_object (GTK_OBJECT (cancel_button), "clicked",
				GTK_SIGNAL_FUNC (gtk_widget_destroy),
				GTK_OBJECT (colour_dialog));
	
	gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
			GTK_SIGNAL_FUNC (colour_dialog_ok), colour_dialog);
	
	gtk_widget_show_all (colour_dialog);
	return 0;
}

GtkWidget **position_icons_new (void)
{
	GtkWidget **icons = NULL;
	int i = 0;
	int j = 0;
	int count = 2;


	DEBUG_MSG ("creating icons...\n");
	// Create the array to hold the icons
	icons = calloc (9, sizeof (GtkWidget *));
	if (icons == NULL)
	{
		DEBUG_MSG ("Cound not create icons...\n");
		perror ("Could not create \"icons\"");
		exit (20432);
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			// g_print("new_pixmap:%d\n",count);
			// Add the widget to the array of pixmaps
			icons[(3 * i) + j] = new_pixmap_mod(count,0,0);
			count++;
		}
	}
	return icons;
}

/*
 * Create dialog window for colour selection.
 */
int font_dialog_window (GtkButton * button,Sven *sven)
{
	 gchar *output = NULL;
	
		g_spawn_command_line_sync("sven-font",&output,NULL,NULL,NULL);
		//printf("OUTPUT:%s\n",output);
		if(g_strcasecmp(output,"")!=0)
			gtk_entry_set_text (GTK_ENTRY (font_entry), output);

	return 0;
}

void create_xosd_gui(Sven *sven,GtkWidget *vbox)
{
	GtkWidget  *hbox, *label,*unit_label;
	GtkWidget *table, **position_icons, *position_table,*sep,*button,*osd_page_label;

	xosd_pos curr_pos;
	xosd_align curr_align;
	
	GSList *group = NULL;

	sven_read_config (sven);

	
	osd_page_label = gtk_label_new (g_strdup_printf("<span size=\"xx-large\"><b><u><i>%s\n</i></u></b></span>",_("XOSD configure")));
	gtk_widget_show (osd_page_label);
	gtk_box_pack_start (GTK_BOX (vbox), osd_page_label, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (osd_page_label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (osd_page_label), 0, 0);

	table = gtk_table_new (7, 3, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (table), 12);
	gtk_table_set_col_spacings (GTK_TABLE (table), 12);
	gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);
	
	// Font selector. 
	label = gtk_label_new (_("Font:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
			GTK_FILL, GTK_FILL, 0, 0);
	 font_entry = gtk_entry_new ();
	if (sven->osd->font)
	gtk_entry_set_text (GTK_ENTRY (font_entry), sven->osd->font);
	gtk_table_attach (GTK_TABLE (table), font_entry, 1, 2, 0, 1,
			GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	
	button = gtk_button_new_from_stock ("gtk-select-font");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (font_dialog_window), sven);
	gtk_table_attach (GTK_TABLE (table), button, 2, 3, 0, 1,
			GTK_FILL, GTK_FILL, 0, 0);
	
	//Colour Selector
	label = gtk_label_new (_("Colour:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
			GTK_FILL, GTK_FILL, 0, 0);
	colour_entry = gtk_entry_new ();
	if (sven->osd->colour)
	gtk_entry_set_text (GTK_ENTRY (colour_entry), sven->osd->colour);
	gtk_table_attach (GTK_TABLE (table), colour_entry, 1, 2, 1, 2,
			GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	button = gtk_button_new_from_stock ("gtk-select-color");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (colour_dialog_window), sven);
	gtk_table_attach (GTK_TABLE (table), button, 2, 3, 1, 2,
			GTK_FILL, GTK_FILL, 0, 0);
	
	//Timeout
	label = gtk_label_new (_("Timeout:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3,
			GTK_FILL, GTK_FILL, 0, 0);
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 2, 3,
			GTK_FILL, GTK_FILL, 0, 0);
	timeout_obj = gtk_adjustment_new (sven->osd->timeout, -1, 60, 1, 1, 1);
	timeout_spin = gtk_spin_button_new (GTK_ADJUSTMENT (timeout_obj), 1.0, 0);
	if (sven->osd->timeout)
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (timeout_spin),
				(gfloat) sven->osd->timeout);
	gtk_box_pack_start (GTK_BOX (hbox), timeout_spin, FALSE, FALSE, 0);
	unit_label = gtk_label_new (_("seconds"));
	gtk_misc_set_alignment (GTK_MISC (unit_label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (unit_label), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start (GTK_BOX (hbox), unit_label, FALSE, FALSE, 0);
	
	//Shadow Offset
	label = gtk_label_new (_("Shadow Offset:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4,
			GTK_FILL, GTK_FILL, 0, 0);
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 3, 4,
			GTK_FILL, GTK_FILL, 0, 0);
	shadow_obj = gtk_adjustment_new (sven->osd->timeout, 0, 60, 1, 1, 1);
	shadow_spin = gtk_spin_button_new (GTK_ADJUSTMENT (shadow_obj), 1.0, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (shadow_spin),
				(gfloat) sven->osd->shadow_offset);
	gtk_box_pack_start (GTK_BOX (hbox), shadow_spin, FALSE, FALSE, 0);
	unit_label = gtk_label_new (_("pixels"));
	gtk_misc_set_alignment (GTK_MISC (unit_label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (unit_label), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start (GTK_BOX (hbox), unit_label, FALSE, FALSE, 0);
	
	//Outline Offset
	label = gtk_label_new (_("Outline Offset:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 4, 5,
			GTK_FILL, GTK_FILL, 0, 0);
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 4, 5,
			GTK_FILL, GTK_FILL, 0, 0);
	outline_obj = gtk_adjustment_new (sven->osd->timeout, 0, 60, 1, 1, 1);
	outline_spin = gtk_spin_button_new (GTK_ADJUSTMENT (outline_obj), 1.0, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (outline_spin),
				(gfloat) sven->osd->outline_offset);
	gtk_box_pack_start (GTK_BOX (hbox), outline_spin, FALSE, FALSE, 0);
	unit_label = gtk_label_new (_("pixels"));
	gtk_misc_set_alignment (GTK_MISC (unit_label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (unit_label), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start (GTK_BOX (hbox), unit_label, FALSE, FALSE, 0);
	
	// Position 
	label = gtk_label_new (_("Position:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 5, 6,GTK_FILL, GTK_FILL, 0, 0);
	
	position_icons = position_icons_new ();
	position_table = gtk_table_new (3, 3, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (position_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (position_table), 6);
	gtk_table_attach (GTK_TABLE (table), position_table, 1, 2, 5, 6,GTK_FILL, GTK_FILL, 0, 0);
	
	curr_pos = XOSD_top;
	for (curr_align = XOSD_left; curr_align <= XOSD_right; curr_align++)
	{
		positions[curr_pos][curr_align] = gtk_radio_button_new (group);
		gtk_container_add (GTK_CONTAINER (positions[curr_pos][curr_align]),position_icons[(curr_pos * 3) + curr_align]);
		assert (positions[curr_pos][curr_align] != NULL);
		
		gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]), FALSE);
		group =gtk_radio_button_group (GTK_RADIO_BUTTON(positions[curr_pos][curr_align]));
	
		if (sven->osd->pos == curr_pos && sven->osd->align == curr_align)
			{
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]),TRUE);
			}
	
		gtk_table_attach (GTK_TABLE (position_table),positions[curr_pos][curr_align],curr_align, curr_align + 1,curr_pos, curr_pos + 1, GTK_FILL, GTK_FILL, 0, 0);
	}
	
	curr_pos = XOSD_middle;
	for (curr_align = XOSD_left; curr_align <= XOSD_right; curr_align++)
	{
		positions[curr_pos][curr_align] = gtk_radio_button_new (group);
		gtk_container_add (GTK_CONTAINER (positions[curr_pos][curr_align]),position_icons[(curr_pos * 3) + curr_align]);
		assert (positions[curr_pos][curr_align] != NULL);
	
		gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]), FALSE);
		group =gtk_radio_button_group (GTK_RADIO_BUTTON(positions[curr_pos][curr_align]));
	
		if (sven->osd->pos == curr_pos && sven->osd->align == curr_align)
		{
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]),TRUE);
		}
	
		gtk_table_attach (GTK_TABLE (position_table),positions[curr_pos][curr_align],curr_align, curr_align + 1,1, 2, GTK_FILL, GTK_FILL, 0, 0);
	}
	
	curr_pos = XOSD_bottom;
	for (curr_align = XOSD_left; curr_align <= XOSD_right; curr_align++)
	{
		positions[curr_pos][curr_align] = gtk_radio_button_new (group);
		gtk_container_add (GTK_CONTAINER (positions[curr_pos][curr_align]),position_icons[(curr_pos * 3) + curr_align]);
	
		assert (positions[curr_pos][curr_align] != NULL);
	
		gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]), FALSE);
		group =gtk_radio_button_group (GTK_RADIO_BUTTON(positions[curr_pos][curr_align]));
	
		if (sven->osd->pos == curr_pos && sven->osd->align == curr_align)
		{
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]),TRUE);
		}
	
		gtk_table_attach (GTK_TABLE (position_table),positions[curr_pos][curr_align],curr_align, curr_align + 1,2, 3, GTK_FILL, GTK_FILL, 0, 0);
	}
	
	// Vertical Offset
	label = gtk_label_new (_("Vertical Offset:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 6, 7,GTK_FILL, GTK_FILL, 0, 0);
	
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 6, 7,GTK_FILL, GTK_FILL, 0, 0);
	
	offset_obj = gtk_adjustment_new (sven->osd->timeout, 0, 300, 1, 1, 1);
	offset_spin = gtk_spin_button_new (GTK_ADJUSTMENT (offset_obj), 1.0, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (offset_spin), sven->osd->offset);
	gtk_box_pack_start (GTK_BOX (hbox), offset_spin, FALSE, FALSE, 0);
	unit_label = gtk_label_new (_("pixels"));
	gtk_misc_set_alignment (GTK_MISC (unit_label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (unit_label), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start (GTK_BOX (hbox), unit_label, FALSE, FALSE, 0);
	
	// Horizontal Offset
	label = gtk_label_new (_("Horizontal Offset:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 7, 8,GTK_FILL, GTK_FILL, 0, 0);
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 7, 8,GTK_FILL, GTK_FILL, 0, 0);
	
	h_offset_obj = gtk_adjustment_new (sven->osd->timeout, 0, 300, 1, 1, 1);
	h_offset_spin = gtk_spin_button_new (GTK_ADJUSTMENT (h_offset_obj), 1.0, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (h_offset_spin), sven->osd->h_offset);
	gtk_box_pack_start (GTK_BOX (hbox), h_offset_spin, FALSE, FALSE, 0);
	unit_label = gtk_label_new (_("pixels"));
	gtk_misc_set_alignment (GTK_MISC (unit_label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (unit_label), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start (GTK_BOX (hbox), unit_label, FALSE, FALSE, 0);
	
	sep = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (vbox), sep, FALSE, FALSE, 0);
	
	gtk_widget_show_all (vbox);
}

