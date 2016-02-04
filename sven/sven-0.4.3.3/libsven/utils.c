/* Sven multimedia keyboard deamon
 * utils.c
 *
 *Copyright (C) 2005 Eugene Morenko(More)
 *mailto: more@irpin.com
 *web: http://sven.linux.kiev.ua/
 *icq:325697438
 *
 *any past code take from XFCE
 * Copyright (c) 2003 Jasper Huijsmans <huysmans@users.sourceforge.net>
 * Copyright (c) 2003 Benedikt Meurer <benny@xfce.org>
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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>

#include <dirent.h>
#include <asm/page.h>


#include "sven.h"
#include "key.h"
#include "utils.h"
#include "mixer.h"
#include "cdpl.h"
#include "eggtrayicon.h"
#include "prefswin.h"
#include "clipman.h"
#include "wininfo.h"


GtkWidget* xfce_create_mixed_button (const gchar *stock, const gchar *text);
gboolean xfce_confirm (const gchar *text, const gchar *stock_id,const gchar *action);


/*Start Greate Image*/
#include "../images/flag/sven_flags.h"

typedef struct {
	const gchar *name;
	const gchar *lang;
	const guint8 *data;
} Tflagpix;

static Tflagpix tfp[] = {
	{"United Arab Emirates","ae",ae},
	{"Armenian","am",am},
	{"Argentina","ar",ar},
	{"Austria","at",at},
	{"Azerbaijan","az",az},
	{"Belgian","be",be},
	{"Cyrillic","bg",bg},
	{"Bahrain","bh",bh},
	{"Brazilian","br",br},
	{"Belarusian","by",by},
	{"Canadian","ca",ca},
	{"Switzerland","ch",ch},
	{"Cuba","cu",cu},
	{"Czech","cz",cz},
	{"German","de",de},
	{"Djibouti","dj",dj},
	{"Danish","dk",dk},
	{"Dzongkha","dz",dz},
	{"Estonian","ee",ee},
	{"Egypt","eg",eg},
	{"Spanish","es",es},
	{"Finnish","fi",fi},
	{"French","fr",fr},
	{"Great Britain","gb",gb},
	{"Georgian","ge",ge},
	{"ISO8859-7","gr",gr},
	{"Croatian","hr",hr},
	{"Croatian(US)","hr",hr},
	{"Hungarian","hu",hu},
	{"Israelian","il",il},
	{"Iraq","iq",iq},
	{"Icelandic","is",is},
	{"Italian","it",it},
	{"Jordan","jo",jo},
	{"Japanese","jp",jp},
	{"Comoros","km",km},
	{"South Korea","kr",kr},
	{"Kuwait","kw",kw},
	{"Latin American","la",la},
	{"Lebanon","lb",lb},
	{"Lithuanian","lt",lt},
	{"Libya","ly",ly},
	{"Morocco","ma",ma},
	{"Macedonian","mk",mk},
	{"Mongolia","mn",mn},
	{"Mexico","mx",mx},
	{"Nederland","nl",nl},
	{"Norwegian","no",no},
	{"Sámegiella","se",no},
	{"Oman","om",om},
	{"Polish","pl",pl},
	{"Palestinian Territory","ps",ps},
	{"Portuguese","pt",pt},
	{"Qatar","qa",qa},
	{"Romanian","ro",ro},
	{"Russian","ru",ru},
	{"Russian(phonetic)","ru",ru},
	{"Saudi Arabia","sa",sa},
	{"Sudan","sd",sd},
	{"Swedish","se",se},
	{"Slovene","si",si},
	{"Slovak","sk",sk},
	{"Somalia","so",so},
	{"Serbian Cyrillic","sr",sr},
	{"Serbian","sr",sr},
	{"Syria","sy",sy},
	{"Thai (Kedmanee)","th",th},
	{"Thai (Pattachote)","th",th},
	{"Thai (TIS-820.2538)","th",th},
	{"Tunisia","tn",tn},
	{"Turkish","tr",tr},
	{"Ukrainian","ua",ua},
	{"Maltese (US layout)","mt",us},
	{"US/ASCII","us",us},
	{"en_US","us",us},
	{"English","us",us},
	{"Uruguay","uy",uy},
	{"Vietnamese","vn",vn},
	{"Yemen","ye",ye},
	{"Yugoslavia","yu",yu},
	{"fr-latin9","in9",zz},
	{"TSCII","tml",zz},
	{"Telugu","tel",zz},
	{"Syriac","syr",zz},
	{"Oriya","ori",zz},
	{"Ogham","ham",zz},
	{"Lao","lo",zz},
	{"Inuktitut","iu",zz},
	{"Dvorak","dvo",zz},
	{"Gurmukhi","gur",zz},
	{"Gujarati","guj",zz},
	{"Bengali","ben",zz},
	{"Arabic","ar",zz},
	{"Kannada","kan",zz},
	{"ISO9995-3","zz",zz},
	{"NULL","zz",zz},
};

