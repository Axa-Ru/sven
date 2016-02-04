/* Sven multimedia keyboard deamon
 * confcorrect.c
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

#include <gtk/gtksignal.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/X.h>
#include <gdk/gdkx.h>

#include "sven.h"
#include "key.h"
#include "mouse.h"
#include "plugin.h"
#include "confswitch.h"

static gchar *menu_tact=NULL;
static gchar *menu_tcom=NULL;

static void sconfswitch_class_init          (SconfswitchClass *klass);
static void sconfswitch_init                (Sconfswitch      *scsw);

GType
sconfswitch_get_type (void)
{
  static GType sccor_type = 0;

  if (!sccor_type)
    {
      static const GTypeInfo sccor_info =
      {
	sizeof (SconfswitchClass),
	NULL,		/* base_init */
	NULL,		/* base_finalize */
	(GClassInitFunc) sconfswitch_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data */
	sizeof (Sconfswitch),
	0,		/* n_preallocs */
	(GInstanceInitFunc) sconfswitch_init,
      };

      sccor_type = g_type_register_static (GTK_TYPE_VBOX, "Sconfswitch",
					    &sccor_info, 0);
    }

  return sccor_type;
}

static void
sconfswitch_class_init (SconfswitchClass *klass)
{
  
}

static void
sconfswitch_init (Sconfswitch *scsw)
{

}

static void check_switch_toggled(GtkToggleButton *togglebutton,Sconfswitch *scsw)
{
	scsw->auto_switch=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (scsw->check_switch));
}


/********************************Start Keyboard events********************************/
static void undo_key_press (GtkWidget *widget,GdkEventKey *event,Sconfswitch *scsw)
{
	char str[1000];
	char *mkey;

	ModifierToString (event->state, str);

	mkey=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
		      (XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), event->hardware_keycode, 0)) != NULL)
		      ? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),event->hardware_keycode, 0))
		      : g_strdup_printf ("%d",event->hardware_keycode));

	gtk_entry_set_text (GTK_ENTRY (scsw->undo_key),mkey);
	g_free(mkey);
}
static void undo_key_release (GtkWidget *widget,GdkEventKey *event,Sconfswitch *scsw)
{
	guint i = 0;
	char **key_stat = g_strsplit(g_strdup(gtk_entry_get_text (GTK_ENTRY (scsw->undo_key))),"+",4);

	while (key_stat[i])
		i++;

	if((g_strcasecmp (key_stat[i-1],"Control")==0)|| (g_strcasecmp (key_stat[i-1],"Shift")==0)|| (g_strcasecmp (key_stat[i-1],"Alt")==0)|| (g_strcasecmp (key_stat[i-1],"Control_R")==0)|| (g_strcasecmp (key_stat[i-1],"Shift_R")==0)|| (g_strcasecmp (key_stat[i-1],"Alt_R")==0)|| (g_strcasecmp (key_stat[i-1],"Control_L")==0)|| (g_strcasecmp (key_stat[i-1],"Shift_L")==0)|| (g_strcasecmp (key_stat[i-1],"Alt_l")==0))
	gtk_entry_set_text (GTK_ENTRY (scsw->undo_key),_("None"));
}

static void case_key_press (GtkWidget *widget,GdkEventKey *event,Sconfswitch *scsw)
{
	char str[1000];
	char *mkey;

	ModifierToString (event->state, str);

	mkey=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
		      (XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), event->hardware_keycode, 0)) != NULL)
		      ? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),event->hardware_keycode, 0))
		      : g_strdup_printf ("%d",event->hardware_keycode));

	gtk_entry_set_text (GTK_ENTRY (scsw->case_key),mkey);
	g_free(mkey);
}
static void case_key_release (GtkWidget *widget,GdkEventKey *event,Sconfswitch *scsw)
{
	guint i = 0;
	char **key_stat = g_strsplit(g_strdup(gtk_entry_get_text (GTK_ENTRY (scsw->case_key))),"+",4);

	while (key_stat[i])
		i++;

	if((g_strcasecmp (key_stat[i-1],"Control")==0)|| (g_strcasecmp (key_stat[i-1],"Shift")==0)|| (g_strcasecmp (key_stat[i-1],"Alt")==0)|| (g_strcasecmp (key_stat[i-1],"Control_R")==0)|| (g_strcasecmp (key_stat[i-1],"Shift_R")==0)|| (g_strcasecmp (key_stat[i-1],"Alt_R")==0)|| (g_strcasecmp (key_stat[i-1],"Control_L")==0)|| (g_strcasecmp (key_stat[i-1],"Shift_L")==0)|| (g_strcasecmp (key_stat[i-1],"Alt_l")==0))
	gtk_entry_set_text (GTK_ENTRY (scsw->case_key),_("None"));
}

