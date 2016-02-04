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
#include "confcorrect.h"

static gchar *menu_tact=NULL;
static gchar *menu_tcom=NULL;

static void sconfcorrect_class_init          (SconfcorrectClass *klass);
static void sconfcorrect_init                (Sconfcorrect      *sccor);
static void print_key_list (GtkListStore *list_model,GtkTreeIter *iter,gchar *key,gchar *value);

GType
sconfcorrect_get_type (void)
{
  static GType sccor_type = 0;

  if (!sccor_type)
    {
      static const GTypeInfo sccor_info =
      {
	sizeof (SconfcorrectClass),
	NULL,		/* base_init */
	NULL,		/* base_finalize */
	(GClassInitFunc) sconfcorrect_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data */
	sizeof (Sconfcorrect),
	0,		/* n_preallocs */
	(GInstanceInitFunc) sconfcorrect_init,
      };

      sccor_type = g_type_register_static (GTK_TYPE_VBOX, "Sconfcorrect",
					    &sccor_info, 0);
    }

  return sccor_type;
}

static void
sconfcorrect_class_init (SconfcorrectClass *klass)
{
  
}

static void
sconfcorrect_init (Sconfcorrect *sccor)
{
	sccor->list_tree_view=NULL;
	sccor->list_model=NULL;
	sccor->list_selection=NULL;
	
	sccor->prefs_new_wid=NULL;
	sccor->prefs_del_wid=NULL;
	sccor->prefs_edit_wid=NULL;
	
	sccor->lang="";
}


static void add_print_list(GtkTreeView *list_tree_view,Sconfcorrect *sccor,gchar *lang)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	ConfigSection *section;
	SvenPlugin *splg;
	GList *list;
	
	ConfigSection *sect;
	ConfigLine *line;

	DEBUG_MSG(_("Start add_print_list\n"));

	 gtk_list_store_clear(GTK_LIST_STORE (sccor->list_model));

	 if ((g_strcasecmp (lang,"English")==0) || (g_strcasecmp (lang,"US/ASCII")==0) || (g_strcasecmp (lang,"en_US")==0) || (g_strcasecmp (lang,"Maltese (US layout)")==0))
	{
		gtk_entry_set_text (GTK_ENTRY(sccor->misspelled),"");
		gtk_entry_set_text (GTK_ENTRY(sccor->correct),"");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sccor->check_enable_lang),sccor->corr_en_init);
		
		if(sccor->sven->keyboard->corr_en!=NULL)
		{	
			if (!(sect = sven_cfg_find_section(sccor->sven->keyboard->corr_en, "orfo")))
				return ;
				
			model = gtk_tree_view_get_model (list_tree_view);
	
			list = sect->lines;
			while (list)
			{
				line = (ConfigLine *) list->data;
				
				gtk_list_store_append (GTK_LIST_STORE (model), &iter);
				print_key_list(sccor->list_model,&iter,g_strdup(line->key),g_strdup(line->value));
				
				list = g_list_next(list);
			}
		}
	
	}
	else if((g_strcasecmp (lang,"Russian")==0) ||  (g_strcasecmp (lang,"Russian(phonetic)")==0)) 
	{
		gtk_entry_set_text (GTK_ENTRY(sccor->misspelled),"");
		gtk_entry_set_text (GTK_ENTRY(sccor->correct),"");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sccor->check_enable_lang),sccor->corr_ru_init);
		
		if(sccor->sven->keyboard->corr_ru!=NULL)
		{	
			if (!(sect = sven_cfg_find_section(sccor->sven->keyboard->corr_ru, "orfo")))
				return ;
				
			model = gtk_tree_view_get_model (list_tree_view);
	
			list = sect->lines;
			while (list)
			{
				line = (ConfigLine *) list->data;
				
				gtk_list_store_append (GTK_LIST_STORE (model), &iter);
				print_key_list(sccor->list_model,&iter,g_strdup(line->key),g_strdup(line->value));
				
				list = g_list_next(list);
			}
		}
	
	}

	gtk_widget_set_sensitive(GTK_WIDGET(sccor->prefs_edit_wid), 0);
	gtk_widget_set_sensitive(GTK_WIDGET(sccor->prefs_del_wid), 0);
}