/* This is an internally used function to create pixmaps. */
GdkPixbuf *new_pixbuf_flag(gchar *langs, int width, int height)
{
	gint i;
	GdkPixbuf *pixbuf;
	/*gchar *lang;
	lang=g_strndup(&langs[0],2);*/
//	printf("Lang:%s\n",langs);
		for (i=0;i<(sizeof(tfp)/sizeof(Tflagpix));i++) {
			if(g_strcasecmp (tfp[i].name,langs)==0) {
				break;
			}
		}
		if (i >= sizeof(tfp)/sizeof(Tflagpix)) {
			DEBUG_MSG(_("new_pixmap, requested unknown type %d\n"), type);
			i = sizeof(tfp)/sizeof(Tflagpix)-1;
		}
		pixbuf = gdk_pixbuf_new_from_inline(-1,tfp[i].data,FALSE,NULL);
		    g_assert (pixbuf);

    if ((width > 0 && height > 0)
	|| (gdk_pixbuf_get_width (pixbuf) != width
	    && gdk_pixbuf_get_height (pixbuf) != height))
    {
	GdkPixbuf *scaled;

	scaled =
	    gdk_pixbuf_scale_simple (pixbuf,
				     width >
				     0 ? width : gdk_pixbuf_get_width (pixbuf),
				     height >
				     0 ? height :
				     gdk_pixbuf_get_height (pixbuf),
				     GDK_INTERP_BILINEAR);
	pixbuf=scaled;
    }
	return pixbuf;
}
 
GtkWidget *new_pixmap_flag(gchar *langs, int width, int height) {
	GtkWidget *wid;
	gint i;
	GdkPixbuf *pixbuf;
	/*gchar *lang;
	lang=g_strndup(&langs[0],2);
	printf("Lang:%s\n",lang);*/
		for (i=0;i<(sizeof(tfp)/sizeof(Tflagpix));i++) {
			if(g_strcasecmp (tfp[i].name,langs)==0) {
				break;
			}
		}
		if (i >= sizeof(tfp)/sizeof(Tflagpix)) {
			DEBUG_MSG(_("new_pixmap, requested unknown type %d\n"), type);
			i = sizeof(tfp)/sizeof(Tflagpix)-1;
		}
		pixbuf = gdk_pixbuf_new_from_inline(-1,tfp[i].data,FALSE,NULL);
		    g_assert (pixbuf);

    if ((width > 0 && height > 0)
	|| (gdk_pixbuf_get_width (pixbuf) != width
	    && gdk_pixbuf_get_height (pixbuf) != height))
    {
	GdkPixbuf *scaled;

	scaled =
	    gdk_pixbuf_scale_simple (pixbuf,
				     width >
				     0 ? width : gdk_pixbuf_get_width (pixbuf),
				     height >
				     0 ? height :
				     gdk_pixbuf_get_height (pixbuf),
				     GDK_INTERP_BILINEAR);
	pixbuf=scaled;
    }
	wid = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);
	return wid;
}
/*END Greate Image*/


/*Start Greate Image*/
#include "../images/sven_icons.h"
#include "../images/sven.xpm"

int wx,wy,dragx,dragy;

typedef struct {
	const guint id;
	const guint8 *data;
} Tpixmap;

static Tpixmap tp[] = {
	{1, sven},
	{2,topleft},
	{3,top},
	{4,topright},
	{5,bottomleft},
	{6,bottom},
	{7,bottomright},
	{8,left},
	{9,centre},
	{10,right},
	{11,configure},
	{12,data_visualization_section},
	{13,key_bindings},
	{14,klipper_dock},
	{15,messagebox_info},
	{16,plugins},
	{17,terminal},
	{18,cdaudio},
	{19,cdplay},
	{20,cdprev},
	{21,cdstop},
	{22,cdpause},
	{23,cdnext},
	{24,cdeject},
	{25,volup},
	{26,voldown},
	{27,volmute},
	{28,vol},
	{29,mouse},
	{30,locale}
};
/* This is an internally used function to create pixmaps. */
GdkPixbuf *create_pixbuf (gint type, int width, int height)
{
	gint i;
	GdkPixbuf *pixbuf;

		for (i=0;i<(sizeof(tp)/sizeof(Tpixmap));i++) {
			if (tp[i].id == type) {
				break;
			}
		}
		if (i >= sizeof(tp)/sizeof(Tpixmap)) {
			DEBUG_MSG(_("new_pixmap, requested unknown type %d\n"), type);
			i = sizeof(tp)/sizeof(Tpixmap)-1;
		}
		pixbuf = gdk_pixbuf_new_from_inline(-1,tp[i].data,FALSE,NULL);
		    g_assert (pixbuf);

    if ((width > 0 && height > 0)
	|| (gdk_pixbuf_get_width (pixbuf) != width
	    && gdk_pixbuf_get_height (pixbuf) != height))
    {
	GdkPixbuf *scaled;

	scaled =
	    gdk_pixbuf_scale_simple (pixbuf,
				     width >
				     0 ? width : gdk_pixbuf_get_width (pixbuf),
				     height >
				     0 ? height :
				     gdk_pixbuf_get_height (pixbuf),
				     GDK_INTERP_BILINEAR);
	pixbuf=scaled;
    }
	return pixbuf;
}

GtkWidget *new_pixmap_mod(gint type, int width, int height) {
	GtkWidget *wid;
	gint i;
	GdkPixbuf *pixbuf;

		for (i=0;i<(sizeof(tp)/sizeof(Tpixmap));i++) {
			if (tp[i].id == type) {
				break;
			}
		}
		if (i >= sizeof(tp)/sizeof(Tpixmap)) {
			DEBUG_MSG(_("new_pixmap, requested unknown type %d\n"), type);
			i = sizeof(tp)/sizeof(Tpixmap)-1;
		}
		pixbuf = gdk_pixbuf_new_from_inline(-1,tp[i].data,FALSE,NULL);
		    g_assert (pixbuf);

    if ((width > 0 && height > 0)
	|| (gdk_pixbuf_get_width (pixbuf) != width
	    && gdk_pixbuf_get_height (pixbuf) != height))
    {
	GdkPixbuf *scaled;

	scaled =
	    gdk_pixbuf_scale_simple (pixbuf,
				     width >
				     0 ? width : gdk_pixbuf_get_width (pixbuf),
				     height >
				     0 ? height :
				     gdk_pixbuf_get_height (pixbuf),
				     GDK_INTERP_BILINEAR);
	pixbuf=scaled;
    }
	wid = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);
	return wid;
}
/*END Greate Image*/

