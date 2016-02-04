/* Sven multimedia keyboard deamon
 * prefswin.c
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
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gtk/gtkwindow.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>				/* getopt() */

#include "sven.h"
#include "utils.h"
#include "prefswin.h"
#include "confdevice.h"
#include "confcorrect.h"
#include "confswitch.h"

#include "configxosd.h"
#include "configplugin.h"
#include "configany.h"
#include "about.h"

enum CategoryViewCols {
    CATEGORY_VIEW_COL_ICON,
    CATEGORY_VIEW_COL_NAME,
    CATEGORY_VIEW_COL_ID,
    CATEGORY_VIEW_N_COLS
};


static GtkWidget *info,*pref_notebook,*pref_treeview,*key_conf,*mouse_conf,*correct_conf,*switch_conf,*lang_combo,*check_flag_ind;
static void on_category_view_realize(GtkTreeView * treeview,GtkNotebook * notebook);

static GtkWidget *create_header (GtkWidget * image, const char *text)
{
	GtkWidget *eventbox, *label, *hbox;
	GtkStyle *style;
	char *markup;
	GdkColormap *cmap;
	GdkColor color;

	eventbox = gtk_event_box_new ();
	gtk_widget_show (eventbox);

	hbox = gtk_hbox_new (FALSE, 12);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 4);
	gtk_widget_show (hbox);
	gtk_container_add (GTK_CONTAINER (eventbox), hbox);

	if (image)
	{
		gtk_widget_show (image);
		gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
	}

	style = gtk_widget_get_style (eventbox);
	cmap = gdk_colormap_get_system ();
	color.red = 0xaaaa;
	color.green = 0x1112;
	color.blue = 0x3fea;

	if (!gdk_color_alloc (cmap, &color)) 
	{
		g_print ("couldn't allocate color");
	}
    
	gtk_widget_modify_bg (eventbox, GTK_STATE_NORMAL,&color);

	markup = g_strconcat ("<span size=\"larger\"  weight=\"bold\">", _(text),"</span>", NULL);
	label = gtk_label_new (markup);
	g_free (markup);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

	style = gtk_widget_get_style (label);
	gtk_widget_modify_fg (label, GTK_STATE_NORMAL,&style->fg[GTK_STATE_SELECTED]);

	return eventbox;
}

static void prefswin_dialog_close_lcb(GtkObject *object,Sven *sven) 
{
	corect_init(sven);
	gtk_widget_destroy(info);
	info=NULL;
}

static void save_conf_xkb (Sven *sven)
{
	ConfigFile *cfgfile;
	gchar *filename;
	gchar *group;
	int i=0;
	GtkTreeIter iterplug;
	
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(lang_combo),&iterplug);
	gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(lang_combo)), &iterplug,1, &group,-1);
	
	for (i = 0; i < sven->sxkb->ngroups; i++) 
	{
		if(g_strcasecmp (sven->sxkb->group2info[i]->name,group)==0) 
			break;
	}
	sven->sxkb->view_flag=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (check_flag_ind));

	filename= g_strconcat(g_get_home_dir(),"/.sven/config", NULL);
	cfgfile = sven_cfg_open_file(filename);

	if (!cfgfile)
		cfgfile = sven_cfg_new();

	sven_cfg_write_int (cfgfile,"xkb", "def_group",i);
	sven_cfg_write_int (cfgfile,"xkb", "view_flag",sven->sxkb->view_flag);
	sven->sxkb->def_group=i;

	sven_cfg_write_file(cfgfile, filename);

	sven_cfg_free(cfgfile);
	g_free(filename);
	
}


static void prefswin_ok_config(GtkWidget *wid,Sven *sven) 
{
	if(sven==NULL)
		return;
	if(info==NULL)
		return;
	
	save_conf_xkb (sven);
	save_config_pref(sven);
	save_config_xosd(sven);
	sconfdevice_save (SCONFDEVICE(key_conf));
	sconfdevice_save (SCONFDEVICE(mouse_conf));
	sconfcorrect_save (SCONFCORRECT(correct_conf));
	sconfswitch_save (SCONFSWITCH(switch_conf));
	
	gtk_widget_destroy(GTK_WIDGET(info));
	info=NULL;
}

static void prefswin_aplly_config (GtkWidget *wid,Sven *sven)
{
	if(sven==NULL)
		return;
	
	save_conf_xkb (sven);
	save_config_pref(sven);
	save_config_xosd(sven);
	sconfdevice_save (SCONFDEVICE(key_conf));
	sconfdevice_save (SCONFDEVICE(mouse_conf));
	sconfcorrect_save (SCONFCORRECT(correct_conf));
	sconfswitch_save (SCONFSWITCH(switch_conf));
}

