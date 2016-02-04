/*
 * Copyright (c) 2004 Eduard Roccatello (eduard@xfce.org)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "sven.h"
#include "utils.h"
#include "clipman.h"
#include "wininfo.h"

static GtkClipboard *primaryClip, *defaultClip;

static void resetTimer (gpointer data);
static gboolean isThere (t_clipman *clip, gchar *txt);
static gchar* filterLFCR (gchar *txt);


static gboolean isThere (t_clipman *clip, gchar *txt)
{
    gint i;
    for (i=0; i<MAXHISTORY; i++) {
        if (strcmp(clip->content[i]->str, txt) == 0)
            return TRUE;
    }
    return FALSE;
}

static gchar* filterLFCR (gchar *txt)
{
    guint i = 0;
    while (txt[i] != '\0') {
        if (txt[i] == '\n' || txt[i] == '\r' || txt[i] == '\t')
            txt[i] = ' ';
        i++;
    }
    txt = g_strstrip(txt);
    return txt;
}

static void
item_pressed_cb (GtkWidget *widget, GdkEventButton *ev, gpointer user_data)
{
    t_action *act = (t_action *)user_data;
    if (ev->button != 3) {
        gtk_clipboard_set_text(defaultClip, act->clip->content[act->idx]->str, -1);
        gtk_clipboard_set_text(primaryClip, act->clip->content[act->idx]->str, -1);
    } else {
        if (confirm(_("Do you want to remove it from the hystory?"), "gtk-clear", NULL)) {
            gtk_clipboard_set_text(defaultClip, "", -1);
            gtk_clipboard_set_text(primaryClip, "", -1);
            g_string_assign(act->clip->content[act->idx], "");
            act->clip->iter = act->idx;
        }
        gtk_menu_popdown(GTK_MENU(act->clip->menu));
    }
    gtk_widget_destroy (act->clip->menu);
}

static void
drag_data_get_cb (GtkWidget *widget, GdkDragContext *dg, GtkSelectionData *seldata, gint i, gint t, gpointer user_data)
{
    t_action *action = user_data;
    gint idx = action->idx;
    gchar *text = action->clip->content[idx]->str;
    gtk_selection_data_set(seldata, gdk_atom_intern("STRING", FALSE), 8, text, strlen(text));
}


static void
clearClipboard (GtkWidget *widget, gpointer data)
{
    gint i;
    t_clipman *clipman = (t_clipman *)data;

    /* Clear History */
    for (i=0; i<MAXHISTORY; i++)
        g_string_assign(clipman->content[i], "");

    /* Clear Clipboard */
    gtk_clipboard_set_text(defaultClip, "", -1);
    gtk_clipboard_set_text(primaryClip, "", -1);

    /* Set iterator to the first element of the array */
    clipman->iter = 0;

}