/* compat stub */
GtkWidget*
mixed_button_new (const char *stock, const char *text)
{
    return xfce_create_mixed_button (stock, text);
}

/**
 * xfce_create_mixed_button:
 * @stock: a stock item name.
 * @text: a text to display.
 *
 * Creates a button with both @stock icon and @text.
 *
 * Return value: the newly created mixed button widget.
 **/
GtkWidget*
xfce_create_mixed_button (const gchar *stock, const gchar *text)
{
    GtkWidget *button, *align, *image, *hbox, *label;

    button = gtk_button_new ();
    label = gtk_label_new_with_mnemonic (text);
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), button);

    image = gtk_image_new_from_stock (stock, GTK_ICON_SIZE_BUTTON);
    hbox = gtk_hbox_new (FALSE, 2);

    align = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);

    gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbox), label, FALSE, FALSE, 0);

    gtk_container_add (GTK_CONTAINER (button), align);
    gtk_container_add (GTK_CONTAINER (align), hbox);
    gtk_widget_show_all (align);

    return button;
}

static void
message_dialog (GtkMessageType type, const gchar * message)
{
	GtkWidget *dlg;
	
	dlg = gtk_message_dialog_new (NULL,
				  GTK_DIALOG_MODAL,
				  type, GTK_BUTTONS_CLOSE, message);
	gtk_label_set_use_markup (GTK_LABEL (GTK_MESSAGE_DIALOG (dlg)->label), TRUE);
	gtk_window_set_position (GTK_WINDOW (dlg), GTK_WIN_POS_CENTER);
	gtk_dialog_run (GTK_DIALOG (dlg));
	gtk_widget_destroy (dlg);
}

/* wrappers around gtk_message_dialog (OBSOLETE) */
void show_info (const char *text)
{
    message_dialog (GTK_MESSAGE_INFO, text);
}

void show_warning (const char *text)
{
    message_dialog (GTK_MESSAGE_WARNING, text);
}

void show_error (const char *text)
{
    message_dialog (GTK_MESSAGE_ERROR, text);
}

/* compat stub */
gboolean confirm (const char *text, const char *stock, const char *action)
{
    return xfce_confirm (text, stock, action);
}

/**
 * xfce_confirm:
 * @text:     a question text
 * @stock_id: a stock item name
 * @action:   if non-NULL, this text is used on the confirm button together
 *            with the @stock icon.
 *
 * Runs a modal confirmation dialog, that has a 'cancel' and a 'confirm'
 * button. The 'confirm' button text can be set by @action if given.
 *
 * If @stock_id is equal to GTK_STOCK_YES, the 'cancel' button becomes a 'no' button.
 *
 * Return value: TRUE if the user confirms, else FALSE.
 **/
gboolean
xfce_confirm (const gchar *text,
              const gchar *stock_id,
              const gchar *action)
{
    GtkWidget *dialog, *button;
    int response = GTK_RESPONSE_NONE;

    dialog = gtk_message_dialog_new (NULL,
                                     GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE,
                                     text);
	gtk_label_set_use_markup (GTK_LABEL (GTK_MESSAGE_DIALOG (dialog)->label), TRUE);
    if (strcmp (stock_id, GTK_STOCK_YES) == 0)
        button = gtk_button_new_from_stock (GTK_STOCK_NO);
    else
        button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);

    gtk_widget_show (button);
    gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button,
                                  GTK_RESPONSE_NO);

    if (action)
        button = mixed_button_new (stock_id, action);
    else
        button = gtk_button_new_from_stock (stock_id);
    gtk_widget_show (button);
    gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button,
                                  GTK_RESPONSE_YES);

   // gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);

    response = gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_hide (dialog);
    gtk_widget_destroy (dialog);

    if (response == GTK_RESPONSE_YES)
        return TRUE;
    else
        return FALSE;
}

void sven_exit (GtkMenuItem *menuitem,Sven *sven)
{
	update_kbd(sven);
	update_mouse(sven);
	if(sven->wininfo->info_win!=NULL)
		gtk_window_get_position(GTK_WINDOW(sven->wininfo->info_win),&sven->mix->x_icon,&sven->mix->y_icon);
	sven_write_config(sven);
	gtk_main_quit();
	exit (1);
}

#define DIR_MODE (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)	/* same as 0755 */
void file_check_directory(void) 
{
	gchar *rcdir = g_strconcat(g_get_home_dir(), "/.sven", NULL);
	
	if (!g_file_test(rcdir, G_FILE_TEST_IS_DIR))
		mkdir(rcdir, DIR_MODE);
	else
		DEBUG_MSG("HOME Sven DIR OK!\n");
	
	g_free(rcdir);
}

void printerror (void)
{
	printf (RED "ERROR: %s", xosd_error);
	printf(NORMAL"\n");
	show_error(xosd_error);
}

void execute (char *cmd, gboolean sync)
{
	gboolean retval;

	if (sync != FALSE)
		retval = g_spawn_command_line_sync
			(cmd, NULL, NULL, NULL, NULL);
	else
		retval = g_spawn_command_line_async (cmd, NULL);

	if (retval == FALSE)
	{
		char *msg;

		msg = g_strdup_printf
			(_("Couldn't execute command: \"<span weight=\"bold\" color=\"red\">%s</span>\"\n"
			   "Verify that this command exists.\n"),cmd);
		show_error(msg);
		printf(RED"ERROR:%s",msg);
		printf(NORMAL"\n");
		g_free (msg);
	}
}

