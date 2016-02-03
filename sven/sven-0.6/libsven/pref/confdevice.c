/* Sven multimedia keyboard deamon
 * confdevice.c
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: morenko@ukr.net
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

#include "../sven.h"
#include "../utils.h"
#include "../key.h"
#include "../mouse.h"
#include "../plugin.h"
#include "../xlib.h"
#include "confdevice.h"
#include "prefswin.h"

static char *def="def";
static char *cmd="cmd";
static char *so=".so";
static gchar *menu_tact=NULL;
static gchar *menu_tcom=NULL;

static void sconfdevice_class_init          (SconfdeviceClass *klass);
static void sconfdevice_init                (Sconfdevice      *scd);
static void print_key_list (GtkListStore *list_model,GtkTreeIter *iter,gchar *comment,gchar *code,gchar *command,gchar *text,gchar *type,gint enable);
static void configure_but_edit_new (GtkMenuItem *menuitem,Sconfdevice *scd);
static void configure_but_edit (GtkMenuItem *menuitem,Sconfdevice *scd);

static void gtk_ifactory_cb (gpointer data,guint action,GtkWidget *widget);
static void new_key_factory_cb (gpointer data,guint action,GtkWidget *widget);
GtkWidget *get_option_menu(Sconfdevice *scd);
static void option_menu_focus (GtkEditable     *editable,Sconfdevice *scd);

static GtkItemFactoryEntry menu_items[] =
{
 // { "/New simulete key",NULL,new_key_factory_cb,0,"<Item>" },

  {"/Text Utils",		NULL,	0,0,"<Branch>"},
  {"/Text Utils/Copy",	NULL,new_key_factory_cb,1,"<Item>"},
  {"/Text Utils/Cut",	NULL,new_key_factory_cb,2,"<Item>"},
  {"/Text Utils/Paste",	NULL,new_key_factory_cb,3,"<Item>"},
  {"/Text Utils/Undo",	NULL,new_key_factory_cb,4,"<Item>"},
  {"/Text Utils/Redo",	NULL,new_key_factory_cb,5,"<Item>"},
  {"/Text Utils/Find",	NULL,new_key_factory_cb,6,"<Item>"},
  {"/Text Utils/Select All",	NULL,new_key_factory_cb,7,"<Item>"},
  {"/File",		NULL,	0,0,"<Branch>"},
  {"/File/New",	NULL,new_key_factory_cb,8,"<Item>"},
  {"/File/Open",	NULL,new_key_factory_cb,9,"<Item>"},
  {"/File/Save",	NULL,new_key_factory_cb,10,"<Item>"},
  {"/File/Print",	NULL,new_key_factory_cb,11,"<Item>"},
  {"/File/Close",	NULL,new_key_factory_cb,12,"<Item>"},
  {"/File/Exit",	NULL,new_key_factory_cb,13,"<Item>"},

  {"/Mouse",		NULL,	0,0,"<Branch>"},
  {"/Mouse/Left",	NULL,new_key_factory_cb,14,"<Item>"},
  {"/Mouse/Center",	NULL,new_key_factory_cb,15,"<Item>"},
  {"/Mouse/Right",	NULL,new_key_factory_cb,16,"<Item>"},
  {"/Mouse/Scroll Up",	NULL,new_key_factory_cb,17,"<Item>"},
  {"/Mouse/Scroll Down",	NULL,new_key_factory_cb,18,"<Item>"},
  {"/Mouse/Scroll Left",	NULL,new_key_factory_cb,19,"<Item>"},
  {"/Mouse/Scroll Right",	NULL,new_key_factory_cb,20,"<Item>"},
  

  { "/Other",		NULL,	0,0,"<Branch>" },
};

static gint nmenu_items = sizeof (menu_items) / sizeof (menu_items[0]);


static void new_key_factory_cb (gpointer data,guint action,GtkWidget *widget)
{
	g_return_if_fail (widget != NULL);
	
	gchar **sub = g_strsplit(gtk_item_factory_path_from_widget (widget),"/",3);
	
	if(action==0)
	{
		menu_tact="def";
		menu_tcom="";
	}
	else if(action==1)
	{
		menu_tact="sem";
		menu_tcom="Control+c";
		
	}
	else if(action==2)
	{
		menu_tact="sem";
 		menu_tcom="Control+x";
	}
	else if(action==3)
	{
		menu_tact="sem";
		menu_tcom="Control+v";
	}
	else if(action==4)
	{
		menu_tact="sem";
		menu_tcom="Control+z";
	}
	else if(action==5)
	{
		menu_tact="sem";
		menu_tcom="Shift+Control+z";
	}
	else if(action==6)
	{
		menu_tact="sem";
		menu_tcom="Control+f";
	}
	else if(action==7)
	{
		menu_tact="sem";
		menu_tcom="Control+a";
	}
	else if(action==8)
	{
		menu_tact="sem";
		menu_tcom="Control+n";
	}
	else if(action==9)
	{
		menu_tact="sem";
		menu_tcom="Control+o";
	}
	else if(action==10)
	{
		menu_tact="sem";
		menu_tcom="Control+s";
	}
	else if(action==11)
	{
		menu_tact="sem";
		menu_tcom="Control+p";
	}
	else if(action==12)
	{
		menu_tact="sem";
		menu_tcom="Control+w";
	}
	else if(action==13)
	{
		menu_tact="sem";
		menu_tcom="Control+q";
	}
	else if(action==14)
	{
		menu_tact="sem_mouse";
		menu_tcom="mouse left";
	}
	else if(action==15)
	{
		menu_tact="sem_mouse";
		menu_tcom="mouse center";
	}
	else if(action==16)
	{
		menu_tact="sem_mouse";
		menu_tcom="mouse right";
	}
	else if(action==17)
	{
		menu_tact="sem_mouse";
		menu_tcom="scrool up";
	}
	else if(action==18)
	{
		menu_tact="sem_mouse";
		menu_tcom="scrool down";
	}
	else if(action==19)
	{
		menu_tact="sem_mouse";
		menu_tcom="scrool left";
	}
	else if(action==20)
	{
		menu_tact="sem_mouse";
		menu_tcom="scrool right";
	}
/*
  {"/Mouse/Left",	NULL,new_key_factory_cb,14,"<Item>"},
  {"/Mouse/Center",	NULL,new_key_factory_cb,15,"<Item>"},
  {"/Mouse/Right",	NULL,new_key_factory_cb,16,"<Item>"},
  {"/Mouse/Scroll Up",	NULL,new_key_factory_cb,17,"<Item>"},
  {"/Mouse/Scroll Down",	NULL,new_key_factory_cb,18,"<Item>"},
  {"/Mouse/Scroll Left",	NULL,new_key_factory_cb,19,"<Item>"},
  {"/Mouse/Scroll Right",	NULL,new_key_factory_cb,20,"<Item>"},
*/
	else
	{
		menu_tact="def";
		menu_tcom="";
	}
}



