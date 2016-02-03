/* XOSD

Copyright (c) 2001 Andre Renaud (andre@ignavus.net)

 * configxosd.c
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: morenko@ukr.net
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

#include "../sven.h"
#include "../utils.h"
#include "../pixmap.h"


static GtkObject *timeout_obj, *offset_obj, *h_offset_obj, *shadow_obj,*outline_obj;
static GtkWidget *timeout_spin, *offset_spin, *h_offset_spin, *shadow_spin,*outline_spin;
static GtkWidget *positions[3][3];
static GtkWidget *colour_entry,*font_entry,*bg_color_enable,*bg_color,*enable_xosd,*enable_frame,*enable_shadow;
GtkOSD *osd=NULL;

void destroy_test_osd (void)
{
	gtk_osd_destroy(osd);
}
static void test_osd (GtkWidget *wid,Sven *sven)
{
	GdkColor       g_color;
	GdkColor       g_bg_color;
	gchar *filename;

	gint drop_shadow;
	gint debug_frame;

	int voffset;
	int hoffset;
	gboolean osd_on;
	gchar  *font;
	gchar  *color;
	gchar  *bgcolor;
	int bg_color_on ;
	int timeout;
	Gtk_Osd_Pos pos;
	Gtk_Osd_Align align;

	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_TOP][GTK_OSD_LEFT])))
	{
		pos = GTK_OSD_TOP;
		align = GTK_OSD_LEFT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_TOP][GTK_OSD_CENTER])))
	{
		pos = GTK_OSD_TOP;
		align = GTK_OSD_CENTER;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_TOP][GTK_OSD_RIGHT])))
	{
		pos = GTK_OSD_TOP;
		align = GTK_OSD_RIGHT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_MIDDLE][GTK_OSD_LEFT])))
	{
		pos = GTK_OSD_MIDDLE;
		align = GTK_OSD_LEFT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_MIDDLE][GTK_OSD_CENTER])))
	{
		pos = GTK_OSD_MIDDLE;
		align = GTK_OSD_CENTER;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_MIDDLE][GTK_OSD_RIGHT])))
	{
		pos = GTK_OSD_MIDDLE;
		align = GTK_OSD_RIGHT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_BOTTOM][GTK_OSD_LEFT])))
	{
		pos = GTK_OSD_BOTTOM;
		align = GTK_OSD_LEFT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_BOTTOM][GTK_OSD_CENTER])))
	{
		pos = GTK_OSD_BOTTOM;
		align = GTK_OSD_CENTER;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_BOTTOM][GTK_OSD_RIGHT])))
	{
		pos = GTK_OSD_BOTTOM;
		align = GTK_OSD_RIGHT;
	}
	
	timeout = (gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (timeout_spin)));
	
	voffset = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (offset_spin));
	hoffset =gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (h_offset_spin));
	bg_color_on=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (bg_color_enable));
	
	gtk_color_button_get_color (GTK_COLOR_BUTTON(bg_color),&g_bg_color);
	bgcolor=gtk_color_selection_palette_to_string(&g_bg_color,1);	

	gtk_color_button_get_color (GTK_COLOR_BUTTON(colour_entry),&g_color);
	color=gtk_color_selection_palette_to_string (&g_color,1);
	
	font=(gchar *)gtk_font_button_get_font_name(GTK_FONT_BUTTON(font_entry));
	debug_frame=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (enable_frame));
	drop_shadow=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (enable_shadow));
	
	if(!osd)
		osd=gtk_osd_new_from_data (font,color,bgcolor,bg_color_on,drop_shadow,debug_frame,timeout,pos,align,voffset,hoffset,1);
	else
	{
		gtk_osd_set_font(osd,font);
		gtk_osd_set_color (osd,color);
		gtk_osd_set_bg_color (osd,bgcolor);
		gtk_osd_set_bg_color_on (osd,bg_color_on);
		gtk_osd_set_drop_shadow(osd,drop_shadow);
		gtk_osd_set_debug_frame(osd,debug_frame);
		gtk_osd_set_timeout (osd,timeout);
		gtk_osd_set_vertical_offset (osd,voffset);
		gtk_osd_set_horizontal_offset (osd,hoffset);
		gtk_osd_set_pos (osd,pos);
		gtk_osd_set_align (osd,align);
	}
	gtk_osd_print (osd,GTK_OSD_TEXT,"OSD TEST",GTK_OSD_TEXT,":",GTK_OSD_INT,50,GTK_OSD_PERCENTAGE,50,-1);

	//g_free(font);
	//g_free(color);
//	g_free(bg_color);
}

void save_config_xosd (Sven *sven)
{
	GdkColor       g_color;
	GdkColor       g_bg_color;
	gchar *filename;

	gint drop_shadow;
	gint debug_frame;

	int voffset;
	int hoffset;
	gboolean osd_on;
	gchar  *font;
	gchar  *color;
	gchar  *bgcolor;
	int bg_color_on ;
	int timeout;
	Gtk_Osd_Pos pos;
	Gtk_Osd_Align align;

	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_TOP][GTK_OSD_LEFT])))
	{
		pos = GTK_OSD_TOP;
		align = GTK_OSD_LEFT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_TOP][GTK_OSD_CENTER])))
	{
		pos = GTK_OSD_TOP;
		align = GTK_OSD_CENTER;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_TOP][GTK_OSD_RIGHT])))
	{
		pos = GTK_OSD_TOP;
		align = GTK_OSD_RIGHT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_MIDDLE][GTK_OSD_LEFT])))
	{
		pos = GTK_OSD_MIDDLE;
		align = GTK_OSD_LEFT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_MIDDLE][GTK_OSD_CENTER])))
	{
		pos = GTK_OSD_MIDDLE;
		align = GTK_OSD_CENTER;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_MIDDLE][GTK_OSD_RIGHT])))
	{
		pos = GTK_OSD_MIDDLE;
		align = GTK_OSD_RIGHT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_BOTTOM][GTK_OSD_LEFT])))
	{
		pos = GTK_OSD_BOTTOM;
		align = GTK_OSD_LEFT;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_BOTTOM][GTK_OSD_CENTER])))
	{
		pos = GTK_OSD_BOTTOM;
		align = GTK_OSD_CENTER;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (positions[GTK_OSD_BOTTOM][GTK_OSD_RIGHT])))
	{
		pos = GTK_OSD_BOTTOM;
		align = GTK_OSD_RIGHT;
	}
	
	
	timeout = (gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (timeout_spin)));
	
	voffset = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (offset_spin));
	hoffset =gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (h_offset_spin));
	bg_color_on=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (bg_color_enable));
	
	gtk_color_button_get_color (GTK_COLOR_BUTTON(bg_color),&g_bg_color);
	bgcolor=gtk_color_selection_palette_to_string(&g_bg_color,1);	

	gtk_color_button_get_color (GTK_COLOR_BUTTON(colour_entry),&g_color);
	color=gtk_color_selection_palette_to_string (&g_color,1);
	
	font=(char *)gtk_font_button_get_font_name(GTK_FONT_BUTTON(font_entry));
	debug_frame=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (enable_frame));
	drop_shadow=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (enable_shadow));
	
	osd_on=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (enable_xosd));
	
	/* Write all sven config [file:config] */
	filename= g_strconcat(g_get_home_dir(),"/.sven/config", NULL);
	
	sven_cfg_write_string (sven->config->all, "osd", "font",font);
	sven_cfg_write_string (sven->config->all, "osd", "color",color);
	sven_cfg_write_int (sven->config->all, "osd", "bg_enable",bg_color_on);
	sven_cfg_write_string (sven->config->all, "osd", "bg_color",bgcolor);
	sven_cfg_write_int (sven->config->all, "osd", "frame",debug_frame);
	sven_cfg_write_int (sven->config->all, "osd", "shadow",drop_shadow);
	sven_cfg_write_int (sven->config->all, "osd", "voffset",voffset);
	sven_cfg_write_int (sven->config->all, "osd", "hoffset",hoffset);
	sven_cfg_write_int (sven->config->all, "osd", "timeout",timeout);
	sven_cfg_write_int (sven->config->all, "osd", "pos",pos);
	sven_cfg_write_int (sven->config->all, "osd", "align",align);
	sven_cfg_write_int (sven->config->all, "osd", "osd_on",osd_on);
	sven_cfg_write_file(sven->config->all, filename);
	g_free(filename);

	if(sven->osd->bg_color_on!=bg_color_on)
	{
		gtk_osd_destroy(sven->osd);
		sven->osd=gtk_osd_new_from_data (font,color,bgcolor,bg_color_on,drop_shadow,debug_frame,timeout,pos,align,voffset,hoffset,osd_on);
	}
	else
	{
		gtk_osd_set_font(sven->osd,font);
		gtk_osd_set_color (sven->osd,color);
		gtk_osd_set_bg_color (sven->osd,bgcolor);
		gtk_osd_set_drop_shadow(sven->osd,drop_shadow);
		gtk_osd_set_debug_frame(sven->osd,debug_frame);
		gtk_osd_set_timeout (sven->osd,timeout);
		gtk_osd_set_vertical_offset (sven->osd,voffset);
		gtk_osd_set_horizontal_offset (sven->osd,hoffset);
		gtk_osd_set_pos (sven->osd,pos);
		gtk_osd_set_align (sven->osd,align);
		gtk_osd_set_osd_on (sven->osd,osd_on);
	}

	//g_free(font);
	//g_free(color);
	//g_free(bg_color);
}

