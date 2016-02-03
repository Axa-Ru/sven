/* Sven multimedia keyboard deamon
 * configure.c
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

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "autoconvert.h"
#include "configure.h"
#include "flag.h"
#include "xutil.h"

#include "../../libsven/sven.h"
#include "../../libsven/utils.h"
#include "../../libsven/xlib.h"
#include "../../libsven/pixmap.h"
#include "../../libsven/emulate_input.h"


static   GtkWidget *confwind;
static GtkWidget *conf_wind_treeview;
static GtkWidget *combo_wind;
static GtkWidget *lang_combo,*check_flag_ind,*check_flag_tray,*check_switch;
/**************************************** window config **********************************************************************/


static void fixed_toggled_w (GtkCellRendererToggle *cell,gchar  *path_str,gpointer data)
{
	GtkTreeModel *model = (GtkTreeModel *)data;
	GtkTreeIter  iter;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	gboolean fixed;

	/* get toggled iter */
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter,1, &fixed, -1);

	/* do something with the value */
	fixed ^= 1;

	/* set new value */
	gtk_list_store_set (GTK_LIST_STORE (model), &iter,1, fixed, -1);

	/* clean up */
	gtk_tree_path_free (path);
}

static void conf_wind_treeview_print(Sven *sven)
{
	GtkListStore *store;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeIter iter;


	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_BOOLEAN);
	gtk_tree_view_set_model(GTK_TREE_VIEW(conf_wind_treeview), GTK_TREE_MODEL(store));
	
	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column, _("List window for not work autoconvert"));
	 gtk_tree_view_append_column(GTK_TREE_VIEW(conf_wind_treeview), column);
	 gtk_tree_view_column_set_spacing(column, 20);

	renderer = gtk_cell_renderer_text_new();
	 gtk_tree_view_column_pack_start(column, renderer, FALSE);
	 gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
	 
	 renderer = gtk_cell_renderer_toggle_new();
	 gtk_tree_view_column_pack_start(column, renderer, FALSE);
	 gtk_tree_view_column_set_attributes(column, renderer, "active", 1, NULL);
	  g_signal_connect (G_OBJECT (renderer), "toggled",G_CALLBACK (fixed_toggled_w), gtk_tree_view_get_model(GTK_TREE_VIEW(conf_wind_treeview)));

	
	if(sven->config->all!=NULL)	
	{
		ConfigSection *sect;

		if (!(sect = sven_cfg_find_section(sven->config->all, "conv_win_list")))
			return ;
	
		{
			ConfigLine *line;
			GList *list;
		
			list = sect->lines;
			while (list)
			{
				line = (ConfigLine *) list->data;

				gtk_list_store_append(store, &iter);
				gtk_list_store_set(store, &iter,
						0,line->key, 
						1,atoi(line->value), -1);	
				list = g_list_next(list);
			}
		}
	}
}

static void print_wind_list (GtkListStore *list_model,GtkTreeIter *iter,gchar *name,gint i)
{
	gtk_list_store_set (list_model,iter,0,name,1,i,-1);
}


static void conf_wind_add (GtkWidget *wid,Sven *sven)
{
	GtkTreeIter iter;
	GtkTreeIter iterplug;
	gchar *name;
	gint i=1;

	gtk_list_store_prepend (GTK_LIST_STORE (gtk_tree_view_get_model(GTK_TREE_VIEW(conf_wind_treeview))), &iter);
	gtk_tree_selection_select_iter(GTK_TREE_SELECTION (G_OBJECT (gtk_tree_view_get_selection (GTK_TREE_VIEW (conf_wind_treeview)))),&iter);

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combo_wind),&iterplug);
	gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(combo_wind)), &iterplug,0, &name,-1);
	
	print_wind_list (GTK_LIST_STORE (gtk_tree_view_get_model(GTK_TREE_VIEW(conf_wind_treeview))),&iter,name,i);
}

static void conf_wind_dele (GtkWidget *wid,gpointer data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	gtk_tree_selection_get_selected(GTK_TREE_SELECTION (G_OBJECT (gtk_tree_view_get_selection (GTK_TREE_VIEW (conf_wind_treeview)))), &model, &iter);
	gtk_list_store_remove(GTK_LIST_STORE (model),&iter);
}