static void gtk_ifactory_cb (gpointer data,guint action,GtkWidget *widget)
{
	g_return_if_fail (widget != NULL);
	
	gchar **sub = g_strsplit(gtk_item_factory_path_from_widget (widget),"/",3);
	if(g_strcasecmp (sub[1],"Other")==0)
		menu_tcom=g_strconcat(sub[2],NULL);
	else
		menu_tcom=g_strconcat(sub[1],"-",sub[2],NULL);
	
	if(action==0)
	{
		menu_tact="def";
	}
	else if(action==1)
	{
		menu_tact=(char *) data;
	}
	else
	{
		menu_tact="def";
	}
}


static void option_menu_focus (GtkEditable     *editable,Sconfdevice *scd)
{
	if(menu_tcom==NULL)
		gtk_entry_set_text (GTK_ENTRY(scd->runs_spec_entry),"");
	else
		gtk_entry_set_text (GTK_ENTRY(scd->runs_spec_entry),menu_tcom);
	
	if(menu_tact==NULL)
		gtk_entry_set_text (GTK_ENTRY(scd->runs_spec_entry_type),"def");
	else
		gtk_entry_set_text (GTK_ENTRY(scd->runs_spec_entry_type),menu_tact);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (scd->runs_radio2), 1);
}



GtkWidget *get_option_menu(Sconfdevice *scd)
{
	GtkItemFactory *item_factory;
	GtkItemFactoryEntry entry;
	GtkWidget *option_menu;
	GSList *l;
	GSList *ll,*plug_func;
  
	/* Same again, not bothering with the accelerators */
	item_factory = gtk_item_factory_new (GTK_TYPE_OPTION_MENU, "<sven>",
						NULL);
	gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, NULL);
	option_menu = gtk_item_factory_get_widget(item_factory, "<sven>");
	
	for (l = scd->sven->plugin->list; l; l=l->next)
	{
		SvenPluginPriv *plug = (SvenPluginPriv *)l->data;
		if((plug!=NULL)&&(plug->func!=NULL))
		{
			if ((plug->func->get_function_list!=NULL)&&(plug->enable==1))
			{
				plug_func=plug->func->get_function_list(scd->sven);
				
				gchar **sub = g_strsplit((char *) plug_func->data,"-",2);
				if(sub[1]!=NULL)
				{
					entry.path = g_strconcat("/",sub[0],NULL);
					entry.accelerator = NULL;
					entry.callback = 0;
					entry.callback_action = 1;
					entry.item_type = "<Branch>";
					gtk_item_factory_create_item(item_factory, &entry, NULL, 1);
				}
				for (ll = plug_func; ll; ll = ll->next)
				{
					char *name=(char *) ll->data;
					gchar **menu = g_strsplit(name,"-",2);
					if(menu[1]==NULL)
						entry.path = g_strconcat("/Other/",menu[0],NULL);
					else
						entry.path = g_strconcat("/",menu[0],"/",menu[1],NULL);
					entry.accelerator = NULL;
					entry.callback = gtk_ifactory_cb;
					entry.callback_action = 1;
					entry.item_type = "<Item>";
					gtk_item_factory_create_item(item_factory, &entry, (gpointer) g_basename(plug->filename) , 1);
				}
			}
		}
	}
	return option_menu;
}

GType
sconfdevice_get_type (void)
{
  static GType scd_type = 0;

  if (!scd_type)
    {
      static const GTypeInfo scd_info =
      {
	sizeof (SconfdeviceClass),
	NULL,		/* base_init */
	NULL,		/* base_finalize */
	(GClassInitFunc) sconfdevice_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data */
	sizeof (Sconfdevice),
	0,		/* n_preallocs */
	(GInstanceInitFunc) sconfdevice_init,
      };

      scd_type = g_type_register_static (GTK_TYPE_VBOX, "Sconfdevice",
					    &scd_info, 0);
    }

  return scd_type;
}

static void
sconfdevice_class_init (SconfdeviceClass *klass)
{
  
}

static void
sconfdevice_init (Sconfdevice *scd)
{
scd->list_tree_view=NULL;
scd->list_model=NULL;
scd->list_selection=NULL;
scd->iter_count_new = 0;

scd->prefs_new_wid=NULL;
scd->prefs_del_wid=NULL;
scd->prefs_edit_wid=NULL;

scd->device=0;

scd->configure_win_button_edit=NULL;
scd->mouse_code=NULL;
scd->desc_entry=NULL;
scd->key_entry=NULL;
scd->text_entry=NULL;
scd->runs_def_entry=NULL;
scd->runs_def_entry_type=NULL;
scd->runs_prog_entry=NULL;
scd->runs_spec_entry=NULL;
scd->status_check=NULL;
scd->runs_radio=NULL;
scd->runs_radio1=NULL;
scd->runs_radio2=NULL;
}