MXosd *greate_osd (MXosd  *osd,int enable)
{
	if(osd->osd)
		xosd_destroy(osd->osd);
	if(enable==1)
		osd->osd = xosd_create(2);
	else
		osd->osd = NULL;
	
	return osd;
}
void apply_xosd_config (MXosd  *osd)
{
	DEBUG_MSG("apply_config\n");
	if (osd->osd)
	{
		if (xosd_set_font (osd->osd, osd->font) == -1)
		{
			g_print ("invalid font %s\n", osd->font);
		}
		xosd_set_colour (osd->osd, osd->colour);
		xosd_set_timeout (osd->osd, osd->timeout);
		xosd_set_shadow_offset (osd->osd, osd->shadow_offset);
		xosd_set_outline_offset (osd->osd, osd->outline_offset);
		xosd_set_pos (osd->osd, osd->pos);
		xosd_set_align (osd->osd, osd->align);
		xosd_set_vertical_offset (osd->osd, osd->offset);
		xosd_set_horizontal_offset (osd->osd, osd->h_offset);
		xosd_set_bar_length (osd->osd,40);
	}
	DEBUG_MSG("done\n");
}

void sven_read_config(Sven *conf)
{
	ConfigFile *cfgfile;
	DEBUG_MSG("read_config\n");
	
	conf->mix=g_malloc0(sizeof (Mmix));

	g_free (conf->osd->colour);
	g_free (conf->osd->font);
	g_free (conf->mix->cdrom);
	g_free (conf->mix->mixer);
	g_free (conf->mix->startup_prog);
	conf->osd->colour = NULL;
	conf->osd->font = NULL;
	conf->mix->cdrom = NULL;
	conf->mix->mixer = NULL;
	conf->mix->startup_prog = NULL;
	
	conf->osd->timeout = 3;
  	conf->osd->offset = 50;
	conf->osd->h_offset = 0;
	conf->osd->shadow_offset = 1;
	conf->osd->outline_offset = 0;
	conf->osd->pos = XOSD_bottom;
	conf->osd->align = XOSD_left;

	conf->mix->procces_view = 0;
	conf->mix->icon_check = 1;
	conf->mix->icon_check_desk = 1;
	conf->mix->startup_prog_check = 1;
	conf->mix->x_icon = 100;
	conf->mix->y_icon = 100;
	conf->mix->enable_xosd = 1;
	conf->mix->enable_plugin = 1;
	

	DEBUG_MSG ("reading configuration data\n");
	cfgfile = sven_cfg_open_file(g_strconcat(g_get_home_dir(),"/.sven/config", NULL));
	if (!cfgfile)
		cfgfile = sven_cfg_new();
	
	if (cfgfile!= NULL)
	{
		sven_cfg_read_string (cfgfile, "sven", "font", &conf->osd->font);
		sven_cfg_read_string (cfgfile, "sven", "colour", &conf->osd->colour);
		sven_cfg_read_int (cfgfile, "sven", "timeout", &conf->osd->timeout);
		sven_cfg_read_int (cfgfile, "sven", "offset", &conf->osd->offset);
		sven_cfg_read_int (cfgfile, "sven", "h_offset", &conf->osd->h_offset);
		sven_cfg_read_int (cfgfile, "sven", "shadow_offset", &conf->osd->shadow_offset);
		sven_cfg_read_int (cfgfile, "sven", "outline_offset", &conf->osd->outline_offset);
		sven_cfg_read_int (cfgfile, "sven", "pos", &conf->osd->pos);
		sven_cfg_read_int (cfgfile, "sven", "align", &conf->osd->align);
		
		sven_cfg_read_string (cfgfile, "sven", "cdrom", &conf->mix->cdrom);
		sven_cfg_read_string (cfgfile, "sven", "mixer", &conf->mix->mixer);
		sven_cfg_read_string (cfgfile, "sven", "startup_prog", &conf->mix->startup_prog);
		sven_cfg_read_int (cfgfile, "sven", "procces_view", &conf->mix->procces_view);
		sven_cfg_read_int (cfgfile, "sven", "icon_check", &conf->mix->icon_check);
		sven_cfg_read_int (cfgfile, "sven", "icon_check_desk", &conf->mix->icon_check_desk);
		sven_cfg_read_int (cfgfile, "sven", "startup_prog_check", &conf->mix->startup_prog_check);
		sven_cfg_read_int (cfgfile, "sven", "enable_xosd", &conf->mix->enable_xosd);
		sven_cfg_read_int (cfgfile, "sven", "enable_plugin", &conf->mix->enable_plugin);
		sven_cfg_read_int (cfgfile, "sven", "x_icon", &conf->mix->x_icon);
		sven_cfg_read_int (cfgfile, "sven", "y_icon", &conf->mix->y_icon);
        	sven_cfg_free (cfgfile);
	}

	if (conf->osd->font == NULL)
	{
		conf->osd->font = g_strdup (osd_default_font);
		DEBUG_MSG ("getting default font\n");
	}

	if (conf->osd->colour == NULL)
	{
		conf->osd->colour = g_strdup ("green");
		DEBUG_MSG ("default colour\n");
	}

	DEBUG_MSG ("done\n");
}
/* Save current settings to the sven config file */
void sven_write_config (Sven *conf)
{
 	ConfigFile *cfgfile;
	gchar *filename;

	filename= g_strconcat(g_get_home_dir(),"/.sven/config", NULL);
	cfgfile = sven_cfg_open_file(filename);

	if (!cfgfile)
		cfgfile = sven_cfg_new();

	sven_cfg_write_string 	(cfgfile, "sven", "colour", conf->osd->colour);
	sven_cfg_write_string 	(cfgfile, "sven", "font", conf->osd->font);
	sven_cfg_write_int 	(cfgfile, "sven", "timeout", conf->osd->timeout);
	sven_cfg_write_int 	(cfgfile, "sven", "offset", conf->osd->offset);
	sven_cfg_write_int 	(cfgfile, "sven", "h_offset", conf->osd->h_offset);
	sven_cfg_write_int 	(cfgfile, "sven", "shadow_offset", conf->osd->shadow_offset);
	sven_cfg_write_int 	(cfgfile, "sven", "outline_offset", conf->osd->outline_offset);
	sven_cfg_write_int 	(cfgfile, "sven", "pos", conf->osd->pos);
	sven_cfg_write_int 	(cfgfile, "sven", "align", conf->osd->align);
	
	sven_cfg_write_string 	(cfgfile, "sven", "cdrom",conf->mix->cdrom);
	sven_cfg_write_string	(cfgfile, "sven", "mixer", conf->mix->mixer);
	sven_cfg_write_string	(cfgfile, "sven", "startup_prog", conf->mix->startup_prog);
	sven_cfg_write_int 	(cfgfile, "sven", "procces_view",conf->mix->procces_view);
	sven_cfg_write_int 	(cfgfile, "sven", "icon_check",conf->mix->icon_check);
	sven_cfg_write_int 	(cfgfile, "sven", "icon_check_desk",conf->mix->icon_check_desk);
	sven_cfg_write_int 	(cfgfile, "sven", "startup_prog_check",conf->mix->startup_prog_check);
	sven_cfg_write_int 	(cfgfile, "sven", "enable_xosd",conf->mix->enable_xosd);
	sven_cfg_write_int 	(cfgfile, "sven", "enable_plugin",conf->mix->enable_plugin);
	sven_cfg_write_int	(cfgfile, "sven", "x_icon", conf->mix->x_icon);
	sven_cfg_write_int	(cfgfile, "sven", "y_icon", conf->mix->y_icon);

	sven_cfg_write_file(cfgfile, filename);
	sven_cfg_free(cfgfile);
	g_free(filename);
}

