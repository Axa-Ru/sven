/* Sven multimedia keyboard deamon
 * configplugin.c
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: morenko@ukr.net
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
#include "../sven.h"
#include "../utils.h"
#include "configplugin.h"
#include "../plugin.h"
#include "prefswin.h"


static GtkWidget *plugins_tree_view;
static GtkTreeStore *plugins_model;
static GObject *plugins_selection;
static GtkWidget *prefs_plugins_config_wid;
static GtkWidget *prefs_plugins_about_wid;
static	GtkWidget *infotext;

static void prefs_plugins_list_clicked(GtkTreeSelection *plugins_select,Sven *sven)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected(plugins_select, &model, &iter)) 
	{
		gchar *modul;
		gtk_tree_model_get(model, &iter,2, &modul, -1);
		SvenPluginPriv * plug=sven_plugin_find(sven,modul);
		if((plug!=NULL)&&(plug->func!=NULL))
		{
			if ((plug->func->configure!=NULL)&&(plug->enable==1))
				gtk_widget_set_sensitive(prefs_plugins_config_wid, 1);
			else
				gtk_widget_set_sensitive(prefs_plugins_config_wid, 0);
			
			if ((plug->func->about!=NULL)&&(plug->enable==1))
				gtk_widget_set_sensitive(prefs_plugins_about_wid, 1);
			else
				gtk_widget_set_sensitive(prefs_plugins_about_wid, 0);
				
			gtk_label_set_markup(GTK_LABEL(infotext),plug->info);
		}
		else
		{
			gtk_widget_set_sensitive(prefs_plugins_config_wid,FALSE);
			gtk_widget_set_sensitive(prefs_plugins_about_wid,FALSE);
			gtk_label_set_markup(GTK_LABEL(infotext),plug->info);
		}
	}
	else
	{
		gtk_widget_set_sensitive(prefs_plugins_config_wid,FALSE);
		gtk_widget_set_sensitive(prefs_plugins_about_wid,FALSE);
		gtk_label_set_markup(GTK_LABEL(infotext),_("Not information"));
	}
}

static void print_plugins_list (GtkTreeIter *iter,gchar *name,gchar *filename,gint enable)
{
	gtk_tree_store_set (plugins_model, iter, 0,enable,1,name,2,filename, -1);
}


static void add_print_plugins(GtkTreeView *plugins_tree_view,Sven *sven)
{
	GtkTreeIter iter;
	GtkTreeIter selected;
	GtkTreeModel *model;
	GSList *l = NULL;
	
	 gtk_tree_store_clear(GTK_TREE_STORE (plugins_model));
	 
	for (l = sven->plugin->list; l; l=l->next)
	{
		SvenPluginPriv *plug = (SvenPluginPriv *)l->data;
		if((plug!=NULL))
		{
		
			model = gtk_tree_view_get_model (plugins_tree_view);
			if (gtk_tree_selection_get_selected (gtk_tree_view_get_selection (GTK_TREE_VIEW (plugins_tree_view)), NULL, &selected))
			{
				if (GTK_IS_TREE_STORE (model))
				{
					gtk_tree_store_append (GTK_TREE_STORE (model), &iter, NULL);
					print_plugins_list (&iter,plug->name,plug->filename,plug->enable);
				}
			}
			else
			{
				if (GTK_IS_TREE_STORE (model))
				{
					gtk_tree_store_append (GTK_TREE_STORE (model), &iter, NULL);
					print_plugins_list (&iter,plug->name,plug->filename,plug->enable);
				}
			}
		}
	}

	gtk_widget_set_sensitive(prefs_plugins_config_wid, 0);
	gtk_widget_set_sensitive(prefs_plugins_about_wid, 0);
	gtk_label_set_markup(GTK_LABEL(infotext),_("Not information"));
}

static void prefs_plugins_configure(gpointer data,Sven *sven)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION (plugins_selection), &model, &iter)) 
	{
		gchar *modul;
		gtk_tree_model_get(model, &iter,2, &modul, -1);

		SvenPluginPriv * plug=sven_plugin_find(sven,modul);
		if((plug!=NULL)&&(plug->func!=NULL))
		{
			if ((plug->func->configure!=NULL)&&(plug->enable==1))
			{
				plug->func->configure(sven,plug->func->private_data);
			}
		}
	}
}
static void prefs_plugins_about(GtkButton * w,Sven *sven)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION (plugins_selection), &model, &iter)) 
	{
		gchar *modul;
		gtk_tree_model_get(model, &iter,2, &modul, -1);

		SvenPluginPriv * plug=sven_plugin_find(sven,modul);
		if((plug!=NULL)&&(plug->func!=NULL))
		{
			if ((plug->func->about!=NULL)&&(plug->enable==1))
			{
				plug->func->about(plug->func->private_data);
			}
		}
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

static void plugin_fixed_toggled (GtkCellRendererToggle *cell,gchar *path_str,gpointer  data)
{
	Sven *sven = (Sven *)data;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	GtkTreeIter iter;
	gboolean toggle_item;
	gchar *filename;
	
	/* get toggled iter */
	gtk_tree_model_get_iter (GTK_TREE_MODEL (plugins_model), &iter, path);
	gtk_tree_model_get (GTK_TREE_MODEL (plugins_model), &iter,0, &toggle_item, 2, &filename, -1);
	
	/* do something with the value */
	toggle_item ^= 1;
	
	 sven_plugin_load(sven,filename,toggle_item);
	
	 prefs_plugins_list_clicked(G_OBJECT (plugins_selection),sven);
	 
	/* set new value */
	gtk_tree_store_set (GTK_TREE_STORE (GTK_TREE_MODEL (plugins_model)), &iter,0,
			toggle_item, -1);
	
	/* clean up */
	gtk_tree_path_free (path);
}