int print_list_config(Sconfdevice *scd,GtkTreeView *list_tree_view,ConfigFile * cfg)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	ConfigSection *section;
	GList *list;
	
	if((scd==NULL)||(cfg==NULL)||(list_tree_view==NULL))
		return -1;

	list = cfg->sections;
	while (list)
	{
		int key_code=0;
		char str[1000];
		gchar *key=NULL;
		char *type="";
		int stat=0;
		gchar *description=NULL;
		gchar *command=NULL;
		gchar *comment=NULL;
		int enable=0;
	
		section = (ConfigSection *) list->data;

		gchar **key_stat = g_strsplit(section->name,":",2);
		key_code=atoi(key_stat[0]);
		if(key_stat[1]==NULL)
			stat=0;
		else
			stat=atoi(key_stat[1]);
	
		sven_cfg_read_string(cfg,section->name, "type",&type);
	
		if(g_strcasecmp (type,def)==0)
		{
			model = gtk_tree_view_get_model (list_tree_view);
			gtk_list_store_append (GTK_LIST_STORE (model), &iter);
		
			ModifierToString (stat, str);
			key=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
				(XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), key_code, 0)) != NULL)
				? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),key_code, 0))
				: g_strdup_printf ("%d",key_code));
	
			sven_cfg_read_string(cfg,section->name, "description",&description);
			sven_cfg_read_string(cfg,section->name, "command",&command);
			sven_cfg_read_string(cfg,section->name, "comment",&comment);
			sven_cfg_read_int(cfg,section->name, "enable",&enable);
			print_key_list (scd->list_model,&iter,description,key,command,comment,def,enable);
			g_free(key);
			g_free(description);
			g_free(command);
			g_free(comment);
		}
		else if(g_strcasecmp (type,cmd)==0)
		{
			model = gtk_tree_view_get_model (list_tree_view);
			gtk_list_store_append (GTK_LIST_STORE (model), &iter);
			
			ModifierToString (stat, str);
				key=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
				(XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), key_code, 0)) != NULL)
				? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),key_code, 0))
				: g_strdup_printf ("%d",key_code));

			sven_cfg_read_string(cfg,section->name, "description",&description);
			sven_cfg_read_string(cfg,section->name, "command",&command);
			sven_cfg_read_string(cfg,section->name, "comment",&comment);
			sven_cfg_read_int(cfg,section->name, "enable",&enable);
			print_key_list (scd->list_model,&iter,description,key,command,comment,cmd,enable);
			g_free(key);
			g_free(description);
			g_free(command);
			g_free(comment);
		}
			
		else if(g_strcasecmp (type,"sem")==0)
		{
			model = gtk_tree_view_get_model (list_tree_view);
			gtk_list_store_append (GTK_LIST_STORE (model), &iter);
			
			ModifierToString (stat, str);
				key=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
				(XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), key_code, 0)) != NULL)
				? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),key_code, 0))
				: g_strdup_printf ("%d",key_code));
			
			sven_cfg_read_string(cfg,section->name, "description",&description);
			sven_cfg_read_string(cfg,section->name, "command",&command);
			sven_cfg_read_string(cfg,section->name, "comment",&comment);
			sven_cfg_read_int(cfg,section->name, "enable",&enable);
			print_key_list (scd->list_model,&iter,description,key,command,comment,"sem",enable);
			g_free(key);
			g_free(description);
			g_free(command);
			g_free(comment);
		}
		else if(g_strcasecmp (type,"sem_mouse")==0)
		{
			model = gtk_tree_view_get_model (list_tree_view);
			gtk_list_store_append (GTK_LIST_STORE (model), &iter);
			
			ModifierToStringMouse (stat, str);
				key=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
				(XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), key_code, 0)) != NULL)
				? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),key_code, 0))
				: g_strdup_printf ("%d",key_code));
			
			sven_cfg_read_string(cfg,section->name, "description",&description);
			sven_cfg_read_string(cfg,section->name, "command",&command);
			sven_cfg_read_string(cfg,section->name, "comment",&comment);
			sven_cfg_read_int(cfg,section->name, "enable",&enable);
			print_key_list (scd->list_model,&iter,description,key,command,comment,"sem_mouse",enable);
			g_free(key);
			g_free(description);
			g_free(command);
			g_free(comment);
		}
		else if(g_strncasecmp (g_strndup(&type[strlen(type)-strlen(so)], strlen(so)),so,3)==0)
		{
			model = gtk_tree_view_get_model (list_tree_view);
			gtk_list_store_append (GTK_LIST_STORE (model), &iter);
				
			ModifierToString (stat, str);
			key=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
				(XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), key_code, 0)) != NULL)
				? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),key_code, 0))
				: g_strdup_printf ("%d",key_code));
		
			sven_cfg_read_string(cfg,section->name, "description",&description);
			sven_cfg_read_string(cfg,section->name, "command",&command);
			sven_cfg_read_string(cfg,section->name, "comment",&comment);
			sven_cfg_read_int(cfg,section->name, "enable",&enable);
			print_key_list (scd->list_model,&iter,description,key,command,comment,g_strdup(g_basename(type)),enable);
			g_free(key);
			g_free(description);
			g_free(command);
			g_free(comment);
		}
		else
		{
			printf("Bad config line:[%s]\n",type);
		}
		
		list = g_list_next(list);
	}
	return 0;
}

static void add_print_list(GtkTreeView *list_tree_view,Sconfdevice *scd)
{
	DEBUG_MSG(_("Start add_print_list\n"));

	 gtk_list_store_clear(GTK_LIST_STORE (scd->list_model));

	if(scd->device==0)
	{
		print_list_config(scd,list_tree_view,scd->sven->config->key);
	}
	else if(scd->device==1)
	{
		print_list_config(scd,list_tree_view,scd->sven->config->mouse);
	}
	gtk_widget_set_sensitive(GTK_WIDGET(scd->prefs_edit_wid), 0);
	gtk_widget_set_sensitive(GTK_WIDGET(scd->prefs_del_wid), 0);
}

static void prefs_list_clicked(GtkTreeSelection *list_selection,Sconfdevice *scd)
{
	GtkTreeIter iter;
	GtkTreeModel *model;


	if (gtk_tree_selection_get_selected(list_selection, &model, &iter)) 
	{
		gtk_widget_set_sensitive(GTK_WIDGET(scd->prefs_edit_wid), 1);
		gtk_widget_set_sensitive(GTK_WIDGET(scd->prefs_del_wid), 1);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(scd->prefs_edit_wid),0);
		gtk_widget_set_sensitive(GTK_WIDGET(scd->prefs_del_wid),0);
	}
}

static void configure_dele_cb_but (GtkWidget *wid,Sconfdevice *scd)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	gtk_tree_selection_get_selected(GTK_TREE_SELECTION (scd->list_selection), &model, &iter);
	gtk_list_store_remove(GTK_LIST_STORE (model),&iter);
}
static gboolean list_event_mouseclick(GtkWidget *widget,GdkEventButton *event,Sconfdevice *scd) 
{

	if (event->button == 1 && event->type == GDK_2BUTTON_PRESS) 
	{	/* double click  */
		configure_but_edit(NULL,scd);
		return TRUE;
	}
	return FALSE;
}

static void fixed_toggled (GtkCellRendererToggle *cell,gchar  *path_str,gpointer data)
{
	GtkTreeModel *model = (GtkTreeModel *)data;
	GtkTreeIter  iter;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	gboolean fixed;

	/* get toggled iter */
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter,5, &fixed, -1);

	/* do something with the value */
	fixed ^= 1;

	/* set new value */
	gtk_list_store_set (GTK_LIST_STORE (model), &iter,5, fixed, -1);

	/* clean up */
	gtk_tree_path_free (path);
}