static void menu_kill(GtkMenuItem     *menuitem,int pid)
{
	 gchar *output = NULL;
	
	if(confirm(_("Do you want to kill this procces?"), "gtk-ok", NULL))
	{
		g_spawn_command_line_sync(g_strdup_printf("kill %d",pid),NULL,&output,NULL,NULL);
		if(g_strcasecmp(output,"")!=0)
			show_error(_(output));
	}
}

static gboolean tray_icon_release (GtkWidget *widget, GdkEventButton *event,Sven *sven)
{	
	if (event->button ==3)
	{
		gtk_menu_popdown (GTK_MENU (sven->popup_menu));
		return FALSE;
	}

	return TRUE;
}

gboolean icon_press_all (GtkWidget *widget, GdkEventButton *event, Sven *sven)
{
	if (event->button == 2)
	{
		clicked_cb(widget,sven->clipman);
		gtk_menu_popup (GTK_MENU(sven->clipman->menu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
		return 1;
	}
	
	if (event->button == 3)
	{
		GtkWidget *shell_menu_text;
		proc_info * p;
		int j = 1;
		int i = 1;
		
		gtk_menu_item_remove_submenu(GTK_MENU_ITEM(sven->shell));
		 sven->shell_menu = gtk_menu_new ();
  		gtk_menu_item_set_submenu (GTK_MENU_ITEM (sven->shell), sven->shell_menu);
		gtk_widget_show (sven->shell_menu);
		

      		while ((p = proc_info_scan()) != 0) 
		{
			int space;
			char *temp=NULL;
			GtkTooltips *proc_tooltip;
			p->mem/=1024;
			if(sven->mix->procces_view==1)
				if(p->uid!=geteuid())
					continue;
		
			space=(16-strlen(p->short_cmd));

			if(space>=0 && space<=2 )
			{
				temp=g_strdup_printf("%s   %.1fMb",p->short_cmd,p->mem);
			}
			else if(space>=2 && space<=4 )
			{
				temp=g_strdup_printf("%s \t%.1fMb",p->short_cmd,p->mem);
			}
			else if(space>=4 && space<=7)
			{
				temp=g_strdup_printf("%s \t\t%.1fMb",p->short_cmd,p->mem);
			}
			else if(space>=7 && space<=12)
			{
				temp=g_strdup_printf("%s \t\t\t%.1fMb",p->short_cmd,p->mem);
			}
			else if(space>=12 && space<=17)
			{
				temp=g_strdup_printf("%s \t\t\t\t%.1fMb",p->short_cmd,p->mem);
			}
			else
			{
				temp=g_strdup_printf("%s %.1fMb",p->short_cmd,p->mem);
			}

			
			
			shell_menu_text = gtk_menu_item_new_with_label (temp);
			proc_tooltip = gtk_tooltips_new ();
			temp=g_strdup_printf("User: %s\nPid: %d",p->name,p->pid);
			gtk_tooltips_set_tip (proc_tooltip,GTK_WIDGET (shell_menu_text),temp,NULL);
			gtk_menu_attach (GTK_MENU (sven->shell_menu), shell_menu_text, i,i+1, j, j+1);
			gtk_widget_show (shell_menu_text);
			 
			//gtk_container_add (GTK_CONTAINER (sven->shell_menu), shell_menu_text);
			g_signal_connect(G_OBJECT(shell_menu_text), "activate", G_CALLBACK(menu_kill),(gpointer *) p->pid);
			
			if(i>=1)
			{
				i=0;
				continue;
			}
			i++;
			j++;
		}
		gtk_menu_popup (GTK_MENU (sven->popup_menu), NULL, NULL, NULL,
				NULL, event->button, event->time);
				
		clicked_cb(widget,sven->clipman);
		return 1;
	}
}

gboolean tray_icon_press (GtkWidget *widget, GdkEventButton *event, Sven *sven)
{
	if (event->button == 1 && event->type == GDK_2BUTTON_PRESS)
	{	/* double click  */
		prefswin_dialog_create (sven);
		return 1;
	}
	if (event->button == 1) 
	{
		int no;
	
		no =  (sven->sxkb->cur_group + 1) % sven->sxkb->ngroups;
		XkbLockGroup(GDK_DISPLAY(), XkbUseCoreKbd, no);
	}  

	icon_press_all (widget,event,sven);

	return 0;
}

void menu_prefswin_create(GtkMenuItem     *menuitem,Sven *sven)
{
	if(sven==NULL)
		return;
	
	prefswin_dialog_create (sven);
}

void menu_about_create(GtkMenuItem     *menuitem,Sven *sven)
{
	if(sven==NULL)
		return;
	
	prefswin_dialog_create (sven);
	change_category_num (8);
}

static void menu_cd_play (GtkMenuItem *menuitem,Sven *sven)
{
	select_cd(sven,"Play","");
}
static void menu_cd_prev (GtkMenuItem *menuitem,Sven *sven)
{
	select_cd(sven,"Prev","");
}
static void menu_cd_stop (GtkMenuItem *menuitem,Sven *sven)
{
	select_cd(sven,"Stop","");
}
static void menu_cd_pause (GtkMenuItem *menuitem,Sven *sven)
{
	select_cd(sven,"Pause","");
}
static void menu_cd_next (GtkMenuItem *menuitem,Sven *sven)
{
	select_cd(sven,"Next","");
}
static void menu_cd_eject (GtkMenuItem *menuitem,Sven *sven)
{
	select_cd(sven,"Eject","");
}

static void menu_vol_up (GtkWidget *widget,GdkEventButton *event,Sven *sven)
{
	select_mixer(sven,"VOLUME UP","");
}
static void menu_vol_down (GtkWidget *widget, GdkEventButton *event,Sven *sven)
{
	select_mixer(sven,"VOLUME DOWN","");
}
static void menu_vol_mute (GtkMenuItem *menuitem,Sven *sven)
{
	select_mixer(sven,"VOLUME MUTE","");
}
GtkWidget* create_menu_icon (Sven *sven)
{
	GtkWidget *menu;
	GtkWidget *menuitem,*menuaddsub,*menusub;
	GtkWidget *image;
	
	menu = gtk_menu_new ();
	
	 /*Greate proc info menu*/
	sven->shell = gtk_image_menu_item_new_with_mnemonic (_("Procces"));
	gtk_widget_show (sven->shell);
	gtk_container_add (GTK_CONTAINER (menu), sven->shell);
	
	image = new_pixmap_mod(17,16,16);
	gtk_widget_show (image);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (sven->shell), image);

	sven->shell_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (sven->shell), sven->shell_menu);
	
	/*Greate cd player menu*/
	menuaddsub = gtk_image_menu_item_new_with_mnemonic (_("CD Player"));
	gtk_widget_show (menuaddsub);
	gtk_container_add (GTK_CONTAINER (menu), menuaddsub);
	
	image = new_pixmap_mod(18,16,16);
	gtk_widget_show (image);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuaddsub), image);
	
	menusub = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuaddsub),menusub);
	
		menuitem =gtk_image_menu_item_new_with_mnemonic (_("CD Prev"));
		gtk_container_add(GTK_CONTAINER (menusub),menuitem);
		gtk_widget_show (menuitem);
		image = new_pixmap_mod(20,16,16);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
		g_signal_connect(G_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_cd_prev),sven);
		
		menuitem =gtk_image_menu_item_new_with_mnemonic (_("CD Stop"));
		gtk_container_add(GTK_CONTAINER (menusub),menuitem);
		gtk_widget_show (menuitem);
		image = new_pixmap_mod(21,16,16);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
		g_signal_connect(G_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_cd_stop),sven);
		
		menuitem =gtk_image_menu_item_new_with_mnemonic (_("CD Play"));
		gtk_container_add(GTK_CONTAINER (menusub),menuitem);
		gtk_widget_show (menuitem);
		image = new_pixmap_mod(19,16,16);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
		g_signal_connect(G_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_cd_play),sven);
		
		menuitem =gtk_image_menu_item_new_with_mnemonic (_("CD Pause"));
		gtk_container_add(GTK_CONTAINER (menusub),menuitem);
		gtk_widget_show (menuitem);
		image = new_pixmap_mod(22,16,16);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
		g_signal_connect(G_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_cd_pause),sven);
		
		menuitem =gtk_image_menu_item_new_with_mnemonic (_("CD Next"));
		gtk_container_add(GTK_CONTAINER (menusub),menuitem);
		gtk_widget_show (menuitem);
		image = new_pixmap_mod(23,16,16);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
		g_signal_connect(G_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_cd_next),sven);
		
		menuitem =gtk_image_menu_item_new_with_mnemonic (_("CD Eject"));
		gtk_container_add(GTK_CONTAINER (menusub),menuitem);
		gtk_widget_show (menuitem);
		image = new_pixmap_mod(24,16,16);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
		g_signal_connect(G_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_cd_eject),sven);
	/*End Cd player*/
	
	/*Greate Volume control menu*/
	menuaddsub = gtk_image_menu_item_new_with_mnemonic (_("Volume"));
	gtk_widget_show (menuaddsub);
	gtk_container_add (GTK_CONTAINER (menu), menuaddsub);
	
	image = new_pixmap_mod(28,16,16);
	gtk_widget_show (image);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuaddsub), image);

	menusub = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuaddsub),menusub);
	
		menuitem =gtk_image_menu_item_new_with_mnemonic (_("Volume Up"));
		gtk_container_add(GTK_CONTAINER (menusub),menuitem);
		gtk_widget_show (menuitem);
		image = new_pixmap_mod(25,16,16);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
		g_signal_connect(G_OBJECT(menuitem), "button_press_event", GTK_SIGNAL_FUNC(menu_vol_up),sven);
	
		menuitem =gtk_image_menu_item_new_with_mnemonic (_("Volume Down"));
		gtk_container_add(GTK_CONTAINER (menusub),menuitem);
		gtk_widget_show (menuitem);
		image = new_pixmap_mod(26,16,16);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
		g_signal_connect((gpointer) menuitem, "button_press_event", GTK_SIGNAL_FUNC(menu_vol_down),sven);
		
		menuitem = gtk_separator_menu_item_new ();
		gtk_widget_show (menuitem);
		gtk_container_add (GTK_CONTAINER (menusub), menuitem);
		gtk_widget_set_sensitive (menuitem, FALSE);
		
		menuitem = gtk_image_menu_item_new_with_mnemonic (_("Mute"));
		gtk_widget_show (menuitem);
		gtk_container_add (GTK_CONTAINER (menusub), menuitem);
		g_signal_connect(G_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_vol_mute),sven);
		image = new_pixmap_mod(27,16,16);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
	/*End volume control*/
	
	sven->clipman->submenu = gtk_image_menu_item_new_with_mnemonic (_("Clipboard Manager"));
	gtk_widget_show (sven->clipman->submenu);
	gtk_container_add (GTK_CONTAINER (menu),sven->clipman->submenu);
	
	image = new_pixmap_mod(14,16,16);
	gtk_widget_show (image);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (sven->clipman->submenu), image);

	sven->clipman->menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (sven->clipman->submenu), sven->clipman->menu);
	
	sven->sxkb->submenu = gtk_image_menu_item_new_with_mnemonic (_("Layout XKB"));
	gtk_widget_show (sven->sxkb->submenu);
	gtk_container_add (GTK_CONTAINER (menu),sven->sxkb->submenu);
	
	image = new_pixmap_mod(30,16,16);
	gtk_widget_show (image);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (sven->sxkb->submenu), image);

	menuitem = gtk_separator_menu_item_new ();
	gtk_widget_show (menuitem);
	gtk_container_add (GTK_CONTAINER (menu), menuitem);
	gtk_widget_set_sensitive (menuitem, FALSE);

	menuitem = gtk_image_menu_item_new_from_stock ("gtk-preferences", NULL);
	gtk_widget_show (menuitem);
	gtk_container_add (GTK_CONTAINER (menu), menuitem);
	g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(menu_prefswin_create),sven);

	menuitem = gtk_image_menu_item_new_from_stock ("gtk-dialog-info", NULL);
	gtk_widget_show (menuitem);
	gtk_container_add (GTK_CONTAINER (menu), menuitem);
	g_signal_connect ((gpointer) menuitem, "activate",G_CALLBACK (menu_about_create),sven);

	menuitem = gtk_separator_menu_item_new ();
	gtk_widget_show (menuitem);
	gtk_container_add (GTK_CONTAINER (menu), menuitem);
	gtk_widget_set_sensitive (menuitem, FALSE);
	
	menuitem = gtk_image_menu_item_new_from_stock ("gtk-quit", NULL);
	gtk_widget_show (menuitem);
	gtk_container_add (GTK_CONTAINER (menu), menuitem);
	g_signal_connect ((gpointer) menuitem, "activate",G_CALLBACK (sven_exit),sven);

 	return menu;
}