static void prefs_list_clicked(GtkTreeSelection *list_selection,Sconfcorrect *sccor)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *misspel;
	gchar *corr;

	if (gtk_tree_selection_get_selected(list_selection, &model, &iter)) 
	{
		gtk_tree_model_get(model,&iter, 0, &misspel,1, &corr,-1);
		
		gtk_entry_set_text (GTK_ENTRY(sccor->misspelled),misspel);
		gtk_entry_set_text (GTK_ENTRY(sccor->correct),corr);
		
		gtk_widget_set_sensitive(GTK_WIDGET(sccor->prefs_edit_wid), 1);
		gtk_widget_set_sensitive(GTK_WIDGET(sccor->prefs_del_wid), 1);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(sccor->prefs_edit_wid),0);
		gtk_widget_set_sensitive(GTK_WIDGET(sccor->prefs_del_wid),0);
	}
}

static void configure_dele_cb_but (GtkWidget *wid,Sconfcorrect *sccor)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *misspel;

	gtk_tree_selection_get_selected(GTK_TREE_SELECTION (sccor->list_selection), &model, &iter);
	gtk_tree_model_get(model,&iter, 0, &misspel,-1);
	 if ((g_strcasecmp (sccor->lang,"English")==0) || (g_strcasecmp (sccor->lang,"US/ASCII")==0) || (g_strcasecmp (sccor->lang,"en_US")==0) || (g_strcasecmp (sccor->lang,"Maltese (US layout)")==0))
	{
		if (sccor->sven->keyboard->corr_en!=NULL)
			sven_cfg_remove_key(sccor->sven->keyboard->corr_en,"orfo",misspel);
	}
	else if((g_strcasecmp (sccor->lang,"Russian")==0) ||  (g_strcasecmp (sccor->lang,"Russian(phonetic)")==0)) 
	{
		if (sccor->sven->keyboard->corr_ru!=NULL)
			sven_cfg_remove_key(sccor->sven->keyboard->corr_ru,"orfo",misspel);
	}
	gtk_list_store_remove(GTK_LIST_STORE (model),&iter);
}

static void configure_modifer_but (GtkWidget *wid,Sconfcorrect *sccor)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	int t=0;
	gchar *misspel;
	gchar *corr;
	
	t=gtk_tree_selection_get_selected(GTK_TREE_SELECTION (sccor->list_selection), &model, &iter);
	/*Not selected*/
	if(t==0)
	{
		gtk_list_store_prepend (GTK_LIST_STORE (sccor->list_model), &iter);
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION (sccor->list_selection),&iter);
	}
	
	misspel = g_strdup (gtk_entry_get_text (GTK_ENTRY (sccor->misspelled)));
 	corr =g_strdup (gtk_entry_get_text (GTK_ENTRY (sccor->correct)));

	 if ((g_strcasecmp (sccor->lang,"English")==0) || (g_strcasecmp (sccor->lang,"US/ASCII")==0) || (g_strcasecmp (sccor->lang,"en_US")==0) || (g_strcasecmp (sccor->lang,"Maltese (US layout)")==0))
	{
		if (!sccor->sven->keyboard->corr_en)
			sccor->sven->keyboard->corr_en = sven_cfg_new();
			
		sven_cfg_write_string(sccor->sven->keyboard->corr_en,"orfo",misspel,corr);
	}
	else if((g_strcasecmp (sccor->lang,"Russian")==0) ||  (g_strcasecmp (sccor->lang,"Russian(phonetic)")==0)) 
	{
		if (!sccor->sven->keyboard->corr_ru)
			sccor->sven->keyboard->corr_ru = sven_cfg_new();
			
		sven_cfg_write_string(sccor->sven->keyboard->corr_ru,"orfo",misspel,corr);
	}
	print_key_list (sccor->list_model,&iter,misspel,corr);
}