static void add_page_conf_key (GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *scrolledwindow,*viewport;
	
	if(sven==NULL)
		return;
		
	/*greate list key config*/
	key_conf = sconfdevice_new (sven,_("Key configure"),0);
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
	mouse_conf=sconfdevice_new (sven,_("Mouse configure"),1);
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

static void add_page_conf_correct(GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *scrolledwindow,*viewport;
	
	if(sven==NULL)
		return;

	correct_conf=sconfcorrect_new (sven,_("AutoCorrect Option"),0);
	gtk_widget_show (correct_conf);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
	
	gtk_container_add (GTK_CONTAINER (viewport), correct_conf);
	
	gtk_notebook_append_page (notebook, scrolledwindow,gtk_label_new (name));
}

static void add_page_conf_switch(GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *scrolledwindow,*viewport;
	if(sven==NULL)
		return;

	switch_conf=sconfswitch_new (sven,_("Switching Option"));
	gtk_widget_show (switch_conf);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
	
	gtk_container_add (GTK_CONTAINER (viewport), switch_conf);

	gtk_notebook_append_page (notebook, scrolledwindow,gtk_label_new (name));
}

static GtkTreeModel * create_combo_menu_flag_conf (Sven *sven)
{
        GtkTreeIter iter;
        GtkListStore *store;
	int i;

        store = gtk_list_store_new (2,GDK_TYPE_PIXBUF,G_TYPE_STRING);
	
	for (i = 0; i < sven->sxkb->ngroups; i++) 
	{
	
		gtk_list_store_append (store, &iter);
        	gtk_list_store_set (store, &iter,0,new_pixbuf_flag(sven->sxkb->group2info[i]->name,18,12),1,sven->sxkb->group2info[i]->name,-1);
	}
        
	return GTK_TREE_MODEL (store);
}

static void add_page_conf_xkb (GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *vbox;
	GtkWidget *label,*scrolledwindow,*viewport,*frame,*alignment;
	GtkCellRenderer *renderer;
	
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
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	//gtk_container_add (GTK_CONTAINER (vbox), frame);
	
	label = gtk_label_new (_("<b>Select default language</b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

	lang_combo = gtk_combo_box_new_with_model (create_combo_menu_flag_conf (sven));
	gtk_container_add (GTK_CONTAINER (alignment), lang_combo);
	//gtk_box_pack_start (GTK_BOX (alignment), lang_combo, FALSE, TRUE, 0);
	
	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (lang_combo),renderer,TRUE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (lang_combo), renderer,"pixbuf", 0,NULL);
	
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (lang_combo),renderer,TRUE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (lang_combo), renderer,"text", 1,NULL);
	
	gtk_combo_box_set_active (GTK_COMBO_BOX (lang_combo),sven->sxkb->def_group);
	gtk_widget_show (lang_combo);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	check_flag_ind = gtk_check_button_new_with_mnemonic (_("Display popup language indicator on layout change"));
	gtk_widget_show (check_flag_ind);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_flag_ind),sven->sxkb->view_flag);
	gtk_container_add (GTK_CONTAINER (alignment), check_flag_ind);
	

/*	
	label = gtk_label_new (_("<b><span color=\"red\">Sorry this function realised by next  version</span></b>"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
*/
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

