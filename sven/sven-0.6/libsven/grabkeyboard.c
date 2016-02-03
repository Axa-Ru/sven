
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
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


#include "sven.h"
#include "key.h"
#include "utils.h"

#include "emulate_input.h"
#include "grabkeyboard.h"
#include "plugin.h"

#include "xlib.h"

int sven_cfg_find_function(Sven *sven,ConfigFile * cfg,int key,int mod);

GdkFilterReturn window_event_filter(GdkXEvent *xevent, GdkEvent *gdkevent, gpointer data)
{
	Sven *sven = (Sven *)data;
	XEvent *event= (XEvent *)xevent;

	XKeyEvent *key = (XKeyEvent *) xevent;

	switch (event->type) 
	{
		case KeyPress:
			if (event->xkey.send_event == TRUE)
				return GDK_FILTER_REMOVE;

					
			if(sven_cfg_find_function(sven,sven->config->key,key->keycode,key->state)==1)
				return GDK_FILTER_REMOVE;
	
			//printf ("KeyPress window: ID %#x \n",(unsigned int)event->xkey.window);
			return GDK_FILTER_REMOVE;
		break;
		case ButtonPress:
			if(sven_cfg_find_function(sven,sven->config->mouse,event->xbutton.button,
			event->xbutton.state)==1)
				return GDK_FILTER_REMOVE;
		break;
	}
    return GDK_FILTER_CONTINUE;
}

int sven_cfg_find_function(Sven *sven,ConfigFile * cfg,int key,int mod)
{
	ConfigSection *section;
	GList *list;

	list = cfg->sections;
	while (list)
	{
		int key_code=0;
		int key_mod=0;
		gchar **key_stat;
	
		section = (ConfigSection *) list->data;
		key_stat= g_strsplit(section->name,":",2);
		if(key_stat[1]==NULL)
			key_mod=0;
		else
			key_mod=atoi(key_stat[1]);
			key_code=atoi(key_stat[0]);
			
		g_strfreev(key_stat);

		if (key_code == key && (key_mod == mod || key_mod == DellModifier(mod) ||key_mod == AddCapsModifier(mod) || key_mod == AddCapsNumModifier(mod) || key_code == AddNumModifier(mod)))
		{
			char *type;
			char *command;
			char *comment;
			
			sven_cfg_read_string(cfg,section->name, "type",&type);
			sven_cfg_read_string(cfg,section->name, "command",&command);
			sven_cfg_read_string(cfg,section->name, "comment",&comment);
			do_action (sven,type,command,comment);
			free(type);type=NULL;
			free(command);command=NULL;
			free(comment);comment=NULL;
			return 1;
		}

		list = g_list_next(list);
	}
	return 0;
}

void do_action (gpointer data,char *type,char *command,char *title)
{
	Sven *sven = (Sven *)data;
	char *temp=NULL;
	
//	printf("do_action, type is: %s - %s\n", type,command);
	if((sven==NULL)||(type==NULL)||(command==NULL))
		return;

	if(g_strcasecmp (type,"def")==0)
	{
	}
	else if(g_strcasecmp (type,"sem")==0)
	{
		int key;
		unsigned int  mod;
		
		key=StringToModifier_Key(command,&mod);
		
		if((mod==8)&&(key==23))
			emulate_key_to_root_wind(key,mod,GDK_DISPLAY ());
		else
			emulate_key(key,mod,GDK_DISPLAY ());
	}
	else if(g_strcasecmp (type,"sem_mouse")==0)
	{
		int key;
		unsigned int  mod;
		
		key=StringToModifier_Mouse(command,&mod);
		
		emulate_button (key);
	}
	else if(g_strcasecmp (type,"cmd")==0)
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,title,-1);
		/*if(title!=NULL)
			gtk_osd_display (sven->osd,title);*/
		execute (command, TRUE);
	}
	else if(g_strncasecmp ((temp=g_strndup(&type[strlen(type)-strlen(".so")], strlen(".so"))),".so",3)==0)
	{
		SvenPluginPriv * plug=sven_plugin_find(sven,type);
		if((plug!=NULL)&&(plug->func!=NULL))
		{
			if ((plug->func->command!=NULL)&&(plug->enable==1))
			{
				plug->func->command(sven,plug->func->private_data,command,title);
			}
			/*else
			{
				printf("Bad %s modul\n",type);
			}*/
		}
		/*else
			printf("Bad %s == NULL modul\n",type);*/
	}
	else
	{
		gtk_osd_print (sven->osd,GTK_OSD_TEXT,"Bad command action",-1);
		printf("Bad command action\n");
	}
	free(temp);temp=NULL;
}