static void configure_new_but (GtkWidget *wid,Sconfcorrect *sccor)
{
	GtkTreeIter iter;
	gchar *misspel;
	gchar *corr;
		
	gtk_list_store_prepend (GTK_LIST_STORE (sccor->list_model), &iter);
	gtk_tree_selection_select_iter(GTK_TREE_SELECTION (sccor->list_selection),&iter);

	misspel = g_strdup (gtk_entry_get_text (GTK_ENTRY (sccor->misspelled)));
 	corr =g_strdup (gtk_entry_get_text (GTK_ENTRY (sccor->correct)));

	 if ((g_strcasecmp (sccor->lang,"English")==0) || (g_strcasecmp (sccor->lang,"US/ASCII")==0) || (g_strcasecmp (sccor->lang,"en_US")==0) || (g_strcasecmp (sccor->lang,"Maltese (US layout)")==0))
	{
		if (!sccor->sven->keyboard->corr_en)
			sccor->sven->keyboard->corr_en = sven_cfg_new();
			
		sven_cfg_write_string(sccor->sven->keyboard->corr_en,"orfo",misspel,corr);
	}
	else if((g_strcasecmp (sccor->lang,"Russian")==0) ||  (g_strcasecmp (sccor->lang,"Russian(phonetic)")==0)) 
	{
		if (!sccor->sven->keyboard->corr_ru)
			sccor->sven->keyboard->corr_ru = sven_cfg_new();
			
		sven_cfg_write_string(sccor->sven->keyboard->corr_ru,"orfo",misspel,corr);
	}
	print_key_list (sccor->list_model,&iter,misspel,corr);
}

static GtkTreeModel * create_combo_menu_flag (Sconfcorrect *sccor)
{
        GtkTreeIter iter;
        GtkListStore *store;
	int i;

        store = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	
	sccor->lang=sccor->sven->sxkb->group2info[0]->name;
	for (i = 0; i < sccor->sven->sxkb->ngroups; i++) 
	{
	
		gtk_list_store_append (store, &iter);
        	gtk_list_store_set (store, &iter,0, new_pixbuf_flag(sccor->sven->sxkb->group2info[i]->name,18,12),1,sccor->sven->sxkb->group2info[i]->name,-1);
	}
        
	return GTK_TREE_MODEL (store);
}

static void lang_combo_focus (GtkEditable *editable,Sconfcorrect *sccor)
{
	gchar *lang;
	GtkTreeIter iterplug;
	
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(sccor->lang_combo),&iterplug);
	gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(sccor->lang_combo)), &iterplug, 1, &lang,-1);
	
	sccor->lang=lang;
	add_print_list(GTK_TREE_VIEW (sccor->list_tree_view),sccor,lang);
}

static void changed_toggled(GtkToggleButton *togglebutton,Sconfcorrect *sccor)
{
	
	 if ((g_strcasecmp (sccor->lang,"English")==0) || (g_strcasecmp (sccor->lang,"US/ASCII")==0) || (g_strcasecmp (sccor->lang,"en_US")==0) || (g_strcasecmp (sccor->lang,"Maltese (US layout)")==0))
	{
		sccor->corr_en_init=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (sccor->check_enable_lang));
	}
	else if((g_strcasecmp (sccor->lang,"Russian")==0) ||  (g_strcasecmp (sccor->lang,"Russian(phonetic)")==0)) 
	{
		sccor->corr_ru_init=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (sccor->check_enable_lang));
	}
}