static void focus_key_press (GtkWidget *widget,GdkEventKey *event,Sconfswitch *scsw)
{
	char str[1000];
	char *mkey;

	ModifierToString (event->state, str);

	mkey=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
		      (XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), event->hardware_keycode, 0)) != NULL)
		      ? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),event->hardware_keycode, 0))
		      : g_strdup_printf ("%d",event->hardware_keycode));

	gtk_entry_set_text (GTK_ENTRY (scsw->focus_key),mkey);
	g_free(mkey);
}
static void focus_key_release (GtkWidget *widget,GdkEventKey *event,Sconfswitch *scsw)
{
	guint i = 0;
	char **key_stat = g_strsplit(g_strdup(gtk_entry_get_text (GTK_ENTRY (scsw->focus_key))),"+",4);

	while (key_stat[i])
		i++;

	if((g_strcasecmp (key_stat[i-1],"Control")==0)|| (g_strcasecmp (key_stat[i-1],"Shift")==0)|| (g_strcasecmp (key_stat[i-1],"Alt")==0)|| (g_strcasecmp (key_stat[i-1],"Control_R")==0)|| (g_strcasecmp (key_stat[i-1],"Shift_R")==0)|| (g_strcasecmp (key_stat[i-1],"Alt_R")==0)|| (g_strcasecmp (key_stat[i-1],"Control_L")==0)|| (g_strcasecmp (key_stat[i-1],"Shift_L")==0)|| (g_strcasecmp (key_stat[i-1],"Alt_l")==0))
	gtk_entry_set_text (GTK_ENTRY (scsw->focus_key),_("None"));
}