static gboolean exposed(GtkWidget *w,GdkEventExpose *gee,GdkPixmap *myback)
{
	 gdk_draw_pixmap(w->window,
		  w->style->fg_gc[GTK_WIDGET_STATE (w)],
		  myback,
		  gee->area.x, gee->area.y,
		  gee->area.x, gee->area.y,
		  gee->area.width, gee->area.height);
  return FALSE;
}

static gboolean bmotion(GtkWidget *w,GdkEventMotion *geb,GtkWidget *mainw)
{
	if (geb==NULL)
		return TRUE;

	wx=wx+((int)geb->x_root-dragx);
	wy=wy+((int)geb->y_root-dragy);

	dragx=(int)geb->x_root;
	dragy=(int)geb->y_root;
	//printf("bmotion: %d,%d\n",wx,wy);
  
	gdk_window_move(mainw->window,wx,wy);
	gdk_flush();
	
	return FALSE;
}

static gboolean tray_destroyed (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	init_tray((Sven *) user_data);
	printf("Destroy tray icon\n");
	return TRUE;
}

void init_tray (Sven *sven)
{
	GtkWidget *evbox;
        
	 /* Popup menu */
	 if(sven->popup_menu==NULL)
		sven->popup_menu= create_menu_icon (sven);
	
	if(sven->tray_icon_tooltip==NULL)
		sven->tray_icon_tooltip = gtk_tooltips_new ();
	
	if(sven->tray_icon==NULL && sven->mix->icon_check==1)
	{
		/* Event box */
		evbox = gtk_event_box_new ();
		gtk_event_box_set_visible_window(GTK_EVENT_BOX(evbox),0);
		gtk_widget_set_size_request (evbox, 25, 20);
		
		DEBUG_MSG("Start tray icon\n");
		sven->tray_icon = egg_tray_icon_new (_("Sven Multimedia Keys daemon"));
		gtk_widget_set_size_request (GTK_WIDGET(sven->tray_icon), 25, 20);
		
		gtk_tooltips_set_tip (sven->tray_icon_tooltip,GTK_WIDGET (sven->tray_icon),_("Sven Multimedia Keys daemon active"),NULL);
			
		g_signal_connect (G_OBJECT (sven->tray_icon), "button_press_event",G_CALLBACK (tray_icon_press),sven);
		g_signal_connect (G_OBJECT (sven->tray_icon), "button_release_event",G_CALLBACK (tray_icon_release),sven);
		g_signal_connect(G_OBJECT(sven->tray_icon), "destroy",G_CALLBACK (tray_destroyed), (gpointer) sven);
		
		sven->tray_flag = new_pixmap_mod(1,24,24);
		gtk_container_add(GTK_CONTAINER(evbox),sven->tray_flag);
		gtk_container_add (GTK_CONTAINER (sven->tray_icon), evbox);
		gtk_widget_show_all (GTK_WIDGET (sven->tray_icon));
	}
	else if(sven->tray_icon!=NULL && sven->mix->icon_check==0)
	{
		egg_tray_icon_destroy(GTK_WIDGET(sven->tray_icon));
		sven->tray_icon=NULL;
	}
	
}