static void add_page_conf_pref (GtkNotebook * notebook, const gchar * name,Sven *sven)
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
	create_pref_gui(sven,vbox);

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

	img=create_pixbuf (13,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,CATEGORY_VIEW_COL_ICON,
    			 img,
			 CATEGORY_VIEW_COL_NAME,
			 _("Key configure"), CATEGORY_VIEW_COL_ID,
			 0, -1);
	g_object_unref(img);
	
	img=create_pixbuf (29,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,CATEGORY_VIEW_COL_ICON,
    			 img,
			 CATEGORY_VIEW_COL_NAME,
			 _("Mouse configure"), CATEGORY_VIEW_COL_ID,
			 1, -1);
	g_object_unref(img);
	
	img=create_pixbuf (30,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,CATEGORY_VIEW_COL_ICON,
    			 img,
			 CATEGORY_VIEW_COL_NAME,
			 _("XKB Option"), CATEGORY_VIEW_COL_ID,
			 2, -1);
	g_object_unref(img);
	
	img=create_pixbuf (23,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,CATEGORY_VIEW_COL_ICON,
    			 NULL,
			 CATEGORY_VIEW_COL_NAME,
			 _("AutoCorrect Option"), CATEGORY_VIEW_COL_ID,
			 3, -1);
	g_object_unref(img);
	
	img=create_pixbuf (27,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,CATEGORY_VIEW_COL_ICON,
    			 NULL,
			 CATEGORY_VIEW_COL_NAME,
			 _("Switching Option"), CATEGORY_VIEW_COL_ID,
			 4, -1);
	g_object_unref(img);
	
	img=create_pixbuf (11,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,
                           CATEGORY_VIEW_COL_ICON, img,
                           CATEGORY_VIEW_COL_NAME,
                           "Other Preference", CATEGORY_VIEW_COL_ID,
                           5, -1);
	g_object_unref(img);
	
	img=create_pixbuf (12,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,
                           CATEGORY_VIEW_COL_ICON, img,
                           CATEGORY_VIEW_COL_NAME,
                           _("XOSD configure"), CATEGORY_VIEW_COL_ID,
                           6, -1);
	g_object_unref(img);
	
	img=create_pixbuf (16,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,
                           CATEGORY_VIEW_COL_ICON, img,
                           CATEGORY_VIEW_COL_NAME,
                           _("Plugins"), CATEGORY_VIEW_COL_ID,
                           7, -1);
	g_object_unref(img);
	
	img=create_pixbuf (15,34,34);
	 gtk_list_store_append(store, &iter);
	 gtk_list_store_set(store, &iter,
                           CATEGORY_VIEW_COL_ICON, img,
                           CATEGORY_VIEW_COL_NAME,
                           _("Info"), CATEGORY_VIEW_COL_ID,
                           8, -1);
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
	
	 if (info)
	 {
		 gdk_window_show (info->window);
		return;
	}

	info = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (info),  _("Sven "VERSION" Preference windows"));
	gtk_window_set_position (GTK_WINDOW (info), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (info), 600, 500);
	//gtk_window_set_type_hint (GTK_WINDOW (info), GDK_WINDOW_TYPE_HINT_DIALOG);
	//gtk_window_set_destroy_with_parent (GTK_WINDOW (info), TRUE);
	 gtk_signal_connect (GTK_OBJECT (info), "destroy",
		      GTK_SIGNAL_FUNC (prefswin_dialog_close_lcb), info);

	logo_pb = new_pixmap_mod(1,60,60);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (info), vbox);
    
	/* header with logo */
	text = g_strdup_printf
	("%s\n<span size=\"smaller\" style=\"italic\" color=\"#6872fe\">%s</span>",
	 _("Sven multimedia keyboard"),
	 _("Copyright 2004-2005 by Eugene Morenko(More)"));
    
	header = create_header (logo_pb, text);
	gtk_widget_show (header);
	gtk_box_pack_start (GTK_BOX (vbox), header, FALSE, TRUE, 0);
	g_free (text);

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
	gtk_widget_set_size_request (pref_treeview, 170, -1);
  
	/* the pref_notebook */
	pref_notebook = gtk_notebook_new ();
	gtk_widget_show (pref_notebook);
    
	gtk_paned_pack2 (GTK_PANED (hpaned), pref_notebook, TRUE, TRUE);
   
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (pref_notebook), FALSE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (pref_notebook), FALSE);
	gtk_notebook_set_scrollable (GTK_NOTEBOOK (pref_notebook), TRUE);
	gtk_notebook_popup_enable (GTK_NOTEBOOK (pref_notebook));

	/* add pages */
	add_page_conf_key (GTK_NOTEBOOK (pref_notebook), _("Configure key"),sven);
	add_page_conf_mouse (GTK_NOTEBOOK (pref_notebook), _("Configure mouse"),sven);
	add_page_conf_xkb (GTK_NOTEBOOK (pref_notebook), _("Option XKB"),sven);
	add_page_conf_correct (GTK_NOTEBOOK (pref_notebook), _("Option AutoCorrect"),sven);
	add_page_conf_switch (GTK_NOTEBOOK (pref_notebook), _("Option Switching"),sven);
	add_page_conf_pref (GTK_NOTEBOOK (pref_notebook), _("Preference"),sven);
	add_page_conf_xosd (GTK_NOTEBOOK (pref_notebook), _("Configure XOSD"),sven);
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
	
	gtk_widget_show_all(info);
}

