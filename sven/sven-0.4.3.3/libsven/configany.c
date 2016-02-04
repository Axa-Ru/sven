/* Sven multimedia keyboard deamon
 * configany.c
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
#include "cdpl.h"
#include "mixer.h"
#include "plugin.h"


static GtkWidget *cd_entry,*mixer_entry,*proc_radio,*icon_check,*icon_check_desk,*startup_prog,*startup_prog_check,*enable_xosd,*enable_plugin=NULL;

void save_config_pref (Sven *sven)
{
	if (sven->mix->cdrom)
		g_free (sven->mix->cdrom);
	if (sven->mix->mixer)
		g_free (sven->mix->mixer);
	if (sven->mix->startup_prog)
		g_free (sven->mix->startup_prog);
	
	sven->mix->cdrom = g_strdup (gtk_entry_get_text (GTK_ENTRY (cd_entry)));
	sven->mix->mixer = g_strdup (gtk_entry_get_text (GTK_ENTRY (mixer_entry)));
	sven->mix->startup_prog = g_strdup(gtk_entry_get_text (GTK_ENTRY (startup_prog)));
	
	sven->mix->procces_view=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (proc_radio));
	sven->mix->icon_check=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (icon_check));
	sven->mix->icon_check_desk=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (icon_check_desk));
	sven->mix->startup_prog_check=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (startup_prog_check));
	sven->mix->enable_xosd=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (enable_xosd));
	sven->mix->enable_plugin=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (enable_plugin));
	
	greate_osd(sven->osd,sven->mix->enable_xosd);
	if(sven->mix->enable_plugin==1)
		sven_plugin_start(sven);
	cd_start(sven);
	restart_icon(sven);
	restart_tray(sven);
	
}

void create_pref_gui(Sven *sven,GtkWidget *vbox)
{
	GtkWidget  *hbox, *label;


	  GtkWidget *frame;
  GtkWidget *alignment;
  GSList *group = NULL;
 
	label = gtk_label_new (g_strdup_printf("<span size=\"xx-large\"><b><u><i>%s\n</i></u></b></span>",_("Other preference")));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0);

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

	cd_entry = gtk_entry_new ();
	gtk_widget_show (cd_entry);
	gtk_container_add (GTK_CONTAINER (alignment), cd_entry);
	if(sven->mix->cdrom!=NULL)
		gtk_entry_set_text (GTK_ENTRY(cd_entry),sven->mix->cdrom);
	else
		gtk_entry_set_text (GTK_ENTRY(cd_entry),CDDEVICE);

	label = gtk_label_new (_("<b><span color=\"#0046D5\">CD-Rom</span></b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

	mixer_entry = gtk_entry_new ();
	gtk_widget_show (mixer_entry);
	gtk_container_add (GTK_CONTAINER (alignment), mixer_entry);
	if(sven->mix->mixer!=NULL)
		gtk_entry_set_text (GTK_ENTRY(mixer_entry), sven->mix->mixer);
	else
		gtk_entry_set_text (GTK_ENTRY(mixer_entry),DEFMIXER);

	label = gtk_label_new (_("<b><span color=\"#0046D5\">Mixer</span></b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_container_add (GTK_CONTAINER (alignment), hbox);

	proc_radio = gtk_radio_button_new_with_mnemonic (NULL, _("All"));
	gtk_widget_show (proc_radio);
	gtk_box_pack_start (GTK_BOX (hbox), proc_radio, FALSE, TRUE, 0);
	gtk_radio_button_set_group (GTK_RADIO_BUTTON (proc_radio), group);
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (proc_radio));
	if(sven->mix->procces_view==0)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (proc_radio), TRUE);

	proc_radio = gtk_radio_button_new_with_mnemonic (NULL, _("User"));
	gtk_widget_show (proc_radio);
	gtk_box_pack_start (GTK_BOX (hbox), proc_radio, TRUE, TRUE, 0);
	gtk_radio_button_set_group (GTK_RADIO_BUTTON (proc_radio), group);
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (proc_radio));
	if(sven->mix->procces_view==1)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (proc_radio), TRUE);

	label = gtk_label_new (_("<b><span color=\"#0046D5\">View process</span></b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_container_add (GTK_CONTAINER (alignment), hbox);

	icon_check = gtk_check_button_new_with_mnemonic (_("Tray icon"));
	gtk_widget_show (icon_check);
	gtk_box_pack_start (GTK_BOX (hbox), icon_check, FALSE, TRUE, 0);
	if(sven->mix->icon_check==1)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (icon_check), TRUE);

	icon_check_desk = gtk_check_button_new_with_mnemonic (_("Desktop icon"));
	gtk_widget_show (icon_check_desk);
	gtk_box_pack_start (GTK_BOX (hbox), icon_check_desk, TRUE, TRUE, 0);
	if(sven->mix->icon_check_desk==1)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (icon_check_desk), TRUE);
	
	
	label = gtk_label_new (_("<b><span color=\"#0046D5\">View icon</span></b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_container_add (GTK_CONTAINER (alignment), hbox);

	startup_prog = gtk_entry_new ();
	gtk_widget_show (startup_prog);
	gtk_box_pack_start (GTK_BOX (hbox), startup_prog, FALSE, TRUE, 0);
	if(sven->mix->startup_prog!=NULL)
		gtk_entry_set_text (GTK_ENTRY(startup_prog),sven->mix->startup_prog);
	/*else
		gtk_entry_set_text (GTK_ENTRY(startup_prog),"");*/
		
	startup_prog_check = gtk_check_button_new_with_mnemonic (_("Enable"));
	gtk_widget_show (startup_prog_check);
	gtk_box_pack_start (GTK_BOX (hbox), startup_prog_check, FALSE, TRUE, 0);
	if(sven->mix->startup_prog_check==1)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (startup_prog_check), TRUE);

	label = gtk_label_new (_("<b><span color=\"#0046D5\">Run startup program</span></b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	enable_xosd = gtk_check_button_new_with_mnemonic (_("Enable"));
	gtk_widget_show (enable_xosd);
	gtk_container_add (GTK_CONTAINER (alignment), enable_xosd);
	if(sven->mix->enable_xosd==1)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (enable_xosd), TRUE);
	
	label = gtk_label_new (_("<b><span color=\"#0046D5\">View XOSD</span></b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	enable_plugin = gtk_check_button_new_with_mnemonic (_("Enable"));
	gtk_widget_show (enable_plugin);
	gtk_container_add (GTK_CONTAINER (alignment), enable_plugin);
	if(sven->mix->enable_plugin==1)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (enable_plugin), TRUE);
	
	label = gtk_label_new (_("<b><span color=\"#0046D5\">Start Plugins</span></b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	label = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
	
	gtk_widget_show_all (vbox);
}