static GtkTreeModel * create_combo_menu_conf_wind (Sven *sven)
{
        GtkTreeIter iter;
        GtkListStore *store;

        store = gtk_list_store_new (1,G_TYPE_STRING);
	
	{
		Window rwin, parent, *children, *child, app; unsigned int num;
		
		XQueryTree(XOpenDisplay(XDisplayName(NULL)),gdk_x11_get_default_root_xwindow(), &rwin, &parent, &children, &num);
	
		child = children;
		while (num) 
		{
			app = (Window) 0;
			GetAppWindow(GDK_DISPLAY (),*child, &app);
			if (app) 
			{
				gtk_list_store_append(store, &iter);
				gtk_list_store_set(store, &iter,0,xObject_getWMClass_class (GDK_DISPLAY (),app),  -1);
			}
			child++; num--;
		}
		if (children) XFree(children);
	}
        
	return GTK_TREE_MODEL (store);
}

static void create_conf_wind(gpointer data,GtkWidget *vbox1)
{ 
	Sven *sven = (Sven *)data;
	GtkWidget *scrolledwindow;
	GtkWidget *hbox2;
	GtkWidget *button1;
	GtkWidget *button2;
	GtkCellRenderer *renderer;

	DEBUG_MSG("Start create_about_gui\n");

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
	
	conf_wind_treeview = gtk_tree_view_new ();
	gtk_widget_show (conf_wind_treeview);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), conf_wind_treeview);
	
	 hbox2 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox2);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, FALSE,10);
	
	button1 = gtk_button_new_from_stock ("gtk-remove");
	gtk_widget_show (button1);
	g_signal_connect(G_OBJECT(button1), "clicked", (GtkSignalFunc) conf_wind_dele, conf_wind_treeview);
	gtk_box_pack_start (GTK_BOX (hbox2), button1, FALSE, FALSE, 0);
	
	combo_wind = gtk_combo_box_new_with_model (create_combo_menu_conf_wind (sven));
	gtk_box_pack_start (GTK_BOX (hbox2), combo_wind, TRUE, TRUE, 0);
	
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_wind),renderer,TRUE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_wind), renderer,"text", 0,NULL);
	
	gtk_combo_box_set_active (GTK_COMBO_BOX (combo_wind),0);
	gtk_widget_show (combo_wind);
	
	button2 = gtk_button_new_from_stock ("gtk-add");
	gtk_widget_show (button2);
	g_signal_connect(G_OBJECT(button2), "clicked", (GtkSignalFunc) conf_wind_add, sven);
	gtk_box_pack_start (GTK_BOX (hbox2), button2, FALSE, FALSE, 0);
	
	conf_wind_treeview_print(sven); 
}


/*****************************************************************************************************************************/


static void prefswin_dialog_close_lcb(GtkObject *object,gpointer data) 
{
	gtk_widget_destroy(confwind);
	confwind=NULL;
}

void configure_save (gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	Sven *sven = (Sven *)aconv->sven;
	gchar *filename;
	GtkTreeModel *model;
	GtkTreeIter  iter;
	GtkTreePath *path;
	
	gchar *group;
	int i=0;
	GtkTreeIter iterplug;

	if(sven==NULL)
		return;
	
	filename= g_strconcat(g_get_home_dir(),"/.sven/config", NULL);

	if (!sven->config->all)
		sven->config->all = sven_cfg_new();
		
	sven_cfg_remove_section(sven->config->all, "conv_win_list");

	model=gtk_tree_view_get_model(GTK_TREE_VIEW(conf_wind_treeview));
	path = gtk_tree_path_new_first ();
	if (gtk_tree_model_get_iter (model, &iter, path) == FALSE)
	{
		gtk_tree_path_free (path);
		/* return; */
	} else {

	do
	{

		gchar *name=NULL;
		gint i=1;

		gtk_tree_model_get(model,&iter,0,&name,1,&i,-1);
	
		sven_cfg_write_int(sven->config->all,"conv_win_list",name,i);
	
		gtk_tree_path_next (path);
	}
	while (gtk_tree_model_iter_next (model,&iter));

	}

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(lang_combo),&iterplug);
	gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(lang_combo)), &iterplug,1, &group,-1);
	
	for (i = 0; i < aconv->sxkb->ngroups; i++) 
	{
		if(g_strcasecmp (aconv->sxkb->group2info[i]->name,group)==0) 
			break;
	}
	aconv->sxkb->view_flag=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (check_flag_ind));
	aconv->sxkb->view_flag_tray=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (check_flag_tray));

	sven_cfg_write_int (sven->config->all,"xkb", "def_group",i);
	sven_cfg_write_int (sven->config->all,"xkb", "view_flag",aconv->sxkb->view_flag);
	sven_cfg_write_int (sven->config->all,"xkb", "view_flag_tray",aconv->sxkb->view_flag_tray);
	aconv->sxkb->def_group=i;

	if(!aconv->sxkb->view_flag_tray)
	{
		GdkPixbuf *pixbuf=NULL;
	
		pixbuf=create_pixbuf_type (17,20,20);
		gtk_status_icon_set_from_pixbuf(sven->tray_icon,pixbuf);
		g_object_unref(pixbuf);
	}
	
	sven_cfg_write_int (sven->config->all,"autoconvert", "auto_switch",gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (check_switch)));
	
