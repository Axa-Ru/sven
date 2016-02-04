/* Sven multimedia keyboard deamon
 * configplugin.c
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
#include "configplugin.h"
#include "plugin.h"


	GtkWidget *plugins_tree_view;
	GtkTreeStore *plugins_model;
	GObject *plugins_selection;
	GtkWidget *prefs_plugins_config_wid;
	GtkWidget *prefs_plugins_about_wid;

static void prefs_plugins_list_clicked(GtkTreeSelection *plugins_select,Sven *sven)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	SvenPlugin *splg;
	
	if (gtk_tree_selection_get_selected(plugins_select, &model, &iter)) 
	{
		gchar *modul;
		gtk_tree_model_get(model, &iter, 2, &modul, -1);

		splg = g_slist_nth(sven->plugins,plugin_find_pos(sven,modul))->data;
		if(splg!=NULL)
		{

			if (splg && splg->configure)
				gtk_widget_set_sensitive(prefs_plugins_config_wid, 1);
			else
				gtk_widget_set_sensitive(prefs_plugins_config_wid, 0);
	
			if (splg && splg->about)
				gtk_widget_set_sensitive(prefs_plugins_about_wid, 1);
			else
				gtk_widget_set_sensitive(prefs_plugins_about_wid, 0);
		}
	}
	else
	{
		gtk_widget_set_sensitive(prefs_plugins_config_wid,FALSE);
		gtk_widget_set_sensitive(prefs_plugins_about_wid,FALSE);
	}
}

static void print_plugins_list (GtkTreeIter *iter,gchar *name,gchar *module,gint enable,gint number)
{
	gtk_tree_store_set (plugins_model, iter, 0,number,1,name,2,module, -1);
}


static void add_print_plugins(GtkTreeView *plugins_tree_view,Sven *sven)
{
	GtkTreeIter iter;
	GtkTreeIter selected;
	GtkTreeModel *model;
	GSList *glist = sven->plugins;
	SvenPlugin *splg;
	gint i = 0;

	 gtk_tree_store_clear(GTK_TREE_STORE (plugins_model));

	while (glist)
	{
		splg = (SvenPlugin *) glist->data;

		model = gtk_tree_view_get_model (plugins_tree_view);
		if (gtk_tree_selection_get_selected (gtk_tree_view_get_selection (GTK_TREE_VIEW (plugins_tree_view)), NULL, &selected))
		{
			if (GTK_IS_TREE_STORE (model))
			{
				gtk_tree_store_append (GTK_TREE_STORE (model), &iter, &selected);
				print_plugins_list (&iter,splg->name,splg->filename,i,i+1);
			}
		}
		else
		{
			if (GTK_IS_TREE_STORE (model))
			{
				  gtk_tree_store_append (GTK_TREE_STORE (model), &iter, NULL);
				  print_plugins_list (&iter,splg->name,splg->filename,i,i+1);
			}
			glist = glist->next;
			i++;
 		}
	}
	gtk_widget_set_sensitive(prefs_plugins_config_wid, 0);
	gtk_widget_set_sensitive(prefs_plugins_about_wid, 0);
}

static void prefs_plugins_configure(gpointer data,Sven *sven)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	SvenPlugin *splg;
	
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION (plugins_selection), &model, &iter)) 
	{
		gchar *modul;
		gtk_tree_model_get(model, &iter, 2, &modul, -1);

		splg = g_slist_nth(sven->plugins,plugin_find_pos(sven,modul))->data;
		if (splg->configure!=NULL)
		splg->configure(sven);
	}
}
static void prefs_plugins_about(GtkButton * w,Sven *sven)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	SvenPlugin *splg;
	
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION (plugins_selection), &model, &iter)) 
	{
		gchar *modul;
		gtk_tree_model_get(model, &iter, 2, &modul, -1);

		splg = g_slist_nth(sven->plugins,plugin_find_pos(sven,modul))->data;
		if (splg->about!=NULL)
		splg->about();
	}
}

static gboolean plugin_list_event_mouseclick(GtkWidget *widget,GdkEventButton *event,Sven *sven) 
{
	if (event->button == 1 && event->type == GDK_2BUTTON_PRESS) 
	{	// double click  
		prefs_plugins_configure(NULL,sven);
		return TRUE;
	}
	return FALSE;
}

void create_plugins_gui(Sven *sven,GtkWidget *vbox1)
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell;
	GtkWidget *scrolled_win;
	GtkWidget *hbbox;
	GtkWidget *hbox,*key_page_label;

	DEBUG_MSG("Start create_plugins_gui\n");
	
	key_page_label = gtk_label_new (g_strdup_printf("<span size=\"xx-large\" ><b><u><i>%s\n</i></u></b></span>",_("Plugins")));
	gtk_widget_show (key_page_label);
	gtk_box_pack_start (GTK_BOX (vbox1), key_page_label, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (key_page_label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (key_page_label), 0, 0);
  

	plugins_model = gtk_tree_store_new (3, G_TYPE_INT, G_TYPE_STRING,G_TYPE_STRING);
	plugins_tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (plugins_model));
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (plugins_tree_view), TRUE);
	plugins_selection = G_OBJECT (gtk_tree_view_get_selection (GTK_TREE_VIEW (plugins_tree_view)));
	gtk_tree_selection_set_mode (GTK_TREE_SELECTION (plugins_selection), GTK_SELECTION_SINGLE);

	g_signal_connect (G_OBJECT (plugins_selection),"changed",G_CALLBACK (prefs_plugins_list_clicked),sven);
	g_signal_connect(G_OBJECT(plugins_tree_view), "button-press-event",G_CALLBACK(plugin_list_event_mouseclick), sven);

	scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled_win),  plugins_tree_view);
	gtk_container_border_width(GTK_CONTAINER(scrolled_win), 5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolled_win, TRUE, TRUE, 0);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox, FALSE, FALSE, 5);

	hbbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbbox), GTK_BUTTONBOX_START);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(hbbox), 10);
	gtk_button_box_set_child_size(GTK_BUTTON_BOX(hbbox), 85, 17);
	gtk_box_pack_start(GTK_BOX(hbox), hbbox, TRUE, TRUE, 0);

	prefs_plugins_config_wid = gtk_button_new_with_label(_("Configure"));
	g_signal_connect(G_OBJECT(prefs_plugins_config_wid ), "clicked", G_CALLBACK(prefs_plugins_configure),sven);
	gtk_box_pack_start(GTK_BOX(hbbox), prefs_plugins_config_wid, TRUE, TRUE, 0);

	prefs_plugins_about_wid = gtk_button_new_with_label(_("About"));
	g_signal_connect(G_OBJECT(prefs_plugins_about_wid), "clicked", G_CALLBACK(prefs_plugins_about), sven);

	gtk_box_pack_start(GTK_BOX(hbbox), prefs_plugins_about_wid, TRUE, TRUE, 0);
	 
	 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Number"), cell, "text", 0, NULL);
		  gtk_tree_view_column_set_sort_column_id (column, 0);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (plugins_tree_view), column);

		 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Name"), cell, "markup",1, NULL);
		  gtk_tree_view_column_set_sort_column_id (column, 1);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (plugins_tree_view), column);

		 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Module"), cell, "markup", 2, NULL);
		  gtk_tree_view_column_set_sort_column_id (column, 2);
		 gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (plugins_tree_view), column);

	add_print_plugins(GTK_TREE_VIEW (plugins_tree_view),sven);
}