GtkWidget* sconfdevice_new (Sven *sven,gchar *name,int device)
{
	GtkWidget *widget;
	Sconfdevice *scd;
	
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell;
	GtkWidget *scrolled_win;
	GtkWidget *hbbox;
	GtkWidget *hbox,*box;
  
	if(sven==NULL)
		return NULL;
	
	widget = g_object_new (SCONFDEVICE_TYPE,NULL);
	box = GTK_WIDGET (widget);
	scd = SCONFDEVICE (widget);
  
	//printf("Start create_list_gui\n");

	scd->device=device;
	scd->sven=sven;
	
	gtk_box_pack_start (GTK_BOX (box), create_header(_(name)), FALSE, FALSE, 0);
	
	scd->list_model = gtk_list_store_new (6, G_TYPE_STRING, G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_BOOLEAN);

	scd->list_tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (scd->list_model));
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (scd->list_tree_view), TRUE);
	scd->list_selection = G_OBJECT (gtk_tree_view_get_selection (GTK_TREE_VIEW (scd->list_tree_view)));
	g_object_unref (G_OBJECT (scd->list_model));
	gtk_tree_selection_set_mode (GTK_TREE_SELECTION (scd->list_selection), GTK_SELECTION_SINGLE);

	g_signal_connect (G_OBJECT (scd->list_selection),"changed",G_CALLBACK (prefs_list_clicked), scd);
	g_signal_connect(G_OBJECT(scd->list_tree_view), "button-press-event",G_CALLBACK(list_event_mouseclick), scd);

	
	scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled_win),  scd->list_tree_view);
	gtk_container_border_width(GTK_CONTAINER(scrolled_win), 5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(box), scrolled_win, TRUE, TRUE, 0);
	
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 5);

	hbbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbbox), GTK_BUTTONBOX_START);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(hbbox), 10);
	gtk_button_box_set_child_size(GTK_BUTTON_BOX(hbbox), 85, 17);
	gtk_box_pack_start(GTK_BOX(hbox), hbbox, TRUE, TRUE, 0);

	scd->prefs_new_wid = gtk_button_new_from_stock ("gtk-new");
	gtk_signal_connect (GTK_OBJECT(scd->prefs_new_wid),"clicked",(GtkSignalFunc) configure_but_edit_new,scd);
	
	gtk_box_pack_start(GTK_BOX(hbbox),scd->prefs_new_wid, TRUE, TRUE, 0);

	scd->prefs_edit_wid=gtk_button_new_from_stock ("gtk-properties");
	gtk_signal_connect (GTK_OBJECT (scd->prefs_edit_wid), "clicked",(GtkSignalFunc) configure_but_edit, scd);
	gtk_box_pack_start(GTK_BOX(hbbox), scd->prefs_edit_wid, TRUE, TRUE, 0);

	scd->prefs_del_wid = gtk_button_new_from_stock ("gtk-delete");
	g_signal_connect(G_OBJECT(scd->prefs_del_wid), "clicked", (GtkSignalFunc) configure_dele_cb_but, scd);
	gtk_box_pack_start(GTK_BOX(hbbox), scd->prefs_del_wid, TRUE, TRUE, 0);

		 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Description"), cell, "text", 0, NULL);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_column_set_sort_column_id (column, 0);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (scd->list_tree_view), column);

		 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Key Code"), cell, "text",1, NULL);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_column_set_sort_column_id (column, 1);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (scd->list_tree_view), column);

		 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Command"), cell, "text", 2, NULL);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_column_set_sort_column_id (column, 2);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (scd->list_tree_view), column);

		 cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Text Display"), cell, "text", 3, NULL);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_column_set_sort_column_id (column,3);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (scd->list_tree_view), column);
		  
		cell = gtk_cell_renderer_text_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Info"), cell, "text", 4, NULL);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_column_set_sort_column_id (column,4);
		  gtk_tree_view_column_set_visible(column,0);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (scd->list_tree_view), column);

		cell = gtk_cell_renderer_toggle_new ();
		  column = gtk_tree_view_column_new_with_attributes (_("Status"), cell, "active",5, NULL);
		  gtk_tree_view_column_set_clickable (column, TRUE);
		  g_signal_connect (G_OBJECT (cell), "toggled",G_CALLBACK (fixed_toggled), scd->list_model);
		  gtk_tree_view_column_set_resizable (column, TRUE);
		  gtk_tree_view_append_column (GTK_TREE_VIEW (scd->list_tree_view), column);

		  gtk_widget_show(scd->list_tree_view);
		 
	hbbox = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (box), hbbox, FALSE, FALSE, 0);
	
	add_print_list(GTK_TREE_VIEW (scd->list_tree_view),scd);
	
	return GTK_WIDGET (widget);
}

static void print_key_list (GtkListStore *list_model,GtkTreeIter *iter,gchar *comment,gchar *code,gchar *command,gchar *text,gchar *type,gint enable)
{
	gtk_list_store_set (list_model,iter,0,comment,1,code,2,command,3,text,4,type,5,enable,-1);
}

/*--------------------------------------------------------------------------------*/
static void runs_prog_entry_focus  (GtkEditable     *editable,Sconfdevice *scd)
{
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (scd->runs_radio1), 1);
}

/*********************************Start Mouse events************************************/
GdkFilterReturn events_mouse (GdkXEvent *xevent, GdkEvent *event,gpointer data)
{
	Sconfdevice *scd=(Sconfdevice *) data;
	XEvent *xev = (XEvent *) xevent;
	char str[1000];
		
	if (xev->type == ButtonPress)
	{
		ModifierToStringMouse(xev->xbutton.state,str);
		DEBUG_MSG("state:%d\n button:%d\n",xev->xbutton.state,xev->xbutton.button);
		gtk_label_set_markup(GTK_LABEL(scd->mouse_code),g_strdup_printf ("<span size=\"large\" color=\"#098203\">%s%s%d</span>",str,str[0] ? "+" : "",xev->xbutton.button/*GetNameMouseButton(xev->xbutton.button)*/));
	
		return GDK_FILTER_REMOVE;
	}

	return GDK_FILTER_CONTINUE;
}

gboolean dest_win_mous_event_ok  (GtkObject *object,Sconfdevice *scd)
{
	char *test;
	gdk_pointer_ungrab(CurrentTime);
	gdk_window_remove_filter(gdk_get_default_root_window (),events_mouse,(gpointer) scd);
	gtk_label_get(GTK_LABEL (scd->mouse_code),&test);
	gtk_entry_set_text (GTK_ENTRY (scd->key_entry),test);
	gtk_widget_destroy(GTK_WIDGET(SCONFDEVICE(scd)->configure_win_mouse));
	SCONFDEVICE(scd)->configure_win_mouse=NULL;

  return FALSE;
}
gboolean dest_win_mous_event  (GtkObject *object,Sconfdevice *scd)
{
	gdk_pointer_ungrab(CurrentTime);
	gdk_window_remove_filter(gdk_get_default_root_window (),events_mouse,(gpointer) scd);
	gtk_widget_destroy(GTK_WIDGET(SCONFDEVICE(scd)->configure_win_mouse));
	SCONFDEVICE(scd)->configure_win_mouse=NULL;

  return FALSE;
}