void create_plugins_gui(Sven *sven,GtkWidget *vbox1)
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell;
	GtkWidget *scrolled_win;
	GtkWidget *hbbox;
	GtkWidget *hbox;
	GtkWidget *frame;
	GtkWidget *alignment;
	GtkWidget *label;

	DEBUG_MSG("Start create_plugins_gui\n");

	gtk_box_pack_start (GTK_BOX (vbox1), create_header(_("Plugins")), FALSE, FALSE, 0);
  

	plugins_model = gtk_tree_store_new (3,G_TYPE_BOOLEAN,G_TYPE_STRING,G_TYPE_STRING);
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

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start(GTK_BOX(vbox1), frame, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.05, 0.5);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	
	label = gtk_label_new (_("Description"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 2);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 5, 5, 5, 5);

	infotext = gtk_label_new (_("Not information"));
	gtk_widget_show (infotext);
	gtk_container_add (GTK_CONTAINER (alignment), infotext);
	GTK_WIDGET_SET_FLAGS (infotext, GTK_CAN_FOCUS);
	gtk_label_set_line_wrap (GTK_LABEL (infotext), TRUE);
	gtk_label_set_selectable (GTK_LABEL (infotext), TRUE);
	
	
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
	 
           	cell = gtk_cell_renderer_toggle_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Status"), cell, "active",0, NULL);
		  gtk_tree_view_column_set_clickable (column, TRUE);
		  g_signal_connect (G_OBJECT (cell), "toggled",G_CALLBACK (plugin_fixed_toggled),sven);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (plugins_tree_view), column);
		  
	        cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Name"), cell, "markup", 1, NULL);
		  gtk_tree_view_column_set_sort_column_id (column, 0);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (plugins_tree_view), column);

		 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Module"), cell, "markup",2, NULL);
		  gtk_tree_view_column_set_sort_column_id (column, 1);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (plugins_tree_view), column);


		  gtk_widget_show(plugins_tree_view);

	add_print_plugins(GTK_TREE_VIEW (plugins_tree_view),sven);
}


void config_plugin_save (Sven *sven)
{
	gchar *filename;
	GtkTreeModel *model;
	GtkTreeIter  iter;
	GtkTreePath *path;

	if(sven==NULL)
		return;
	
	filename= g_strconcat(g_get_home_dir(),"/.sven/config", NULL);

	if (!sven->config->all)
		sven->config->all = sven_cfg_new();
		
	sven_cfg_remove_section(sven->config->all, "plugins");

	model=gtk_tree_view_get_model(GTK_TREE_VIEW(plugins_tree_view));
	path = gtk_tree_path_new_first ();
	if (gtk_tree_model_get_iter (model, &iter, path) == FALSE)
	{
		gtk_tree_path_free (path);
		return;
	}

	do
	{

		gchar *name;
		gint i=1;

		gtk_tree_model_get(model,&iter,0,&i,2,&name,-1);
	
		if(i==1)
			sven_cfg_write_int(sven->config->all,"plugins",g_strdup(g_basename(name)),i);
	
		gtk_tree_path_next (path);
	}
	while (gtk_tree_model_iter_next (model,&iter));

	sven_cfg_write_file(sven->config->all, filename);
	g_free(filename);
}