void init_icon (Sven *sven)
{
	
	if(sven->mix->icon_check_desk==1 && sven->wininfo->info_win==NULL)
	{
		create_wininfo (sven);
	}
	
	else if(sven->wininfo->info_win!=NULL && sven->mix->icon_check_desk==0)
	{
		gtk_widget_hide_all(GTK_WIDGET(sven->wininfo->info_win));
	}
	else if(sven->wininfo->info_win!=NULL && sven->mix->icon_check_desk==1)
	{
		gtk_widget_show_all(GTK_WIDGET(sven->wininfo->info_win));
	}

	/*gdk_draw_text(myback,gtk_style_get_font(icon_win->style), icon_win->style->fg_gc[GTK_WIDGET_STATE (icon_win)],1,10,"texter",6);*/
}

void restart_tray (Sven *sven)
{
	if(sven->mix->icon_check==1)
		init_tray (sven);
	else if(sven->mix->icon_check==0 && sven->tray_icon!=NULL )
	{
		egg_tray_icon_destroy(GTK_WIDGET(sven->tray_icon));
		sven->tray_icon=NULL;
	}
}
void restart_icon (Sven *sven)
{
	if(sven->mix->icon_check_desk==1)
		if(GTK_WIDGET(sven->wininfo->info_win)==NULL)
			init_icon (sven);
		else
			gtk_widget_show_all (GTK_WIDGET(sven->wininfo->info_win));
	else if(sven->mix->icon_check_desk==0 && GTK_WIDGET(sven->wininfo->info_win)!=NULL)
		gtk_widget_hide_all(GTK_WIDGET(sven->wininfo->info_win));
}