static void mouse_button_dialog_window (GtkButton * wbutton, Sconfdevice *scd)
{
  GtkWidget *vbox;
  GtkWidget *label;
  GtkWidget *hbuttonbox,*button;
 
  if (scd->configure_win_mouse)
		return ;
	
	scd->configure_win_mouse = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (scd->configure_win_mouse), _("Sven "VERSION" Configuration button mouse"));
	gtk_window_set_position (GTK_WINDOW (scd->configure_win_mouse), GTK_WIN_POS_MOUSE);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (scd->configure_win_mouse), TRUE);
	gtk_signal_connect (GTK_OBJECT (scd->configure_win_mouse), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroyed), &scd->configure_win_mouse);
	gtk_signal_connect (GTK_OBJECT (scd->configure_win_mouse), "destroy", (GtkSignalFunc) dest_win_mous_event, scd);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (scd->configure_win_mouse), vbox);

	label = gtk_label_new (_("<span size=\"large\" color=\"#1347ce\"><b>Press any buttons combination  for mouse</b></span>"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

	scd->mouse_code = gtk_label_new ("");
	gtk_widget_show (scd->mouse_code);
	gtk_box_pack_start (GTK_BOX (vbox), scd->mouse_code, TRUE, FALSE, 0);
	gtk_widget_set_size_request (scd->mouse_code, -1, 200);
  	gtk_label_set_line_wrap (GTK_LABEL (scd->mouse_code), TRUE);
  
  
	hbuttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, FALSE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
	
	button = gtk_button_new_from_stock ("gtk-ok");
	gtk_widget_show (button);
	gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	g_signal_connect(button, "clicked", G_CALLBACK(dest_win_mous_event_ok),scd);
	
	button = gtk_button_new_from_stock ("gtk-cancel");
	gtk_widget_show (button);
	gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	g_signal_connect(button, "clicked", G_CALLBACK(dest_win_mous_event), scd);

	gtk_widget_show (scd->configure_win_mouse);

  
   
	gdk_pointer_grab (gdk_get_default_root_window (), TRUE,GDK_BUTTON_PRESS_MASK,NULL, NULL, GDK_CURRENT_TIME);
	gdk_window_add_filter (gdk_get_default_root_window (),events_mouse,(gpointer) scd);

}
/*********************************END Mouse events************************************/
/********************************Start Keyboard events********************************/
static void configure_press_key (GtkWidget *widget,GdkEventKey *event,Sconfdevice *scd)
{
	char str[1000];
	char *mkey;

	//printf("Start\n");
	//printf("keyval=%d,\n hardware_keycode=%d\nstate=%d\nlength=%d\nstring=%s\n group=%d\n",event->keyval, event->hardware_keycode,event->state,event->length,event->string,event->group);
	//printf("________________________________________________________________________________________\n");
	//printf("End\n");
	ModifierToString (event->state, str);

	mkey=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
		      (XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), event->hardware_keycode, 0)) != NULL)
		      ? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),event->hardware_keycode, 0))
		      : g_strdup_printf ("%d",event->hardware_keycode));

	gtk_entry_set_text (GTK_ENTRY (scd->key_entry),mkey);
	g_free(mkey);
}
static void configure_release_key (GtkWidget *widget,GdkEventKey *event,Sconfdevice *scd)
{
	guint i = 0;
	char **key_stat = g_strsplit(g_strdup(gtk_entry_get_text (GTK_ENTRY (scd->key_entry))),"+",4);

	while (key_stat[i])
		i++;

	if((g_strcasecmp (key_stat[i-1],"Control")==0)|| (g_strcasecmp (key_stat[i-1],"Shift")==0)|| (g_strcasecmp (key_stat[i-1],"Alt")==0)|| (g_strcasecmp (key_stat[i-1],"Control_R")==0)|| (g_strcasecmp (key_stat[i-1],"Shift_R")==0)|| (g_strcasecmp (key_stat[i-1],"Alt_R")==0)|| (g_strcasecmp (key_stat[i-1],"Control_L")==0)|| (g_strcasecmp (key_stat[i-1],"Shift_L")==0)|| (g_strcasecmp (key_stat[i-1],"Alt_l")==0))
	gtk_entry_set_text (GTK_ENTRY (scd->key_entry),_("Press any key"));
}

static void file_dialog_ok (GtkButton * button,Sconfdevice *scd)
{
	char *file_name=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(scd->filechooser));
	gtk_entry_set_text (GTK_ENTRY(scd->runs_prog_entry), file_name);
	gtk_widget_destroy (scd->filechooser);
}

static void file_dialog_window (GtkButton * button, Sconfdevice *scd)
{
	GtkWidget *dialog_vbox1;
	GtkWidget *dialog_action_area1;
	GtkWidget *button_cancel;
	GtkWidget *button_ok;
  
	scd->filechooser = gtk_file_chooser_dialog_new ("", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, NULL, NULL);
	gtk_window_set_type_hint (GTK_WINDOW (scd->filechooser), GDK_WINDOW_TYPE_HINT_DIALOG);

	dialog_vbox1 = GTK_DIALOG (scd->filechooser)->vbox;
	gtk_widget_show (dialog_vbox1);

	dialog_action_area1 = GTK_DIALOG (scd->filechooser)->action_area;
	gtk_widget_show (dialog_action_area1);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);
	 
	button_cancel = gtk_button_new_from_stock ("gtk-cancel");
	gtk_widget_show (button_cancel);
	gtk_dialog_add_action_widget (GTK_DIALOG (scd->filechooser), button_cancel, GTK_RESPONSE_CANCEL);
	GTK_WIDGET_SET_FLAGS (button_cancel, GTK_CAN_DEFAULT);

	button_ok = gtk_button_new_from_stock ("gtk-open");
	gtk_widget_show (button_ok);
	gtk_dialog_add_action_widget (GTK_DIALOG (scd->filechooser), button_ok, GTK_RESPONSE_OK);
	GTK_WIDGET_SET_FLAGS (button_ok, GTK_CAN_DEFAULT);
  
	gtk_signal_connect_object (GTK_OBJECT (button_cancel), "clicked",
			     GTK_SIGNAL_FUNC (gtk_widget_destroy),
			     GTK_OBJECT (scd->filechooser));

	gtk_signal_connect (GTK_OBJECT (button_ok), "clicked",
	      GTK_SIGNAL_FUNC (file_dialog_ok),scd);
	gtk_widget_grab_default (button_ok);
  
	gtk_widget_show_all (scd->filechooser);
}


