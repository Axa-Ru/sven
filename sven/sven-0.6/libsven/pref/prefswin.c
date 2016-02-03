/* Sven multimedia keyboard deamon
 * prefswin.c
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
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gtk/gtkwindow.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>				/* getopt() */

#include "../sven.h"
#include "../pixmap.h"
#include "about.h"
#include "confdevice.h"
#include "prefswin.h"
#include "configplugin.h"
#include "configxosd.h"


enum CategoryViewCols {
    CATEGORY_VIEW_COL_ICON,
    CATEGORY_VIEW_COL_NAME,
    CATEGORY_VIEW_COL_ID,
    CATEGORY_VIEW_N_COLS
};


static GtkWidget *infow,*pref_notebook,*pref_treeview,*key_conf,*mouse_conf;
static void on_category_view_realize(GtkTreeView * treeview,GtkNotebook * notebook);

static void prefswin_dialog_close_lcb(GtkObject *object,Sven *sven) 
{
	/*gtk_widget_destroy(pref_notebook);
	gtk_widget_destroy(pref_treeview);*/
	//gtk_widget_destroy(key_conf);
	//gtk_widget_destroy(mouse_conf);
	destroy_test_osd ();
	gtk_widget_destroy(infow);
	infow=NULL;
	pref_notebook=NULL;
	pref_treeview=NULL;
	key_conf=NULL;
	mouse_conf=NULL;

}

static void prefswin_aplly_config (GtkWidget *wid,Sven *sven)
{
	if(sven==NULL)
		return;
		
	sconfdevice_save (SCONFDEVICE(key_conf));
	sconfdevice_save (SCONFDEVICE(mouse_conf));
	save_config_xosd (sven);
	config_plugin_save (sven);
}

static void prefswin_ok_config(GtkWidget *wid,Sven *sven) 
{
	if(sven==NULL)
		return;
	if(infow==NULL)
		return;
	

	prefswin_aplly_config(NULL,sven);
	
	/*gtk_widget_destroy(pref_notebook);
	gtk_widget_destroy(pref_treeview);*/
	//gtk_widget_destroy(key_conf);
	//gtk_widget_destroy(mouse_conf);
	destroy_test_osd ();
	gtk_widget_destroy(infow);
	infow=NULL;
	pref_notebook=NULL;
	pref_treeview=NULL;
	key_conf=NULL;
	mouse_conf=NULL;
}

static void add_page_conf_key (GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *scrolledwindow,*viewport;
	
	if(sven==NULL)
		return;
		
	/*greate list key config*/
	key_conf = sconfdevice_new (sven,_("Keyboard"),0);
	if(key_conf==NULL)
		return;
	gtk_widget_show (key_conf);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
	
	gtk_container_add (GTK_CONTAINER (viewport), key_conf);
	
	gtk_notebook_append_page (notebook,scrolledwindow,gtk_label_new (name));
}
static void add_page_conf_mouse (GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *scrolledwindow,*viewport;
	
	if(sven==NULL)
		return;

	/*greate list mouse config*/
	mouse_conf=sconfdevice_new (sven,_("Mouse"),1);
	gtk_widget_show (mouse_conf);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
	
	gtk_container_add (GTK_CONTAINER (viewport), mouse_conf);

	gtk_notebook_append_page (notebook, scrolledwindow,gtk_label_new (name));
}

static void add_page_conf_xosd (GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *vbox;
	GtkWidget *scrolledwindow,*viewport;
	
	if(sven==NULL)
		return;
		
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (viewport), vbox);

	/*greate xosd config*/
	create_xosd_gui(sven,vbox);

	gtk_notebook_append_page (notebook, scrolledwindow,gtk_label_new (name));
}

static void add_page_conf_plugin (GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *vbox;
	GtkWidget *scrolledwindow,*viewport;
	
	if(sven==NULL)
		return;
	
		scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (viewport), vbox);
	
	/*greate plugin config*/
	create_plugins_gui(sven,vbox);

	gtk_notebook_append_page (notebook, scrolledwindow,gtk_label_new (name));
}


static void add_page_conf_about (GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *vbox;
	GtkWidget *scrolledwindow,*viewport;
	
	if(sven==NULL)
		return;
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (viewport), vbox);

	/*greate about dialog*/
	create_about_gui(sven,vbox);

	gtk_notebook_append_page (notebook, scrolledwindow,gtk_label_new (name));
}

void change_category_num(int num)
{

	gtk_tree_selection_unselect_all(gtk_tree_view_get_selection(GTK_TREE_VIEW(pref_treeview)));
	gtk_notebook_set_current_page(GTK_NOTEBOOK(pref_notebook), num);
}

static void change_category(GtkNotebook * notebook,GtkTreeSelection * selection)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gint index;

	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
        	return;

	gtk_tree_model_get(model, &iter, CATEGORY_VIEW_COL_ID, &index, -1);
	gtk_notebook_set_current_page(notebook, index);
}

