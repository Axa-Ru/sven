/* Sven multimedia keyboard deamon
 * utils.c
 *
 *Copyright (C) 2005 Eugene Morenko(More)
 *mailto: morenko@ukr.net
 *web: http://sven.linux.kiev.ua/
 *icq:325697438
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
#include <X11/Xatom.h>
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


#include "sven.h"
#include "utils.h"
#include "plugin.h"

#include "key.h"
#include "mouse.h"

#define DIR_MODE (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)	/* same as 0755 */
void file_check_directory(char *dir) 
{
	gchar *rcdir = g_strconcat(g_get_home_dir(),dir, NULL);
	
	if (!g_file_test(rcdir, G_FILE_TEST_IS_DIR))
		mkdir(rcdir, DIR_MODE);
	
	g_free(rcdir);
}

void sven_read_config(Sven *sven)
{
	gchar *filename;
	sven->config=g_slice_new0(ConfigSven);
	
	
	/* Read all sven config [file:config] */
	filename= g_strconcat(g_get_home_dir(),"/.sven/config", NULL);
	sven->config->all = sven_cfg_open_file(filename);
	if (!sven->config->all)
		sven->config->all = sven_cfg_new();
	g_free(filename);
	
	/* Read key config [file:key_config] */
	filename= g_strconcat(g_get_home_dir(),"/.sven/key_config", NULL);
	if (!g_file_test(filename, G_FILE_TEST_EXISTS)) 
	{
		g_free(filename);
		filename= g_strconcat(PKGDATADIR,"/key_config", NULL);
	}
	sven->config->key = sven_cfg_open_file(filename);
	if (!sven->config->key)
		sven->config->key = sven_cfg_new();
	g_free(filename);
	
	/* Read mouse config [file:mouse_config] */
	filename= g_strconcat(g_get_home_dir(),"/.sven/mouse_config", NULL);
	if (!g_file_test(filename, G_FILE_TEST_EXISTS)) 
	{
		g_free(filename);
		filename= g_strconcat(PKGDATADIR,"/mouse_config", NULL);
	}
	sven->config->mouse = sven_cfg_open_file(filename);
	if (!sven->config->mouse)
		sven->config->mouse = sven_cfg_new();
	g_free(filename);
}

void sven_write_config (Sven *sven)
{
	gchar *filename;

	/* Write all sven config [file:config] */
	filename= g_strconcat(g_get_home_dir(),"/.sven/config", NULL);
	sven_cfg_write_file(sven->config->all, filename);
	sven_cfg_free(sven->config->all);
	g_free(filename);

	
	/* Write key config [file:key_config] */
	filename= g_strconcat(g_get_home_dir(),"/.sven/key_config", NULL);
	sven_cfg_write_file(sven->config->key, filename);
	sven_cfg_free(sven->config->key);
	g_free(filename);

	/* Write mouse config [file:mouse_config] */
	filename= g_strconcat(g_get_home_dir(),"/.sven/mouse_config", NULL);
	sven_cfg_write_file(sven->config->mouse, filename);
	sven_cfg_free(sven->config->mouse);
	g_free(filename);
}

void sven_exit (GtkMenuItem *menuitem,Sven *sven)
{
	update_kbd(sven);
	update_mouse(sven);
	sven_plugin_shutdown(sven);
	sven_write_config(sven);
	gtk_main_quit();
	exit (1);
}
/*******************************************************************************************/
/* UI Info, message, confirm, error window*/


GtkWidget* xfce_create_mixed_button (const gchar *stock, const gchar *text);
gboolean xfce_confirm (const gchar *text, const gchar *stock_id,const gchar *action);

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
	GLADE_HOOKUP_OBJECT_NO_REF (dlg, dlg, "dlg");
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

/*****************************************************************************************************/

void execute (char *cmd, gboolean sync)
{
	gboolean retval;
//	char *cmd_new;

//	cmd_new = g_strdup_printf("%s &\n",cmd);
	
//	if (sync != FALSE)
		retval = g_spawn_command_line_async (cmd, NULL);
	/*else
		retval = g_spawn_command_line_sync (cmd, NULL, NULL, NULL, NULL);
*/
/*	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", cmd_new, NULL);
		exit (EXIT_SUCCESS);
	}
	
	g_free(cmd_new);	
*/	
	if (retval == FALSE)
	{
		char *msg;

		msg = g_strdup_printf
			(_("Couldn't execute command: \"<span weight=\"bold\" color=\"red\">%s</span>\"\n"
			   "Verify that this command exists.\n"),cmd);
		show_error(msg);
		printf("[Sven:execute][ERROR]:%s",msg);
		g_free (msg);
	}
}

/*****************************************************************************************************************************/

GtkWidget *create_header (const char *text)
{
	GtkWidget *eventbox, *label;
	GtkStyle *style;
	char *markup;
	GdkColor color;

	eventbox = gtk_event_box_new ();
	gtk_widget_show (eventbox);
	
	gdk_color_parse("#4A79CD",&color);
	gtk_widget_modify_bg (eventbox, GTK_STATE_NORMAL,&color);

	markup = g_strconcat ("<span size=\"larger\"  weight=\"bold\">", _(text),"</span>", NULL);
	label = gtk_label_new (markup);
	g_free (markup);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	//gtk_misc_set_alignment (GTK_MISC (label),0,5);

	gtk_widget_show (label);
	gtk_container_add (GTK_CONTAINER (eventbox), label);

	style = gtk_widget_get_style (label);
	gtk_widget_modify_fg (label, GTK_STATE_NORMAL,&style->fg[GTK_STATE_SELECTED]);

	return eventbox;
}