/***********************END Keyboard events*******************************************/
static void greate_dialog_config (GtkWidget *vbox1,Sconfdevice *scd)
{
	GtkWidget *vbox;
	GtkWidget *frame;
	GtkWidget *alignment;
	GtkWidget *table1;
	GtkWidget *label;
	GtkWidget *table2;
	GtkWidget *hbox2;
	GtkWidget *addprog,*optmenu;
	GtkCellRenderer *renderer;
	GSList *runs_radio_group=NULL;

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_box_pack_start (GTK_BOX (vbox1), vbox, TRUE, TRUE, 0);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	//gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
	
	if(scd->device==0)
		label = gtk_label_new (g_strdup_printf("<span size=\"xx-large\" color=\"blue\"><b>%s</b></span>",_("Configure key")));
	else if(scd->device==1)
		label = gtk_label_new (g_strdup_printf("<span size=\"xx-large\" color=\"blue\"><b>%s</b></span>",_("Configure Mouse")));

	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	table1 = gtk_table_new (6, 1, FALSE);
	gtk_widget_show (table1);
	gtk_container_add (GTK_CONTAINER (alignment), table1);

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_table_attach (GTK_TABLE (table1), frame, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

	scd->desc_entry = gtk_entry_new ();
	gtk_widget_show (scd->desc_entry);
	gtk_container_add (GTK_CONTAINER (alignment), scd->desc_entry);

	label = gtk_label_new (g_strdup_printf("<b>%s</b>",_("Description")));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_table_attach (GTK_TABLE (table1), frame, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);

	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);

	if(scd->device==0)
	{
		scd->key_entry = gtk_entry_new ();
		gtk_widget_show (scd->key_entry);
		gtk_signal_connect (GTK_OBJECT (scd->key_entry), "key_press_event",
				GTK_SIGNAL_FUNC (configure_press_key),scd);
	
		gtk_signal_connect (GTK_OBJECT (scd->key_entry), "key_release_event",
			GTK_SIGNAL_FUNC (configure_release_key),scd);
		gtk_container_add (GTK_CONTAINER (alignment), scd->key_entry);

		label = gtk_label_new (g_strdup_printf("<b>%s</b>",_("Key code")));
		gtk_widget_show (label);
		gtk_frame_set_label_widget (GTK_FRAME (frame), label);
		gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	}
	else if(scd->device==1)
	{
		hbox2 = gtk_hbox_new (FALSE, 0);
		gtk_widget_show (hbox2);
		gtk_container_add (GTK_CONTAINER (alignment), hbox2);
		
		scd->key_entry = gtk_entry_new ();
		gtk_widget_show (scd->key_entry);
		gtk_box_pack_start (GTK_BOX (hbox2), scd->key_entry, TRUE, TRUE, 0);
		
		addprog = gtk_button_new_with_mnemonic ("Add mouse\n    button");
		gtk_widget_show (addprog);
		gtk_signal_connect (GTK_OBJECT (addprog), "clicked",GTK_SIGNAL_FUNC (mouse_button_dialog_window), scd);
		gtk_box_pack_start (GTK_BOX (hbox2), addprog, FALSE, FALSE, 0);
		
		label = gtk_label_new (g_strdup_printf("<b>%s</b>",_("Button mouse code")));
		gtk_widget_show (label);
		gtk_frame_set_label_widget (GTK_FRAME (frame), label);
		gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	}
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_table_attach (GTK_TABLE (table1), frame, 0, 1, 4, 5,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	scd->status_check = gtk_check_button_new_with_mnemonic (_("Enable Key"));
	gtk_widget_show (scd->status_check);
	gtk_container_add (GTK_CONTAINER (alignment), scd->status_check);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (scd->status_check), TRUE);
	
	label = gtk_label_new (g_strdup_printf("<b>%s</b>",_("Status")));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	label = gtk_hseparator_new ();
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table1), label, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_table_attach (GTK_TABLE (table1), frame, 0, 1, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	
	alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment);
	gtk_container_add (GTK_CONTAINER (frame), alignment);
	gtk_container_set_border_width (GTK_CONTAINER (alignment), 9);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 0, 0, 12, 0);
	
	scd->text_entry = gtk_entry_new ();
	gtk_widget_show (scd->text_entry);
	gtk_container_add (GTK_CONTAINER (alignment), scd->text_entry);
	
	label = gtk_label_new (g_strdup_printf("<b>%s</b>",_("Text Display")));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	frame = gtk_frame_new (NULL);
	gtk_widget_show (frame);
	gtk_table_attach (GTK_TABLE (table1), frame, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	table2 = gtk_table_new (3, 2, FALSE);
	gtk_widget_show (table2);
	gtk_container_add (GTK_CONTAINER (frame), table2);
	gtk_container_set_border_width (GTK_CONTAINER (table2), 5);
	
	scd->runs_radio = gtk_radio_button_new_with_mnemonic (NULL, _("Default setting:"));
	gtk_widget_show (scd->runs_radio);
	gtk_table_attach (GTK_TABLE (table2), scd->runs_radio, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
	gtk_radio_button_set_group (GTK_RADIO_BUTTON (scd->runs_radio), runs_radio_group);
	runs_radio_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (scd->runs_radio));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (scd->runs_radio), TRUE);
	
	scd->runs_radio1 = gtk_radio_button_new_with_mnemonic (NULL, _("Program:"));
	gtk_widget_show (scd->runs_radio1);
	gtk_table_attach (GTK_TABLE (table2), scd->runs_radio1, 0, 1, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_radio_button_set_group (GTK_RADIO_BUTTON (scd->runs_radio1), runs_radio_group);
	runs_radio_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (scd->runs_radio1));
	
	scd->runs_radio2 = gtk_radio_button_new_with_mnemonic (NULL, _("Special action:"));
	gtk_widget_show (scd->runs_radio2);
	gtk_table_attach (GTK_TABLE (table2), scd->runs_radio2, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
	gtk_radio_button_set_group (GTK_RADIO_BUTTON (scd->runs_radio2), runs_radio_group);
	runs_radio_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (scd->runs_radio2));
	
	scd->runs_def_entry = gtk_entry_new ();
	gtk_widget_show (scd->runs_def_entry);
	gtk_editable_set_editable (GTK_EDITABLE (scd->runs_def_entry), FALSE);
	scd->runs_def_entry_type = gtk_entry_new ();
	gtk_widget_show (scd->runs_def_entry_type);
	gtk_table_attach (GTK_TABLE (table2), scd->runs_def_entry, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

	hbox2 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox2);
	
	scd->runs_spec_entry = gtk_entry_new ();
	gtk_widget_show (scd->runs_spec_entry);
	scd->runs_spec_entry_type = gtk_entry_new ();
	//gtk_editable_set_editable (GTK_EDITABLE (scd->runs_spec_entry), FALSE);

	optmenu=get_option_menu(scd);
	g_signal_connect ((gpointer) optmenu, "changed", G_CALLBACK (option_menu_focus),scd);
	gtk_box_pack_start (GTK_BOX (hbox2), scd->runs_spec_entry, TRUE, TRUE, 0);

	gtk_box_pack_start (GTK_BOX (hbox2),optmenu, FALSE, FALSE, 0);
	
  	gtk_table_attach (GTK_TABLE (table2), hbox2, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);


	hbox2 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox2);
	gtk_table_attach (GTK_TABLE (table2), hbox2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

	scd->runs_prog_entry = gtk_entry_new ();
	gtk_widget_show (scd->runs_prog_entry);
	g_signal_connect ((gpointer) scd->runs_prog_entry, "changed", G_CALLBACK (runs_prog_entry_focus),scd);
	gtk_box_pack_start (GTK_BOX (hbox2), scd->runs_prog_entry, TRUE, TRUE, 0);

	addprog = gtk_button_new_from_stock ("gtk-add");
	gtk_widget_show (addprog);
	gtk_box_pack_start (GTK_BOX (hbox2), addprog, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (addprog), "clicked",
			GTK_SIGNAL_FUNC (file_dialog_window), scd);
	
	label = gtk_label_new (g_strdup_printf("<b>%s</b>",_("Runs")));
	gtk_widget_show (label);
	gtk_frame_set_label_widget (GTK_FRAME (frame), label);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	
	label = gtk_hseparator_new ();
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);
} 

static void configure_but_edit_destroy (GtkWidget *wid,Sconfdevice *scd)
{
	if(GTK_WIDGET(SCONFDEVICE(scd)->configure_win_button_edit)!=NULL)
		gtk_widget_destroy (GTK_WIDGET(SCONFDEVICE(scd)->configure_win_button_edit));
	
	SCONFDEVICE(scd)->configure_win_button_edit = NULL;
	scd->mouse_code=NULL;
	scd->desc_entry=NULL;
	scd->key_entry=NULL;
	scd->text_entry=NULL;
	scd->runs_def_entry=NULL;
	scd->runs_def_entry_type=NULL;
	scd->runs_prog_entry=NULL;
	scd->runs_spec_entry=NULL;
	scd->status_check=NULL;
	scd->runs_radio=NULL;
	scd->runs_radio1=NULL;
	scd->runs_radio2=NULL;
	
	scd->iter_count_new=0;
}