GtkWidget **position_icons_new (void)
{
	GtkWidget **icons = NULL;
	int i = 0;
	int j = 0;
	int count = 7;


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
			icons[(3 * i) + j] = create_pixmap_type(count,15,15);
			count++;
		}
	}
	return icons;
}

void create_xosd_gui(Sven *sven,GtkWidget *vbox)
{
	GtkWidget  *hbox,*hbox1, *label,*unit_label,*button1;
	GtkWidget *table, **position_icons, *position_table,*sep,*button;
	GdkColor   *colors;
	gint n_colors;

	Gtk_Osd_Pos curr_pos;
	Gtk_Osd_Align curr_align;
	
	GSList *group = NULL;

	gtk_box_pack_start (GTK_BOX (vbox),(gpointer )create_header(_("XOSD")), FALSE, FALSE, 0);

	table = gtk_table_new (7,2, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (table), 12);
	gtk_table_set_col_spacings (GTK_TABLE (table), 12);
	gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);
	
	// Font selector. 
	label = gtk_label_new (_("Font:"));
	gtk_misc_set_alignment (GTK_MISC (label), 1.0,0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
			GTK_FILL, GTK_FILL, 0, 0);
	
	if(sven->osd->font!=NULL)
		 font_entry = gtk_font_button_new_with_font (sven->osd->font);
	 else 
	  	font_entry = gtk_font_button_new_with_font ("Sans 18");
	gtk_table_attach (GTK_TABLE (table), font_entry, 1, 2, 0, 1,
			GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	
	//Colour Selector
	label = gtk_label_new (_("Colour:"));
	gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
			GTK_FILL, GTK_FILL, 0, 0);
			
	gtk_color_selection_palette_from_string (sven->osd->color,&colors,&n_colors);
	colour_entry = gtk_color_button_new_with_color (colors);
	gtk_table_attach (GTK_TABLE (table), colour_entry, 1, 2, 1, 2,
			GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	
	//Timeout
	label = gtk_label_new (_("Timeout:"));
	gtk_misc_set_alignment (GTK_MISC (label), 1.0,0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3,
			GTK_FILL, GTK_FILL, 0, 0);
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 2, 3,
			GTK_FILL, GTK_FILL, 0, 0);
	timeout_obj = gtk_adjustment_new (sven->osd->timeout, -1, 60, 1, 1, 1);
	timeout_spin = gtk_spin_button_new (GTK_ADJUSTMENT (timeout_obj), 1.0, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (timeout_spin),
				(gfloat) sven->osd->timeout);
	gtk_box_pack_start (GTK_BOX (hbox), timeout_spin, FALSE, FALSE, 0);
	unit_label = gtk_label_new (_("seconds"));
	gtk_misc_set_alignment (GTK_MISC (unit_label),0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (unit_label), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start (GTK_BOX (hbox), unit_label, FALSE, FALSE, 0);

	// Position 
	label = gtk_label_new (_("Position:"));
	gtk_misc_set_alignment (GTK_MISC (label), 1.0,0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3,4,GTK_FILL, GTK_FILL, 0, 0);
	
	position_icons = position_icons_new ();
	position_table = gtk_table_new (3, 3, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (position_table), 6);
	gtk_table_set_col_spacings (GTK_TABLE (position_table), 6);
	gtk_table_attach (GTK_TABLE (table), position_table, 1, 2, 3,4,GTK_FILL, GTK_FILL, 0, 0);
	
	curr_pos = GTK_OSD_TOP;
	for (curr_align = GTK_OSD_LEFT; curr_align <= GTK_OSD_RIGHT; curr_align++)
	{
		positions[curr_pos][curr_align] = gtk_radio_button_new (group);
		gtk_container_add (GTK_CONTAINER (positions[curr_pos][curr_align]),position_icons[(curr_pos * 3) + curr_align]);
		
		gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]), FALSE);
		group =gtk_radio_button_group (GTK_RADIO_BUTTON(positions[curr_pos][curr_align]));
	
		if (sven->osd->pos == curr_pos && sven->osd->align == curr_align)
		{
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]),TRUE);
		}
	
		gtk_table_attach (GTK_TABLE (position_table),positions[curr_pos][curr_align],curr_align, curr_align + 1,curr_pos, curr_pos + 1, GTK_FILL, GTK_FILL, 0, 0);
	}
	
	curr_pos = GTK_OSD_MIDDLE;
	for (curr_align = GTK_OSD_LEFT; curr_align <= GTK_OSD_RIGHT; curr_align++)
	{
		positions[curr_pos][curr_align] = gtk_radio_button_new (group);
		gtk_container_add (GTK_CONTAINER (positions[curr_pos][curr_align]),position_icons[(curr_pos * 3) + curr_align]);
	
		gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]), FALSE);
		group =gtk_radio_button_group (GTK_RADIO_BUTTON(positions[curr_pos][curr_align]));
	
		if (sven->osd->pos == curr_pos && sven->osd->align == curr_align)
		{
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(positions[curr_pos][curr_align]),TRUE);
		}
	
		gtk_table_attach (GTK_TABLE (position_table),positions[curr_pos][curr_align],curr_align, curr_align + 1,1, 2, GTK_FILL, GTK_FILL, 0, 0);
	}
	
	curr_pos = GTK_OSD_BOTTOM;
	for (curr_align = GTK_OSD_LEFT; curr_align <= GTK_OSD_RIGHT; curr_align++)
	{
		positions[curr_pos][curr_align] = gtk_radio_button_new (group);
		gtk_container_add (GTK_CONTAINER (positions[curr_pos][curr_align]),position_icons[(curr_pos * 3) + curr_align]);
	
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
	gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 4,5,GTK_FILL, GTK_FILL, 0, 0);
	
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 4, 5,GTK_FILL, GTK_FILL, 0, 0);
	
	offset_obj = gtk_adjustment_new (sven->osd->timeout, 0, 300, 1, 1, 1);
	offset_spin = gtk_spin_button_new (GTK_ADJUSTMENT (offset_obj), 1.0, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (offset_spin), sven->osd->voffset);
	gtk_box_pack_start (GTK_BOX (hbox), offset_spin, FALSE, FALSE, 0);
	unit_label = gtk_label_new (_("pixels"));
	gtk_misc_set_alignment (GTK_MISC (unit_label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (unit_label), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start (GTK_BOX (hbox), unit_label, FALSE, FALSE, 0);
	
	// Horizontal Offset
	label = gtk_label_new (_("Horizontal Offset:"));
	gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 5,6,GTK_FILL, GTK_FILL, 0, 0);
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 5,6,GTK_FILL, GTK_FILL, 0, 0);
	
	h_offset_obj = gtk_adjustment_new (sven->osd->timeout, 0, 300, 1, 1, 1);
	h_offset_spin = gtk_spin_button_new (GTK_ADJUSTMENT (h_offset_obj), 1.0, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (h_offset_spin), sven->osd->hoffset);
	gtk_box_pack_start (GTK_BOX (hbox), h_offset_spin, FALSE, FALSE, 0);
	unit_label = gtk_label_new (_("pixels"));
	gtk_misc_set_alignment (GTK_MISC (unit_label), 0.0, 0.0);
	gtk_label_set_justify (GTK_LABEL (unit_label), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start (GTK_BOX (hbox), unit_label, FALSE, FALSE, 0);
	
	// Background color
	label = gtk_label_new (_("Background Color:"));
	gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 6,7,GTK_FILL, GTK_FILL, 0, 0);
	
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 6,7,GTK_FILL, GTK_FILL, 0, 0);
	
	bg_color_enable=gtk_check_button_new_with_mnemonic (_("Enable"));
	gtk_box_pack_start (GTK_BOX (hbox), bg_color_enable, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (bg_color_enable), sven->osd->bg_color_on);
	
	gtk_color_selection_palette_from_string (sven->osd->bg_color,&colors,&n_colors);
	bg_color = gtk_color_button_new_with_color (colors);
	
	gtk_box_pack_start (GTK_BOX (hbox), bg_color, FALSE, FALSE, 0);

	sep = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (vbox), sep, FALSE, FALSE, 10);

	hbox1 = gtk_hbox_new (TRUE, 10);
	gtk_widget_show (hbox1);
	
	enable_xosd = gtk_check_button_new_with_mnemonic (_("Enable XOSD"));
	gtk_widget_show (enable_xosd);
	gtk_box_pack_start (GTK_BOX (hbox1), enable_xosd, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (enable_xosd), sven->osd->osd_on);
	
	enable_frame = gtk_check_button_new_with_mnemonic (_("Enable Frame"));
	gtk_widget_show (enable_frame);
	gtk_box_pack_start (GTK_BOX (hbox1), enable_frame, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (enable_frame), sven->osd->debug_frame);
	
	enable_shadow = gtk_check_button_new_with_mnemonic (_("Enable Shadow"));
	gtk_widget_show (enable_shadow);
	gtk_box_pack_start (GTK_BOX (hbox1), enable_shadow, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (enable_shadow), sven->osd->drop_shadow);
	
	gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE, 0);
	
	sep = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (vbox), sep, FALSE, FALSE,5);

	button1 = gtk_button_new_with_mnemonic (_("Test OSD"));
	gtk_widget_show (button1);
	gtk_box_pack_start (GTK_BOX (vbox), button1, FALSE, FALSE, 5);
	g_signal_connect(button1, "clicked", G_CALLBACK(test_osd),sven);
	
	gtk_widget_show_all (vbox);
}