/*	sven_cfg_write_int (sven->config->all, "autoconvert", "auto_switch",gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (check_switch)));
	sven_cfg_write_string (sven->config->all, "autoconvert", "conv_lang",g_strdup(gtk_entry_get_text (GTK_ENTRY (undo_key)))?g_strdup(gtk_entry_get_text (GTK_ENTRY (undo_key))):"Pause");
	sven_cfg_write_string (sven->config->all, "autoconvert", "conv_case",g_strdup(gtk_entry_get_text (GTK_ENTRY (case_key)))?g_strdup(gtk_entry_get_text (GTK_ENTRY (case_key))):"Scroll_Lock");
*/
	sven_cfg_write_file(sven->config->all, filename);
	g_free(filename);
}

static void prefswin_ok_config(GtkWidget *wid,gpointer data) 
{
	if(data==NULL)
		return;
	if(confwind==NULL)
		return;
	
	configure_save (data);
	
	gtk_widget_destroy(GTK_WIDGET(confwind));
	confwind=NULL;
}

static void prefswin_aplly_config (GtkWidget *wid,gpointer data)
{
	if(data==NULL)
		return;
		
	configure_save (data);
}

static void add_page_conf_wind (GtkNotebook * notebook, const gchar * name,Sven *sven)
{
	GtkWidget *vbox;
	
	if(sven==NULL)
		return;

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
    
	create_conf_wind(sven,vbox);

	gtk_notebook_append_page (notebook,vbox,gtk_label_new (name));
}

static void add_page_conf_convert (GtkNotebook * notebook, const gchar * name,gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	Sven *sven = (Sven *)aconv->sven;
	GtkWidget *vbox,*alignment;
	int i=1;

	if((sven==NULL)||(aconv==NULL))
		return;
	
	sven_cfg_read_int (sven->config->all, "autoconvert", "auto_switch",&i);


	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
    
	alignment = gtk_alignment_new (0.1, 1, 0, 1);
	gtk_widget_show (alignment);
	gtk_box_pack_start (GTK_BOX (vbox), alignment, FALSE, FALSE, 0);
	
	check_switch = gtk_check_button_new_with_mnemonic (_("AutoSwitch - switch keyboard language automatically"));
	gtk_widget_show (check_switch);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_switch),i);
	gtk_container_add (GTK_CONTAINER (alignment), check_switch);

	gtk_notebook_append_page (notebook,vbox,gtk_label_new (name));
}


static GtkTreeModel * create_combo_menu_flag_conf (gpointer data)
{
	AConvert *aconv = (AConvert *)data;
        GtkTreeIter iter;
        GtkListStore *store;
	int i;

        store = gtk_list_store_new (2,GDK_TYPE_PIXBUF,G_TYPE_STRING);
	
	for (i = 0; i < aconv->sxkb->ngroups; i++) 
	{
	
		gtk_list_store_append (store, &iter);
		GdkPixbuf *pixbuf=create_pixbuf_flag(aconv->sxkb->group2info[i]->name,18,12);
        	gtk_list_store_set (store, &iter,0,pixbuf,1,aconv->sxkb->group2info[i]->name,-1);
		g_object_unref(pixbuf);
	}
        
	return GTK_TREE_MODEL (store);
}