static void configure_apply_but_edit (GtkWidget *wid,Sconfdevice *scd)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	int t=0;
	gchar *description;
	gchar *key_code;
	gchar *command;
	gchar *info="cmd";
	gchar *text_disp;
	gint status;
	GtkTreeIter iterplug;


	t=gtk_tree_selection_get_selected(GTK_TREE_SELECTION (scd->list_selection), &model, &iter);
	/*Not selected*/
	if(t==0)
	{
		gtk_list_store_append (GTK_LIST_STORE (scd->list_model), &iter);
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION (scd->list_selection),&iter);
	}
	
	description = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->desc_entry)));
 	key_code =g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->key_entry)));
	text_disp = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->text_entry)));
	status=GTK_TOGGLE_BUTTON(scd->status_check)->active;

	if(GTK_TOGGLE_BUTTON(scd->runs_radio)->active)
	{
 		command = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_def_entry)));
		info = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_def_entry_type)));
	}
	else if(GTK_TOGGLE_BUTTON(scd->runs_radio1)->active)
	{
		command = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_prog_entry)));
		info="cmd";
	}
	else if(GTK_TOGGLE_BUTTON(scd->runs_radio2)->active)
	{
 		command = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_spec_entry)));
		info = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_spec_entry_type)));
	}
	else
	{
		command = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_def_entry)));
		info = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_def_entry_type)));
	}
	if(command!=NULL && info!=NULL)
	print_key_list (scd->list_model,&iter,description,key_code,command,text_disp,info,status);
}
static void configure_apply_but_edit_new (GtkWidget *wid,Sconfdevice *scd)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	int t=0;
	gchar *description;
	gchar *key_code;
	gchar *command;
	gchar *info="cmd";
	gchar *text_disp;
	gint status;
	GtkTreeIter iterplug;

	t=gtk_tree_selection_get_selected(GTK_TREE_SELECTION (scd->list_selection), &model, &iter);
	/*Not selected*/
	if(t==0)
	{
		//gtk_list_store_append (GTK_LIST_STORE (list_model), &iter);
		gtk_list_store_append (GTK_LIST_STORE (scd->list_model), &iter);
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION (scd->list_selection),&iter);
	}
	else if (t>=0 && scd->iter_count_new==0)
	{
		gtk_list_store_append (GTK_LIST_STORE (scd->list_model), &iter);
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION (scd->list_selection),&iter);
	}
	
	scd->iter_count_new = 1;
	
	description = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->desc_entry)));
 	key_code =g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->key_entry)));
	text_disp = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->text_entry)));
	status=GTK_TOGGLE_BUTTON(scd->status_check)->active;

	if(GTK_TOGGLE_BUTTON(scd->runs_radio)->active)
	{
 		command = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_def_entry)));
		info = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_def_entry_type)));
	}
	else if(GTK_TOGGLE_BUTTON(scd->runs_radio1)->active)
	{
		command = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_prog_entry)));
		info="cmd";
	}
	else if(GTK_TOGGLE_BUTTON(scd->runs_radio2)->active)
	{
 		command = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_spec_entry)));
		info = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_spec_entry_type)));
	}
	else
	{
		command = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_def_entry)));
		info = g_strdup (gtk_entry_get_text (GTK_ENTRY (scd->runs_def_entry_type)));
	}
	print_key_list (scd->list_model,&iter,description,key_code,command,text_disp,info,status);
}

static void configure_ok_but_edit (GtkWidget *wid,Sconfdevice *scd)
{
	configure_apply_but_edit(NULL,scd);
	scd->iter_count_new = 0;
	configure_but_edit_destroy(NULL,scd);
}

static void configure_ok_but_edit_new (GtkWidget *wid,Sconfdevice *scd)
{
	configure_apply_but_edit_new(NULL,scd);
	scd->iter_count_new = 0;
	configure_but_edit_destroy(NULL,scd);
}


static void new_command_but(GtkWidget *vbox,Sconfdevice *scd,int i)
{
	GtkWidget  *bbox, *ok, *cancel, *apply, *sep;
	
	scd->iter_count_new = 0;
	
	sep = gtk_hseparator_new ();
	gtk_box_pack_start (GTK_BOX (vbox), sep, FALSE, FALSE, 0);

	/* Command Buttons */
	bbox = gtk_hbutton_box_new ();
	gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (bbox), 5);
	gtk_box_pack_start (GTK_BOX (vbox), bbox, FALSE, FALSE, 0);
  
	ok =  gtk_button_new_from_stock ("gtk-ok");
	if(i==1)
		gtk_signal_connect (GTK_OBJECT (ok), "clicked",GTK_SIGNAL_FUNC (configure_ok_but_edit), scd);
	else
		gtk_signal_connect (GTK_OBJECT (ok), "clicked",GTK_SIGNAL_FUNC (configure_ok_but_edit_new), scd);
	GTK_WIDGET_SET_FLAGS (ok, GTK_CAN_DEFAULT);
	gtk_box_pack_start (GTK_BOX (bbox), ok, TRUE, TRUE, 0);
	gtk_widget_grab_default (ok);

	apply = gtk_button_new_from_stock ("gtk-apply");
	if(i==1)
		gtk_signal_connect (GTK_OBJECT (apply), "clicked",GTK_SIGNAL_FUNC (configure_apply_but_edit), scd);
	else
		gtk_signal_connect (GTK_OBJECT(apply),"clicked",GTK_SIGNAL_FUNC(configure_apply_but_edit_new), scd);
	
	GTK_WIDGET_SET_FLAGS (apply, GTK_CAN_DEFAULT);
	gtk_box_pack_start (GTK_BOX (bbox), apply, TRUE, TRUE, 0);

	cancel = gtk_button_new_from_stock ("gtk-cancel");
	gtk_signal_connect (GTK_OBJECT (cancel), "clicked",GTK_SIGNAL_FUNC(configure_but_edit_destroy), scd);
	GTK_WIDGET_SET_FLAGS (cancel, GTK_CAN_DEFAULT);
	gtk_box_pack_start (GTK_BOX (bbox), cancel, TRUE, TRUE, 0);
	gtk_widget_show_all (bbox);
}


static GtkWidget *new_wind_conf(Sconfdevice *scd)
{
	GtkWidget *vbox;

	if (scd->configure_win_button_edit)
		return NULL;

	scd->configure_win_button_edit = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT (scd->configure_win_button_edit), "destroy",GTK_SIGNAL_FUNC (gtk_widget_destroyed), &scd->configure_win_button_edit);
	/*gtk_signal_connect (GTK_OBJECT (scd->configure_win_button_edit), "destroy", GTK_SIGNAL_FUNC( configure_but_edit_destroy), scd);*/
	gtk_window_set_title (GTK_WINDOW (scd->configure_win_button_edit),
			_("Sven "VERSION" Configuration button: Edit"));
	gtk_window_set_position (GTK_WINDOW (scd->configure_win_button_edit), GTK_WIN_POS_MOUSE);
	//gtk_window_set_destroy_with_parent (GTK_WINDOW (scd->configure_win_button_edit), 1);

	vbox = gtk_vbox_new (FALSE,0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (scd->configure_win_button_edit), vbox);
 
	return vbox;
}