proc_info * proc_info_scan(void)
{
	static DIR *dir;
	FILE *fp;
	struct dirent *entry;
	static proc_info ret_status;
	proc_info curstatus;
	char *name;
	char status[32];
	gchar buffer[64];
	int pid;
	struct stat sb;


	if (!dir) 
	{
		dir = opendir("/proc");
		if(!dir)
			printf("Can't open /proc");
	}
	for(;;) 
	{
		if((entry = readdir(dir)) == NULL) 
		{
			closedir(dir);
			dir = 0;
			return 0;
		}
		name = entry->d_name;
		if (!(*name >= '0' && *name <= '9'))
			continue;

		memset(&curstatus, 0, sizeof(proc_info));
		pid = atoi(name);
		curstatus.pid = pid;
		
		sprintf(status, "/proc/%d", pid);
		if(stat(status, &sb))
			continue;
	
		curstatus.uid=sb.st_uid;
			
		struct passwd *myuser = getpwuid(sb.st_uid);
		curstatus.name=myuser->pw_name;
		
		sprintf(status, "/proc/%d/status", pid);	
		if((fp = fopen(status, "r")) == NULL)
			continue;
			
		while (fgets(buffer, 128, fp)) 
		{
			gchar *buf = buffer;
			buf = g_strstrip(buf);
		
			if(!strncmp(buf, "VmSize", 6))
				get_int_val(curstatus.mem);
			if(!strncmp(buf, "Name", 4))
				get_str_val(curstatus.short_cmd);
		}
		fclose(fp);
		
		return memcpy(&ret_status, &curstatus, sizeof(proc_info));
	}
}


void load_startup_prog(Sven *sven)
{
	if(sven->mix->startup_prog_check==1)
	{
		gchar *temp, *temp2;
		if(sven->mix->startup_prog==NULL)
			return; 
		
		if(sven->mix->startup_prog=="")
			return; 
		
		if(g_strcasecmp(sven->mix->startup_prog," ")==-1);
			return; 
		
		if(g_strcasecmp(sven->mix->startup_prog,"")!=0);
		{
			temp = g_strdup(sven->mix->startup_prog);
			while ((temp2 = strchr(temp, ';')) != NULL)
			{
				(*temp2) = '\0';
				execute (temp, FALSE);
				temp = temp2 + 1;
			}
			execute (temp, FALSE);;
		}
		free(temp);
		free(temp2);
	}
}