GtkWidget* sconfswitch_new (Sven *sven,gchar *name)
{
	GtkWidget *widget;
	Sconfswitch *scsw;
	GtkWidget *box,*page_label,*vbox;
	
	 GtkWidget *allbox;
  GtkWidget *frame;
  GtkWidget *alignment;
  GtkWidget *hbox;
  GtkWidget *entry;
  GtkWidget *label;
  GtkWidget *checkbutton;
  GtkWidget *hbox1;
  GtkWidget *entry1;
	
	g_return_val_if_fail (sven != NULL , NULL);
 

	widget = g_object_new (SCONFSWITCH_TYPE,NULL);
	box = GTK_WIDGET (widget);
	scsw = SCONFSWITCH (widget);
  
	scsw->sven=sven;
	scsw->auto_switch=scsw->sven->keyboard->auto_switch;
		
	page_label = gtk_label_new (g_strdup_printf("<span size=\"xx-large\"><b><u><i> %s \n</i></u></b></span>",name));
	gtk_widget_show (page_label);
	gtk_box_pack_start (GTK_BOX (box), page_label, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (page_label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (page_label), 0, 0);
	
	allbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (allbox);
	gtk_box_pack_start (GTK_BOX (box), allbox, TRUE,TRUE, 0);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (allbox), frame, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("<b>Undo/Forced switch</b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	alignment = gtk_alignment_new (0, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 5, 0);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (alignment), vbox);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);
	
	scsw->undo_key = gtk_entry_new ();
	gtk_widget_show (scsw->undo_key);
	gtk_entry_set_text (GTK_ENTRY (scsw->undo_key),scsw->sven->keyboard->conv_lang);
	gtk_signal_connect (GTK_OBJECT (scsw->undo_key), "key_press_event",
				GTK_SIGNAL_FUNC (undo_key_press),scsw);
	
	gtk_signal_connect (GTK_OBJECT (scsw->undo_key), "key_release_event",
				GTK_SIGNAL_FUNC (undo_key_release),scsw);
	gtk_box_pack_start (GTK_BOX (hbox), scsw->undo_key, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("<span color=\"red\">Use this key to undo the last automatic change or to manually force layout change of the last entered word.</span>"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
	gtk_misc_set_padding (GTK_MISC (label), 9, 0);
	
	
	alignment = gtk_alignment_new (0.1, 1, 0, 1);
	gtk_widget_show (alignment);
	gtk_box_pack_start (GTK_BOX (vbox), alignment, FALSE, FALSE, 0);
	
	scsw->check_switch = gtk_check_button_new_with_mnemonic (_("AutoSwitch - switch keyboard language automatically"));
	gtk_widget_show (scsw->check_switch);
	g_signal_connect (G_OBJECT (scsw->check_switch), "toggled",G_CALLBACK (check_switch_toggled),scsw);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(scsw->check_switch),scsw->auto_switch);
	gtk_container_add (GTK_CONTAINER (alignment), scsw->check_switch);
	
	
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (allbox), frame, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("<b>Selected text convert</b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 8);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 5, 0);
	
	hbox1 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox1);
	gtk_container_add (GTK_CONTAINER (alignment), hbox1);
	
	scsw->focus_key = gtk_entry_new ();
	gtk_widget_show (scsw->focus_key);

	gtk_entry_set_text (GTK_ENTRY (scsw->focus_key),scsw->sven->keyboard->conv_focus);
	gtk_signal_connect (GTK_OBJECT (scsw->focus_key), "key_press_event",
				GTK_SIGNAL_FUNC (focus_key_press),scsw);
	
	gtk_signal_connect (GTK_OBJECT (scsw->focus_key), "key_release_event",
				GTK_SIGNAL_FUNC (focus_key_release),scsw);
	gtk_box_pack_start (GTK_BOX (hbox1), scsw->focus_key, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("<span color=\"red\">Use this key to manually force layout change of any selected text.</span>"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox1), label, TRUE, TRUE, 0);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
	gtk_misc_set_padding (GTK_MISC (label), 9, 0);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (allbox), frame, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("<b>Invert case</b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 8);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 5, 0);
	
	hbox1 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox1);
	gtk_container_add (GTK_CONTAINER (alignment), hbox1);
	
	scsw->case_key = gtk_entry_new ();
	gtk_widget_show (scsw->case_key);

	gtk_entry_set_text (GTK_ENTRY (scsw->case_key),scsw->sven->keyboard->conv_case);
	gtk_signal_connect (GTK_OBJECT (scsw->case_key), "key_press_event",
				GTK_SIGNAL_FUNC (case_key_press),scsw);
	
	gtk_signal_connect (GTK_OBJECT (scsw->case_key), "key_release_event",
				GTK_SIGNAL_FUNC (case_key_release),scsw);
	gtk_box_pack_start (GTK_BOX (hbox1), scsw->case_key, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("<span color=\"red\">Pressing this key will invert the character case of the last entered word or of any selected text</span>"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox1), label, TRUE, TRUE, 0);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
	gtk_misc_set_padding (GTK_MISC (label), 9, 0);
	
	
	
	return GTK_WIDGET (widget);
}

void sconfswitch_save (Sconfswitch *scsw)
{
	ConfigFile *cfgfile;
	gchar *filename;
	int key=0;
	int mod=0;
	int i;
	
	scsw->sven->keyboard->auto_switch=scsw->auto_switch;
	
	scsw->sven->keyboard->conv_lang=g_strdup(gtk_entry_get_text (GTK_ENTRY (scsw->undo_key)));
	scsw->sven->keyboard->conv_focus=g_strdup(gtk_entry_get_text (GTK_ENTRY (scsw->focus_key)));
	scsw->sven->keyboard->conv_case=g_strdup(gtk_entry_get_text (GTK_ENTRY (scsw->case_key)));
	
	filename= g_strconcat(g_get_home_dir(),"/.sven/config", NULL);
	cfgfile = sven_cfg_open_file(filename);

	if (!cfgfile)
		cfgfile = sven_cfg_new();

	sven_cfg_write_int (cfgfile, "keyboard", "auto_switch", scsw->sven->keyboard->auto_switch);
	sven_cfg_write_string (cfgfile, "keyboard", "conv_lang", scsw->sven->keyboard->conv_lang);
	sven_cfg_write_string (cfgfile, "keyboard", "conv_focus", scsw->sven->keyboard->conv_focus);
	sven_cfg_write_string (cfgfile, "keyboard", "conv_case", scsw->sven->keyboard->conv_case);

	sven_cfg_write_file(cfgfile, filename);
	
	scsw->sven->screen->display = gdk_display_get_default ();
	scsw->sven->screen->screens = NULL;

	if (gdk_display_get_n_screens (scsw->sven->screen->display) == 1)
	{
		scsw->sven->screen->screens = g_list_append (scsw->sven->screen->screens,
				gdk_screen_get_default ());
	} else 
	{
		for (i = 0; i < gdk_display_get_n_screens (scsw->sven->screen->display); i++)
		{
	 		GdkScreen *screen;

			screen = gdk_display_get_screen (scsw->sven->screen->display, i);

			if (screen != NULL)
				scsw->sven->screen->screens = g_list_append (scsw->sven->screen->screens,screen);
		}
	}
	
	
	key=StringToModifier_Key(g_strdup(scsw->sven->keyboard->conv_lang),&mod);
	grab_key (scsw->sven->screen,key,mod);
	
	key=StringToModifier_Key(g_strdup(scsw->sven->keyboard->conv_focus),&mod);
	grab_key (scsw->sven->screen,key,mod);
	
	key=StringToModifier_Key(g_strdup(scsw->sven->keyboard->conv_case),&mod);
	grab_key (scsw->sven->screen,key,mod);
	
	sven_cfg_free(cfgfile);
	g_free(filename);
}