GtkWidget* sconfcorrect_new (Sven *sven,gchar *name,int device)
{
	GtkWidget *widget;
	Sconfcorrect *sccor;
	
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell;
	GtkWidget *scrolled_win;
	GtkWidget *hbbox;
	GtkWidget *hbox,*box,*page_label,*vbox;
	GtkWidget *frame;
	GtkWidget *alignment;
	GtkWidget *label;
	GtkWidget *hbox1;
	GtkWidget *hbox2;
	GtkCellRenderer *renderer;
  
	g_return_val_if_fail (sven != NULL , NULL);
 

	widget = g_object_new (SCONFCORRECT_TYPE,NULL);
	box = GTK_WIDGET (widget);
	sccor = SCONFCORRECT (widget);
  
	sccor->lang="";
	sccor->sven=sven;
	sccor->corr_en_init=sccor->sven->keyboard->corr_en_init;
	sccor->corr_ru_init=sccor->sven->keyboard->corr_ru_init;
	
	page_label = gtk_label_new (g_strdup_printf("<span size=\"xx-large\"><b><u><i> %s \n</i></u></b></span>",name));
	gtk_widget_show (page_label);
	gtk_box_pack_start (GTK_BOX (box), page_label, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (page_label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (page_label), 0, 0);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (box), frame, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("<b>Select language</b>"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	hbox1 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox1);
	gtk_container_add (GTK_CONTAINER (alignment), hbox1);

	sccor->lang_combo = gtk_combo_box_new_with_model (create_combo_menu_flag (sccor));
	gtk_box_pack_start (GTK_BOX (hbox1), sccor->lang_combo, FALSE, TRUE, 0);
	
	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (sccor->lang_combo),renderer,TRUE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (sccor->lang_combo), renderer,"pixbuf", 0,NULL);
	
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (sccor->lang_combo),renderer,TRUE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (sccor->lang_combo), renderer,"text", 1,NULL);
	
	gtk_combo_box_set_active (GTK_COMBO_BOX (sccor->lang_combo),0);
	g_signal_connect ((gpointer) sccor->lang_combo, "changed", G_CALLBACK (lang_combo_focus),sccor);
	gtk_widget_show (sccor->lang_combo);

	sccor->check_enable_lang = gtk_check_button_new_with_mnemonic (_("enable autocorect for this language"));
	gtk_widget_show (sccor->check_enable_lang);
	 g_signal_connect (G_OBJECT (sccor->check_enable_lang), "toggled",G_CALLBACK (changed_toggled),sccor);
		gtk_box_pack_start (GTK_BOX (hbox1), sccor->check_enable_lang, FALSE, FALSE, 0);

	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (box), frame, TRUE,TRUE, 0);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (frame), vbox);

	hbox2 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox2);
	gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE,FALSE, 0);

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (hbox2), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);
	
	label = gtk_label_new (_("misspelled"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

	sccor->misspelled = gtk_entry_new ();
	gtk_widget_show (sccor->misspelled);
	gtk_container_add (GTK_CONTAINER (alignment), sccor->misspelled);

	

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (hbox2), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

	label = gtk_label_new (_("correct"));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

	sccor->correct = gtk_entry_new ();
	gtk_widget_show (sccor->correct);
	gtk_container_add (GTK_CONTAINER (alignment), sccor->correct);

	
 	sccor->list_model = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);

	sccor->list_tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (sccor->list_model));
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (sccor->list_tree_view), TRUE);
	sccor->list_selection = G_OBJECT (gtk_tree_view_get_selection (GTK_TREE_VIEW (sccor->list_tree_view)));
	g_object_unref (G_OBJECT (sccor->list_model));
	gtk_tree_selection_set_mode (GTK_TREE_SELECTION (sccor->list_selection), GTK_SELECTION_SINGLE);

	g_signal_connect (G_OBJECT (sccor->list_selection),"changed",G_CALLBACK (prefs_list_clicked), sccor);
	
	
	scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled_win),  sccor->list_tree_view);
	gtk_container_border_width(GTK_CONTAINER(scrolled_win), 5);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_win), 5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	 gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE,TRUE, 0);
	 
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE,FALSE, 0);

	hbbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbbox), GTK_BUTTONBOX_START);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(hbbox), 10);
	gtk_button_box_set_child_size(GTK_BUTTON_BOX(hbbox), 85, 17);
	gtk_box_pack_start(GTK_BOX(hbox), hbbox, TRUE, TRUE, 0);

	sccor->prefs_new_wid = gtk_button_new_from_stock ("gtk-add");
	gtk_signal_connect (GTK_OBJECT(sccor->prefs_new_wid),"clicked",(GtkSignalFunc) configure_new_but,sccor);
	
	gtk_box_pack_start(GTK_BOX(hbbox),sccor->prefs_new_wid, TRUE, TRUE, 0);

	sccor->prefs_edit_wid=gtk_button_new_with_mnemonic (_("Modify"));
	gtk_signal_connect (GTK_OBJECT (sccor->prefs_edit_wid), "clicked",(GtkSignalFunc) configure_modifer_but, sccor);
	gtk_box_pack_start(GTK_BOX(hbbox), sccor->prefs_edit_wid, TRUE, TRUE, 0);

	sccor->prefs_del_wid = gtk_button_new_from_stock ("gtk-delete");
	g_signal_connect(G_OBJECT(sccor->prefs_del_wid), "clicked", (GtkSignalFunc) configure_dele_cb_but, sccor);
	gtk_box_pack_start(GTK_BOX(hbbox), sccor->prefs_del_wid, TRUE, TRUE, 0);

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (sccor->list_tree_view), FALSE);
	
		 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Misspelled"), cell, "text", 0, NULL);
		  gtk_tree_view_column_set_resizable (column, FALSE);
		  gtk_tree_view_column_set_sort_column_id (column, 0);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (sccor->list_tree_view), column);

		 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Correct"), cell, "text",1, NULL);
		  gtk_tree_view_column_set_resizable (column, FALSE);
		  gtk_tree_view_column_set_sort_column_id (column, 1);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (sccor->list_tree_view), column);
	  gtk_widget_show(sccor->list_tree_view);
		 
	hbbox = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (box), hbbox, FALSE, FALSE, 0);
	
	add_print_list(GTK_TREE_VIEW (sccor->list_tree_view),sccor,sccor->sven->sxkb->group2info[sccor->sven->sxkb->cur_group]->name);
	
	return GTK_WIDGET (widget);
}