static void on_category_view_realize(GtkTreeView * treeview,GtkNotebook * notebook)
{
	GtkListStore *store;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	GdkPixbuf *img;

	column = gtk_tree_view_column_new();
	 gtk_tree_view_column_set_title(column, _("Category"));
	 gtk_tree_view_append_column(treeview, column);
	 gtk_tree_view_column_set_spacing(column, 2);

	renderer = gtk_cell_renderer_pixbuf_new();
	 gtk_tree_view_column_pack_start(column, renderer, FALSE);
	 gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 0, NULL);

	renderer = gtk_cell_renderer_text_new();
	 gtk_tree_view_column_pack_start(column, renderer, FALSE);
	 gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);

	store = gtk_list_store_new(CATEGORY_VIEW_N_COLS,GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));

	img=create_pixbuf_type (1,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,CATEGORY_VIEW_COL_ICON,
    			 img,
			 CATEGORY_VIEW_COL_NAME,
			 _("Keyboard"), CATEGORY_VIEW_COL_ID,
			 0, -1);
	g_object_unref(img);
	
	img=create_pixbuf_type (2,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,CATEGORY_VIEW_COL_ICON,
    			 img,
			 CATEGORY_VIEW_COL_NAME,
			 _("Mouse"), CATEGORY_VIEW_COL_ID,
			 1, -1);
	g_object_unref(img);
	
	
	img=create_pixbuf_type (16,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,
                           CATEGORY_VIEW_COL_ICON, img,
                           CATEGORY_VIEW_COL_NAME,
                           _("XOSD"), CATEGORY_VIEW_COL_ID,
                           2, -1);
	g_object_unref(img);
		
	img=create_pixbuf_type (3,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,
                           CATEGORY_VIEW_COL_ICON, img,
                           CATEGORY_VIEW_COL_NAME,
                           _("Plugins"), CATEGORY_VIEW_COL_ID,
                           3, -1);
	g_object_unref(img);
	
	img=create_pixbuf_type (4,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,
                           CATEGORY_VIEW_COL_ICON, img,
                           CATEGORY_VIEW_COL_NAME,
                           _("Info"), CATEGORY_VIEW_COL_ID,
                           4, -1);
	g_object_unref(img);
	

	selection = gtk_tree_view_get_selection(treeview);

	g_signal_connect_swapped(selection, "changed",G_CALLBACK(change_category), notebook);
}
void prefswin_dialog_create(Sven *sven) 
{
	
	GtkWidget *header;
	GtkWidget *logo_pb;
	GtkWidget *vbox, *hbox,*hpaned,*scrolledwindow;
	char *text;
	GtkWidget *hbuttonbox,*but_cancel,*but_ok,*but_apply;

	if(sven==NULL)
		return;
	
	 if (infow)
	 {
		 gdk_window_show (infow->window);
		return;
	}

	infow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (infow),  _("Sven "VERSION" Preference windows"));
	gtk_window_set_position (GTK_WINDOW (infow), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (infow), 600, 500);
	//gtk_window_set_type_hint (GTK_WINDOW (infow), GDK_WINDOW_TYPE_HINT_DIALOG);
	//gtk_window_set_destroy_with_parent (GTK_WINDOW (infow), TRUE);
	 gtk_signal_connect (GTK_OBJECT (infow), "destroy",
		      GTK_SIGNAL_FUNC (prefswin_dialog_close_lcb), sven);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (infow), vbox);
    
	hbox = gtk_hbox_new (FALSE, 8);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
    
	hpaned = gtk_hpaned_new ();
	gtk_widget_show (hpaned);
	gtk_box_pack_start (GTK_BOX (hbox), hpaned, TRUE, TRUE, 0);
  
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_paned_pack1 (GTK_PANED (hpaned), scrolledwindow, FALSE, TRUE);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

	pref_treeview = gtk_tree_view_new ();
	gtk_widget_show (pref_treeview);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), pref_treeview);
	gtk_widget_set_size_request (pref_treeview, 140, -1);
  
	/* the pref_notebook */
	pref_notebook = gtk_notebook_new ();
	gtk_widget_show (pref_notebook);
    
	gtk_paned_pack2 (GTK_PANED (hpaned), pref_notebook, TRUE, TRUE);
   
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (pref_notebook), FALSE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (pref_notebook), FALSE);
	gtk_notebook_set_scrollable (GTK_NOTEBOOK (pref_notebook), TRUE);
	gtk_notebook_popup_enable (GTK_NOTEBOOK (pref_notebook));

	/* add pages */
	add_page_conf_key (GTK_NOTEBOOK (pref_notebook), _("Keyboard"),sven);
	add_page_conf_mouse (GTK_NOTEBOOK (pref_notebook), _("Mouse"),sven);
	add_page_conf_xosd (GTK_NOTEBOOK (pref_notebook), _("XOSD"),sven);
	add_page_conf_plugin (GTK_NOTEBOOK (pref_notebook), _("Plugins"),sven);
	add_page_conf_about (GTK_NOTEBOOK (pref_notebook), _("About"),sven);
	
	
	on_category_view_realize(GTK_TREE_VIEW(pref_treeview),GTK_NOTEBOOK(pref_notebook)); 
    
	/* button */
	hbuttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_END);

	but_ok = gtk_button_new_from_stock ("gtk-ok");
	gtk_widget_show (but_ok);
	g_signal_connect(but_ok, "clicked", G_CALLBACK(prefswin_ok_config),sven);
	gtk_container_add (GTK_CONTAINER (hbuttonbox), but_ok);
	GTK_WIDGET_SET_FLAGS (but_ok, GTK_CAN_DEFAULT);

	but_apply = gtk_button_new_from_stock ("gtk-apply");
	gtk_widget_show (but_apply);
	g_signal_connect(but_apply, "clicked", G_CALLBACK(prefswin_aplly_config),sven);
	gtk_container_add (GTK_CONTAINER (hbuttonbox), but_apply);
	GTK_WIDGET_SET_FLAGS (but_apply, GTK_CAN_DEFAULT);
	
	but_cancel = gtk_button_new_from_stock ("gtk-cancel");
	gtk_widget_show (but_cancel);
	gtk_container_add (GTK_CONTAINER (hbuttonbox), but_cancel);
	GTK_WIDGET_SET_FLAGS (but_cancel, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(but_cancel);
    
	 
	g_signal_connect(but_cancel, "clicked", G_CALLBACK(prefswin_dialog_close_lcb),sven);
	
	gtk_widget_show_all(infow);
}