static void configure_but_edit_new (GtkMenuItem *menuitem,Sconfdevice *scd)
{
	GtkWidget *vbox;
	vbox=new_wind_conf(scd);
	if(vbox)
	{
		greate_dialog_config (vbox,scd);
		new_command_but (vbox,scd,0);
		if(scd->device==0)
			gtk_entry_set_text (GTK_ENTRY (scd->key_entry),_("Press any key"));
		else if(scd->device==1)
			gtk_entry_set_text (GTK_ENTRY (scd->key_entry),_("Select any button"));
		gtk_widget_show_all (scd->configure_win_button_edit);
	}
}

static void configure_but_edit (GtkMenuItem *menuitem,Sconfdevice *scd)
{
	GtkWidget *vbox;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *description;
	gchar *key_code;
	gchar *command;
	gchar *info;
	gchar *text_disp;
	gint status;

	vbox=new_wind_conf(scd);
 
	if(vbox)
	{
		greate_dialog_config (vbox,scd);
		new_command_but (vbox,scd,1);
		
		gtk_tree_selection_get_selected(GTK_TREE_SELECTION (scd->list_selection), &model, &iter);
		gtk_tree_model_get(model, &iter, 0, &description,1, &key_code,2, &command,3, &text_disp,4, &info,5, &status, -1);
		
		gtk_entry_set_text (GTK_ENTRY (scd->desc_entry),description);
		gtk_entry_set_text (GTK_ENTRY (scd->key_entry),key_code);
		gtk_entry_set_text (GTK_ENTRY (scd->runs_def_entry),command);
		gtk_entry_set_text (GTK_ENTRY (scd->runs_def_entry_type),info);
		gtk_entry_set_text (GTK_ENTRY (scd->text_entry),text_disp);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(scd->status_check),status);
		
		gtk_widget_show_all (scd->configure_win_button_edit);
	}
}



void sconfdevice_save (Sconfdevice *scd)
{
	gchar *description;
	gchar *key_code;
	gchar *text_disp;
	gchar *command;
	gchar *info;
	gint enable;
	GtkTreeModel *model;
	GtkTreeIter  iter;
	GtkTreePath *path;
	gchar *filename;
	ConfigFile *cfgfile;

	
	if(scd->device==0)
	{
		filename= g_strconcat(g_get_home_dir(),"/.sven/key_config", NULL);
	
		DEBUG_MSG("save_config_key:%s\n",filename);
		
		model=gtk_tree_view_get_model(GTK_TREE_VIEW(scd->list_tree_view));
		path = gtk_tree_path_new_first ();
		if (gtk_tree_model_get_iter (model, &iter, path) == FALSE)
		{
			gtk_tree_path_free (path);
			update_kbd(scd->sven);
			sven_cfg_free_file(filename);
			sven_cfg_free(scd->sven->config->key);
			scd->sven->config->key = sven_cfg_open_file(filename);
			if (!scd->sven->config->key)
				scd->sven->config->key = sven_cfg_new();
			return;
		}
	
		sven_cfg_free(scd->sven->config->key);
		scd->sven->config->key = sven_cfg_new();

		do
		{
			int key;
			unsigned int mod;
			char *key_mod;

			gtk_tree_model_get(model,&iter, 0, &description,1, &key_code,2,&command,3, &text_disp,4,&info,5, &enable,-1);
	
			DEBUG_MSG("description:%s -- %s -- %s -- %s -- %s -- %d\n",description,key_code,command,text_disp,info,enable);
			key=StringToModifier_Key(key_code,&mod);
			DEBUG_MSG("Keys:%d -- Mod:%d\n",key,mod);
			key_mod=g_strdup_printf("%d:%d",key,mod);
			sven_cfg_write_string	(scd->sven->config->key,key_mod, "type",info);
			sven_cfg_write_int 	(scd->sven->config->key,key_mod, "enable",enable);
			sven_cfg_write_string 	(scd->sven->config->key,key_mod, "description",description);
			sven_cfg_write_string 	(scd->sven->config->key,key_mod, "command", command);
			sven_cfg_write_string 	(scd->sven->config->key,key_mod, "comment", text_disp);

			g_free(key_mod);	

			gtk_tree_path_next (path);
		}
		while (gtk_tree_model_iter_next (model,&iter));
	
		sven_cfg_free_file(filename);
		sven_cfg_write_file(scd->sven->config->key, filename);
		
		update_kbd(scd->sven);
		init_kbd (scd->sven);
		
		g_free(filename);
	}
	else if(scd->device==1)
	{
		filename= g_strconcat(g_get_home_dir(),"/.sven/mouse_config", NULL);

		DEBUG_MSG("save_config_mouse:%s\n",filename);
		model=gtk_tree_view_get_model(GTK_TREE_VIEW(scd->list_tree_view));
		path = gtk_tree_path_new_first ();
		if (gtk_tree_model_get_iter (model, &iter, path) == FALSE)
		{
			gtk_tree_path_free (path);
			update_mouse(scd->sven);
			sven_cfg_free_file(filename);
			sven_cfg_free(scd->sven->config->mouse);
			scd->sven->config->mouse = sven_cfg_open_file(filename);
			if (!scd->sven->config->mouse)
				scd->sven->config->mouse = sven_cfg_new();
			return;
		}
	
		sven_cfg_free(scd->sven->config->mouse);
		scd->sven->config->mouse = sven_cfg_new();
		
		do
		{
			int but;
			unsigned int mod;
			gchar *but_mod;
			gtk_tree_model_get(model,&iter, 0, &description,1, &key_code,2,&command,3, &text_disp,4,&info,5, &enable,-1);
	
			DEBUG_MSG("description:%s -- %s -- %s -- %s -- %s -- %d\n",description,key_code,command,text_disp,info,enable);
			but=StringToModifier_Mouse(key_code,&mod);
			DEBUG_MSG("Keys:%d -- Mod:%d\n",but,mod);
		
			but_mod=g_strdup_printf("%d:%d",but,mod);
			sven_cfg_write_string	(scd->sven->config->mouse,but_mod, "type",info);
			sven_cfg_write_int 	(scd->sven->config->mouse,but_mod, "enable",enable);
			sven_cfg_write_string 	(scd->sven->config->mouse,but_mod, "description",description);
			sven_cfg_write_string 	(scd->sven->config->mouse,but_mod, "command", command);
			sven_cfg_write_string 	(scd->sven->config->mouse,but_mod, "comment", text_disp);
	
			g_free(but_mod);

			gtk_tree_path_next (path);
		}
		while (gtk_tree_model_iter_next (model,&iter));

		sven_cfg_free_file(filename);
		sven_cfg_write_file(scd->sven->config->mouse, filename);

		update_mouse(scd->sven);
		init_mouse (scd->sven);
		
		g_free(filename);
	}
}