static void print_key_list (GtkListStore *list_model,GtkTreeIter *iter,gchar *key,gchar *value)
{
	gtk_list_store_set (list_model,iter,0,key,1,value,-1);
}


void sconfcorrect_save (Sconfcorrect *sccor)
{
	ConfigFile *cfgfile;
	gchar *filename;

	filename= g_strconcat(g_get_home_dir(),"/.sven/en.cor", NULL);
	sven_cfg_write_file(sccor->sven->keyboard->corr_en, filename);
	g_free(filename);
	
	filename= g_strconcat(g_get_home_dir(),"/.sven/ru.cor", NULL);
	sven_cfg_write_file(sccor->sven->keyboard->corr_ru, filename);
	g_free(filename);
	
	sccor->sven->keyboard->corr_en_init=sccor->corr_en_init;
	sccor->sven->keyboard->corr_ru_init=sccor->corr_ru_init;

	filename= g_strconcat(g_get_home_dir(),"/.sven/config", NULL);
	cfgfile = sven_cfg_open_file(filename);

	if (!cfgfile)
		cfgfile = sven_cfg_new();

	sven_cfg_write_int (cfgfile, "keyboard", "corr_en",sccor->sven->keyboard->corr_en_init);
	sven_cfg_write_int (cfgfile, "keyboard", "corr_ru",sccor->sven->keyboard->corr_ru_init);

	sven_cfg_write_file(cfgfile, filename);
	sven_cfg_free(cfgfile);
	g_free(filename);
}