void clicked_cb(GtkWidget *button, gpointer data)
{
    //GtkMenu    *menu = NULL;
    GtkWidget  *mi;
    GtkTargetEntry *te;
    t_clipman  *clipman = data;
    t_action   *action = NULL;
    gboolean    hasOne = FALSE;
    gint        i;                  /* an index */
    guint       last;               /* latest item inserted */
    guint       num = 0;            /* just a counter */

    te = g_new0(GtkTargetEntry,1);
    te->target="UTF8_STRING";
    te->flags=0;
    te->info=0;

	gtk_menu_item_remove_submenu(GTK_MENU_ITEM(clipman->submenu));
	clipman->menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (clipman->submenu), clipman->menu);
	
	gtk_widget_show (clipman->menu);
    //menu = GTK_MENU(gtk_menu_new());

    mi = gtk_menu_item_new_with_label ("  Clipboard History");
    gtk_widget_show (mi);
    gtk_widget_set_sensitive (mi, FALSE);
    //gtk_menu_shell_append (GTK_MENU_SHELL (clipman->menu), mi);
	gtk_container_add (GTK_CONTAINER (clipman->menu), mi);

    mi = gtk_separator_menu_item_new ();
    gtk_widget_show (mi);
    gtk_widget_set_sensitive (mi, FALSE);
    //gtk_menu_shell_append (GTK_MENU_SHELL (clipman->menu), mi);
	gtk_container_add (GTK_CONTAINER (clipman->menu), mi);

    /*
        Append cliboard history items.
        I need to have an circular array scan. Latest item inserted in the array
        is at iter-1 position so we need a special trick to scan the array
    */

    if (clipman->iter!=0)
        last=clipman->iter-1;
    else
        last=MAXHISTORY-1;

    for (i=last;i>=0;i--){
        if (clipman->content[i]->str != NULL && (strcmp(clipman->content[i]->str, "") != 0)) {
            mi = gtk_menu_item_new_with_label (g_strdup_printf("%d. %s", ++num, filterLFCR(g_strndup(clipman->content[i]->str, MENUTXT_LEN))));
            gtk_drag_source_set(mi, GDK_BUTTON1_MASK, te, 1, GDK_ACTION_COPY | GDK_ACTION_MOVE);
            gtk_widget_show (mi);
            action = g_new(t_action, 1);
            action->clip = clipman;
            action->idx = i;
            g_signal_connect (G_OBJECT (mi), "drag_data_get", G_CALLBACK (drag_data_get_cb), (gpointer)action);
            g_signal_connect (G_OBJECT(mi), "button_release_event", G_CALLBACK(item_pressed_cb), (gpointer)action);
            //gtk_menu_shell_append (GTK_MENU_SHELL (clipman->menu), mi);
	gtk_container_add (GTK_CONTAINER (clipman->menu), mi);
            hasOne = TRUE;
        }
    }

    if (last!=MAXHISTORY-1) {
        for (i=MAXHISTORY-1;i>last;i--) {
            if (clipman->content[i]->str != NULL && (strcmp(clipman->content[i]->str, "") != 0)) {
                mi = gtk_menu_item_new_with_label (g_strdup_printf("%d. %s", ++num, filterLFCR(g_strndup(clipman->content[i]->str, 20))));
                gtk_widget_show (mi);
                action = g_new(t_action, 1);
                action->clip = clipman;
                action->idx = i;
                g_signal_connect (G_OBJECT(mi), "button_release_event", G_CALLBACK(item_pressed_cb), (gpointer)action);
                g_signal_connect (G_OBJECT (mi), "drag_data_get", G_CALLBACK (drag_data_get_cb), (gpointer)action);
               //gtk_menu_shell_append (GTK_MENU_SHELL (clipman->menu), mi);
	gtk_container_add (GTK_CONTAINER (clipman->menu), mi);
            }
        }
    }

    /* If the clipboard is empty put a new informational item, else create the clear item */
    if (!hasOne) {
        mi = gtk_menu_item_new_with_label ("< Clipboard Empty >");
        gtk_widget_show (mi);
        gtk_widget_set_sensitive (mi, FALSE);
       //gtk_menu_shell_append (GTK_MENU_SHELL (clipman->menu), mi);
	gtk_container_add (GTK_CONTAINER (clipman->menu), mi);
    }
	else {

        mi = gtk_separator_menu_item_new ();
        gtk_widget_show (mi);
        gtk_widget_set_sensitive (mi, FALSE);
        gtk_menu_shell_append (GTK_MENU_SHELL (clipman->menu), mi);

        mi = gtk_menu_item_new_with_label ("Clear Clipboard");
        gtk_widget_show (mi);
        g_signal_connect (G_OBJECT (mi), "activate", G_CALLBACK (clearClipboard), (gpointer)clipman);
        //gtk_menu_shell_append (GTK_MENU_SHELL (clipman->menu), mi);
	gtk_container_add (GTK_CONTAINER (clipman->menu), mi);
	}

   // clipman->menu = GTK_WIDGET(menu);
   // gtk_menu_popup (clipman->menu, NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
}

gchar* get_text_select () {
    gchar *txt = NULL;
    
    /* Check for text in X clipboard */
    txt = gtk_clipboard_wait_for_text (primaryClip);
    if (txt != NULL) {
    return txt;
     //  printf("TEXT:%s\n",locale_from_utf8(txt));
        g_free(txt);
        txt = NULL;
    }

   
    return NULL;
}

gboolean checkClip (t_clipman *clipman) {
    gchar *txt = NULL;
    
    /* Check for text in X clipboard */
    txt = gtk_clipboard_wait_for_text (primaryClip);
    if (txt != NULL) {
        if (!isThere(clipman, txt)) {
                g_string_assign(clipman->content[clipman->iter], txt);
                if (clipman->iter < (MAXHISTORY - 1))
                    clipman->iter++;
                else
                    clipman->iter = 0;
        }
        g_free(txt);
        txt = NULL;
    }

    /* Check for text in default clipboard */
    txt = gtk_clipboard_wait_for_text (defaultClip);
    if (txt != NULL) {
        if (!isThere(clipman, txt)) {
            g_string_assign(clipman->content[clipman->iter], txt);
            if (clipman->iter < (MAXHISTORY - 1))
                clipman->iter++;
            else
                clipman->iter = 0;
        }

        g_free(txt);
        txt = NULL;
    }

    return TRUE;
}

static void resetTimer (gpointer data)
{
    t_clipman *clipman = (t_clipman *)data;
    if (!(clipman->killing)) {
        if (clipman->timeId != 0)
            g_source_remove(clipman->timeId);
        clipman->timeId = g_timeout_add_full(G_PRIORITY_LOW, CLIPMAN_TIMER_INTERVAL, (GSourceFunc)checkClip, data, (GDestroyNotify)resetTimer);
    }
}
void clipman_new(Sven *sven)
{

        gint       i;
	
	sven->clipman = g_new(t_clipman, 1);
    
    /* Element to be modified */
    sven->clipman->iter = 0;
	sven->clipman->timeId = 0;
    sven->clipman->killing = FALSE;

    for (i=0; i<MAXHISTORY; i++) {
        sven->clipman->content[i] = g_string_new("");
    }
    defaultClip = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
    primaryClip = gtk_clipboard_get (GDK_SELECTION_PRIMARY);

    sven->clipman->timeId = g_timeout_add_full(G_PRIORITY_LOW, CLIPMAN_TIMER_INTERVAL, (GSourceFunc)checkClip, sven->clipman, (GDestroyNotify)resetTimer);
}