static void add_page_conf_xkb (GtkNotebook * notebook, const gchar * name,gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	GtkWidget *vbox;
	GtkWidget *label,*frame,*alignment;
	GtkCellRenderer *renderer;
	
	if((aconv==NULL))
		return;
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("<b>Select default language</b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

	lang_combo = gtk_combo_box_new_with_model (create_combo_menu_flag_conf (aconv));
	gtk_container_add (GTK_CONTAINER (alignment), lang_combo);
	
	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (lang_combo),renderer,TRUE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (lang_combo), renderer,"pixbuf", 0,NULL);
	
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (lang_combo),renderer,TRUE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (lang_combo), renderer,"text", 1,NULL);
	
	gtk_combo_box_set_active (GTK_COMBO_BOX (lang_combo),aconv->sxkb->def_group);
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
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_flag_ind),aconv->sxkb->view_flag);
	gtk_container_add (GTK_CONTAINER (alignment), check_flag_ind);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	check_flag_tray = gtk_check_button_new_with_mnemonic (_("Display language indicator on sistem tray"));
	gtk_widget_show (check_flag_tray);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_flag_tray),aconv->sxkb->view_flag_tray);
	gtk_container_add (GTK_CONTAINER (alignment), check_flag_tray);

	gtk_notebook_append_page (notebook, vbox,gtk_label_new (name));
}

void create_confwind (Sven *sven,gpointer data)
{
  GtkWidget *vbox;
  GtkWidget *notebook;
 GtkWidget *hbuttonbox,*but_cancel,*but_ok,*but_apply;

  confwind = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (confwind),_("Auto Convert Preference windows"));
  gtk_window_set_position (GTK_WINDOW (confwind), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW (confwind),350,400);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (confwind), vbox);

  gtk_box_pack_start (GTK_BOX (vbox), (gpointer ) create_header(_("Auto Convert Preference windows")), FALSE, FALSE, 0);
  
  notebook = gtk_notebook_new ();
  gtk_widget_show (notebook);
  gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_LEFT);
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
  gtk_notebook_popup_enable (GTK_NOTEBOOK (notebook));

  add_page_conf_wind (GTK_NOTEBOOK (notebook), _("Window list"),sven);
  add_page_conf_xkb (GTK_NOTEBOOK (notebook), _("Configure XKB"),data);
  add_page_conf_convert(GTK_NOTEBOOK (notebook), _("Configure Auto Convert"),data);

 /************************************************************************************************************/	
	    
	/* button */
	hbuttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox);
	gtk_box_pack_end (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_END);

	but_ok = gtk_button_new_from_stock ("gtk-ok");
	gtk_widget_show (but_ok);
	g_signal_connect(but_ok, "clicked", G_CALLBACK(prefswin_ok_config),data);
	gtk_container_add (GTK_CONTAINER (hbuttonbox), but_ok);
	GTK_WIDGET_SET_FLAGS (but_ok, GTK_CAN_DEFAULT);

	but_apply = gtk_button_new_from_stock ("gtk-apply");
	gtk_widget_show (but_apply);
	g_signal_connect(but_apply, "clicked", G_CALLBACK(prefswin_aplly_config),data);
	gtk_container_add (GTK_CONTAINER (hbuttonbox), but_apply);
	GTK_WIDGET_SET_FLAGS (but_apply, GTK_CAN_DEFAULT);
	
	but_cancel = gtk_button_new_from_stock ("gtk-cancel");
	gtk_widget_show (but_cancel);
	gtk_container_add (GTK_CONTAINER (hbuttonbox), but_cancel);
	GTK_WIDGET_SET_FLAGS (but_cancel, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(but_cancel);
    	g_signal_connect(but_cancel, "clicked", G_CALLBACK(prefswin_dialog_close_lcb),data);
	
	gtk_widget_show_all(confwind);
}

void configure(gpointer sdata,gpointer data)
{
	Sven *sven = (Sven *)sdata;
	create_confwind(sven,data);
	//prefswin_dialog_create(sven);
//	printf("Start configure\n");
}
