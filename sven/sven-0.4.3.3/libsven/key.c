/* Sven multimedia keyboard deamon
 * key.c
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
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "sven.h"
#include "key.h"
#include "utils.h"
#include "plugin.h"
#include "mixer.h"
#include "cdpl.h"
#include "grabkeyboard.h"
#include "xlib.h"


static char * modifier_string [] = {"Control", "Shift", "Alt","Win"};

int DellModifier (unsigned int modifier)
{
	int mod = 0;

	if (modifier & ControlMask )
		mod += ControlMask;
  
	if (modifier & ShiftMask)
		mod += ShiftMask;

	if (modifier & Mod1Mask)
		mod += Mod1Mask;

	if (modifier & Mod4Mask)
		mod += Mod4Mask;
	
	return mod;
}
int AddCapsModifier (unsigned int modifier)
{
	int mod = DellModifier(modifier);

	mod += LockMask;
	return mod;
}
int AddNumModifier (unsigned int modifier)
{
	int mod = DellModifier(modifier);

	mod += Mod2Mask;
	return mod;
}
int AddCapsNumModifier (unsigned int modifier)
{
	int mod = DellModifier(modifier);

	mod += LockMask;
	mod += Mod2Mask;
	return mod;
}

int ModifierToString (unsigned int modifier, char * str)
{
	str[0] = '\0';
	int n=0;
	if (modifier & ControlMask )
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string[0]);
		n++;
	}
  
	if (modifier & ShiftMask)
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string[1]);
		n++;
	}

	if (modifier & Mod1Mask)
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string[2]);
		n++;
	}
	if (modifier & Mod4Mask)
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string[3]);
		n++;
	}
     return n;
}
int StringToModifier_Key (char * str,unsigned int * modifier)
{
	int key=0;
	gchar *temp;
 
	temp=strtok(str,"+");
	*modifier=0;
	
	while(temp!=NULL)
	{
		if (strcasecmp (temp, "control") == 0)
		  *modifier += ControlMask;
		else if (strcasecmp (temp, "shift") == 0)
		  *modifier += ShiftMask;
		else if (strcasecmp (temp, "mod1") == 0 || strcasecmp (temp, "alt") == 0)
		  *modifier += Mod1Mask;
		else if (strcasecmp (temp, "mod4") == 0 || strcasecmp (temp, "win") == 0)
		  *modifier += Mod4Mask;
		else
		{
			if(atoi(temp)!=0)
				 key=atoi(temp);
			else
			{
				key =XKeysymToKeycode(GDK_DISPLAY(),XStringToKeysym (temp));
				if (key == 0)
					break;
				temp=strtok(NULL,"+");
				    continue;
			}
		}
		temp=strtok(NULL,"+");
	}
	g_free(temp);
	
	return key;
}


static void selection_get_func (GtkClipboard *clipboard, GtkSelectionData *selection_data,
		guint info, gpointer user_data_or_owner)
{
}

static void selection_clear_func (GtkClipboard *clipboard, gpointer user_data_or_owner)
{
	return;
}

gboolean sven_get_lock (Sven *sven)
{
	gboolean result = FALSE;
	GtkClipboard *clipboard;
	Atom clipboard_atom = gdk_x11_get_xatom_by_name (SELECTION_NAME);
	static const GtkTargetEntry targets[] = 
	{
		{ SELECTION_NAME, 0, 0 }
	};

	XGrabServer (GDK_DISPLAY());

	if (XGetSelectionOwner (GDK_DISPLAY(), clipboard_atom) != None)
		goto out;

	clipboard = gtk_clipboard_get (gdk_atom_intern (SELECTION_NAME, FALSE));

	if (!gtk_clipboard_set_with_data  (clipboard, targets,
				G_N_ELEMENTS (targets),
				selection_get_func,
				selection_clear_func, NULL))
		goto out;

	result = TRUE;

out:
	XUngrabServer (GDK_DISPLAY());
	gdk_flush();

	return result;
}

void ungrab_key_real (int key_code, GdkWindow *root)
{
	gdk_error_trap_push ();
	XUngrabKey (GDK_DISPLAY (), key_code, AnyModifier,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())));
	XUngrabKey (GDK_DISPLAY (), AnyKey, AnyModifier, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())));


	gdk_flush ();
	if (gdk_error_trap_pop ()) {
		char *error;

		error = g_strdup_printf
			(_("There was an error removing access to the "
			   "multimedia keys.\nKey %d couldn't be unbound."),
			 key_code);
		show_error (error);
		printf(RED"EROR!:%s",error);
		printf(NORMAL"\n");
		g_free (error);
		exit (1);
	}
}

void ungrab_key (MScreen *mscreen, int key_code)
{
	GList *l;

	for (l = mscreen->screens; l != NULL; l = l->next)
	{
		GdkScreen *screen;

		screen = (GdkScreen *) l->data;
		ungrab_key_real (key_code, gdk_screen_get_root_window (screen));
	}
}

void update_kbd (Sven *sven)
{
	ConfigSection *section;
	GList *list;

	g_return_if_fail( sven != NULL);

	list = sven->cfgfile->sections;
	while (list)
	{
		int stat=0;
		
		section = (ConfigSection *) list->data;
		DEBUG_MSG("READ:%s\n",section->name);
		list = g_list_next(list);
		gchar **key_stat = g_strsplit(section->name,":",2);
		if(key_stat[1]==NULL)
				stat=0;
			else
				stat=atoi(key_stat[1]);
			DEBUG_MSG("key_code:%s - mod:%d\n",key_stat[0],stat);
		ungrab_key(sven->screen, atoi(key_stat[0]));
	}
}


void grab_key_real (int key_code,int state,GdkWindow *root)
{
	gdk_error_trap_push ();
	DEBUG_MSG("grab_key_real:%d - state:%d\n",key_code,state);

	if(state>0)
	{
		XGrabKey (GDK_DISPLAY (), key_code,
			state,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			DellModifier(state),
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			AddCapsModifier(state),
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			AddNumModifier(state),
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			AddCapsNumModifier(state),
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), False,
			GrabModeAsync, GrabModeAsync);
	}
	else
	{
		XGrabKey (GDK_DISPLAY (), key_code,
			0,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod2Mask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod5Mask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			LockMask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod2Mask | LockMask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod5Mask | LockMask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod2Mask | Mod5Mask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
		XGrabKey (GDK_DISPLAY (), key_code,
			Mod2Mask | Mod5Mask | LockMask,
			(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), True,
			GrabModeAsync, GrabModeAsync);
	}

	gdk_flush ();
	if (gdk_error_trap_pop ())
	{
		char *error;
		char str[1000];
		char *key="";

		ModifierToString (state, str);
				key=g_strdup_printf ("%s%s%s",str,str[0] ? "+" : "",
				(XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (), key_code, 0)) != NULL)
				? XKeysymToString (XKeycodeToKeysym (GDK_DISPLAY (),key_code, 0))
				: g_strdup_printf ("%d",key_code));
		
		error = g_strdup_printf
			(_("It seems that another application already has"
			   " access to the multimedia keys.\n"
			   "Key %s couldn't be bound.\n"
			   "Is another daemon already running ?\n"),
			 key);
		show_error(error);
		printf(RED"EROR!:%s",error);
		printf(NORMAL"\n");
		g_free (error);
	}
}

void grab_key (MScreen *mscreen, int key_code,int state)
{
	GList *l;

	
	
	for (l = mscreen->screens; l != NULL; l = l->next)
	{
		GdkScreen *screen;

		screen = (GdkScreen *) l->data;
		grab_key_real (key_code,state, gdk_screen_get_root_window (screen));
	}
}

void init_kbd (Sven *sven)
{
	int i;
	ConfigSection *section;
	gchar *filename;
	GList *list;
	
	sven->screen=g_malloc0(sizeof (MScreen));
	DEBUG_MSG("Start init_kbd\n");

	sven->screen->display = gdk_display_get_default ();
	sven->screen->screens = NULL;

	if (gdk_display_get_n_screens (sven->screen->display) == 1)
	{
		sven->screen->screens = g_list_append (sven->screen->screens,
				gdk_screen_get_default ());
	} else 
	{
		for (i = 0; i < gdk_display_get_n_screens (sven->screen->display); i++)
		{
			GdkScreen *screen;

			screen = gdk_display_get_screen (sven->screen->display, i);

			if (screen != NULL)
				sven->screen->screens = g_list_append (sven->screen->screens,screen);
		}
	}

	filename= g_strconcat(g_get_home_dir(),"/.sven/key_config", NULL);
	
	if (!g_file_test(filename, G_FILE_TEST_EXISTS)) 
	{
		g_free(filename);
		filename= g_strconcat(PKGDATADIR,"key_config", NULL);
	}
	DEBUG_MSG("init_kbd:%s\n",filename);
	sven->cfgfile = sven_cfg_open_file(filename);
	DEBUG_MSG("Start read sven->cfgfile\n");
	
	if (!sven->cfgfile)
			sven->cfgfile = sven_cfg_new();
	
	if(sven->cfgfile!=NULL)
	{
		list = sven->cfgfile->sections;
		while (list)
		{
			int enable=1;
			int stat=0;
	
			section = (ConfigSection *) list->data;
			gchar **key_stat = g_strsplit(section->name,":",2);
			DEBUG_MSG("SIZE:%d\n",(sizeof(key_stat) / sizeof(key_stat[0])));
			if(key_stat[1]==NULL)
				stat=0;
			else
				stat=atoi(key_stat[1]);
			DEBUG_MSG("key_code:%s - mod:%d\n",key_stat[0],stat);
			sven_cfg_read_int(sven->cfgfile,section->name, "enable",&enable);

			if(enable)
				grab_key (sven->screen, atoi(key_stat[0]),stat);
			
			list = g_list_next(list);
		}
	}
	else
	{
		char *error;
		error = g_strdup_printf
		(_("Sorry this file: %s not found.\nPlease greate new configuration keyboard.\n"),
		 filename);
		show_error (error);
		g_free (error);
	}
	
	g_free(filename);
	
	return;
}

void do_action (Sven *sven,char *type,char *command,char *title)
{
	SvenPlugin *splg;
	char *def="def";
	char *cmd="cmd";
	char *sem="sem";
	char *so=".so";
	
	DEBUG_MSG("do_action, type is: %s\n", type);
	
	if(g_strcasecmp (type,def)==0)
	{
		if(g_strcasecmp (command,"Clipboard Manager")==0)
		{
			clicked_cb(NULL,sven->clipman);
			gtk_menu_popup (GTK_MENU(sven->clipman->menu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
			return ;
		}
		if(g_strcasecmp (command,"ConvertLang")==0)
		{
		//	printf("Convert text\n");
			xConvert_text(sven);
			return ;
		}
		if(g_strcasecmp (command,"ConvertFocus")==0)
		{
		//	printf("Convert text\n");
			xConvert_text_focus(sven);
			return ;
		}
		if(g_strcasecmp (command,"ConvertCase")==0)
		{
		//	printf("Convert Case text\n");
			xConvert_text_case(sven);
			return ;
		}
	
		gchar **mixer = g_strsplit(command,"-",2);

		if(g_strcasecmp (mixer[0],"CD")==0)
		{
			select_cd(sven,mixer[1],title);
		}
		if(g_strcasecmp (mixer[0],"MIX")==0)
		{
			select_mixer(sven,mixer[1],title);
		}
	}
	else if(g_strcasecmp (type,sem)==0)
	{
		int key;
		int mod;
		
		key=StringToModifier_Key(command,&mod);
		
		if(mod==8)
			send_key_root_win(key,mod);
		else
			send_key_focus_win(key,mod);
			
		DEBUG_MSG("Keys:%d -- Mod:%d\n",key,mod);
	}
	else if(g_strcasecmp (type,cmd)==0)
	{
		xosd_display (sven->osd->osd,0, XOSD_string,title);
		xosd_display (sven->osd->osd,1, XOSD_string, "");
		execute (command, FALSE);
	}
	else if(sven->mix->enable_plugin==1)
	{
		if(g_strncasecmp (g_strndup(&type[strlen(type)-strlen(so)], strlen(so)),so,3)==0)
		{
			splg = g_slist_nth(sven->plugins,plugin_find_pos(sven,type))->data;
			if(splg!=NULL)
			{
				if (splg->create_plugin!=NULL)
					splg->create_plugin(sven,command);
				else
					show_error(g_strdup_printf(_("Bad %s modul"),splg->filename));
			}
			else
				show_error(g_strdup_printf(_("Bad %s modul"),type));
		}
	}
	else
	{
		printf("Bad config line\n");
	}
}


GdkFilterReturn sven_filter_events (GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
	Sven *sven = (Sven *) data;
	XEvent *xev = (XEvent *) xevent;
	XAnyEvent *xanyev = (XAnyEvent *) xevent;
	XKeyEvent *key = (XKeyEvent *) xevent;
	GdkScreen *event_screen = NULL;
	GList *l;
	ConfigSection *section;
	GList *list;
	DEBUG_MSG("sven_filter_events\n");


	
	/* Look for which screen we're receiving events */
	for (l = sven->screen->screens; (l != NULL) && (event_screen == NULL);
			l = l->next)
	{
		GdkWindow *window;
		GdkScreen *screen;

		screen = (GdkScreen *) l->data;
		window = gdk_screen_get_root_window (screen);

		if (GDK_WINDOW_XID (window) == xanyev->window)
		{
			event_screen = screen;
			break;
		}
	}

	key = (XKeyEvent *) xevent;
	/*printf("Key press !\n");
	printf("e.xkey.keycode=%d\n", xev->xkey.keycode);
	printf("e.xkey.state=%d\n", xev->xkey.state);*/

	DEBUG_MSG("Start\n");
	//printf("keyval=%d,\n hardware_keycode=%d\nstate=%d\nlength=%d\nstring=%s\n group=%d\n",event->key.keyval, event->key.hardware_keycode,event->key.state,event->key.length,event->key.string,event->key.group);
	DEBUG_MSG("________________________________________________________________________________________\n");
	DEBUG_MSG("type=%d, \nstate=%d\nkeycode=%d\n",key->type, key->state,key->keycode);
	DEBUG_MSG("End\n");
	
	if (xev->type == ButtonPress)
	{
		//printf("BUTTON PRess\n");
		list = sven->cfgfilemouse->sections;
		while (list)
		{
			char *type;
			char *description;
			char *command;
			int key_code=0;
			int key_mod=0;
	
			section = (ConfigSection *) list->data;
			gchar **key_stat = g_strsplit(section->name,":",2);
			if(key_stat[1]==NULL)
				key_mod=0;
			else
				key_mod=atoi(key_stat[1]);
			DEBUG_MSG("key_code:%s - mod:%d\n",key_stat[0],key_mod);
			key_code=atoi(key_stat[0]);
			DEBUG_MSG("READ:%s\n",section->name);
			
			sven_cfg_read_string(sven->cfgfilemouse,section->name, "type",&type);
			sven_cfg_read_string(sven->cfgfilemouse,section->name, "comment",&description);
			sven_cfg_read_string(sven->cfgfilemouse,section->name, "command",&command);
	
			DEBUG_MSG("description:%s\n",description);
			DEBUG_MSG("command:%s\n",command);
			DEBUG_MSG("key_code:%d\n",key_code);
			DEBUG_MSG("comparing %d and %d\n", key_code,key->keycode);
	
	
			if (key_code == xev->xbutton.button && (key_mod == xev->xbutton.state || key_mod == DellModifier(xev->xbutton.state) || key_mod == AddCapsModifier(xev->xbutton.state) || key_mod == AddCapsNumModifier(xev->xbutton.state) || key_code == AddNumModifier(xev->xbutton.state)))
			{
				/*if (xev->type != ButtonPress)
					return GDK_FILTER_CONTINUE;*/
				sven->screen->current_screen = event_screen;
	
				do_action (sven,type,command,description);
					return GDK_FILTER_REMOVE;
			}
			list = g_list_next(list);
		}
	}
	
	if (xev->type == KeyPress)
	{
		int key_code=0;
		int key_mod=0;
		
		key_code=StringToModifier_Key(g_strdup(sven->keyboard->conv_lang),&key_mod);
		
		if (key_code == key->keycode && (key_mod == key->state || key_mod == DellModifier(key->state) || 	 	key_mod == AddCapsModifier(key->state) || key_mod == AddCapsNumModifier(key->state) || key_code == AddNumModifier(key->state)))
		{ 
			sven->screen->current_screen = event_screen;
			
				do_action (sven,"def","ConvertLang","");
				return GDK_FILTER_REMOVE;
		}
		
		key_code=StringToModifier_Key(g_strdup(sven->keyboard->conv_case),&key_mod);
		if (key_code == key->keycode && (key_mod == key->state || key_mod == DellModifier(key->state) || 	 	key_mod == AddCapsModifier(key->state) || key_mod == AddCapsNumModifier(key->state) || key_code == AddNumModifier(key->state)))
		{
			sven->screen->current_screen = event_screen;
			
				do_action (sven,"def","ConvertCase","");
				return GDK_FILTER_REMOVE;
		}
		
		key_code=StringToModifier_Key(g_strdup(sven->keyboard->conv_focus),&key_mod);
		if (key_code == key->keycode && (key_mod == key->state || key_mod == DellModifier(key->state) || 	 	key_mod == AddCapsModifier(key->state) || key_mod == AddCapsNumModifier(key->state) || key_code == AddNumModifier(key->state)))
		{
			sven->screen->current_screen = event_screen;
			
				do_action (sven,"def","ConvertFocus","");
				return GDK_FILTER_REMOVE;
		}

		list = sven->cfgfile->sections;
		while (list)
		{
			char *type;
			char *description;
			char *command;
			int key_code=0;
			int key_mod=0;
	
			section = (ConfigSection *) list->data;
			gchar **key_stat = g_strsplit(section->name,":",2);
			if(key_stat[1]==NULL)
				key_mod=0;
			else
				key_mod=atoi(key_stat[1]);
			DEBUG_MSG("key_code:%s - mod:%d\n",key_stat[0],key_mod);
			
			key_code=atoi(key_stat[0]);
			DEBUG_MSG("READ:%s\n",section->name);
			
			sven_cfg_read_string(sven->cfgfile,section->name, "type",&type);
			sven_cfg_read_string(sven->cfgfile,section->name, "comment",&description);
			sven_cfg_read_string(sven->cfgfile,section->name, "command",&command);
	
			DEBUG_MSG("description:%s\n",description);
			DEBUG_MSG("command:%s\n",command);
			DEBUG_MSG("key_code:%d\n",key_code);
			DEBUG_MSG("comparing %d and %d\n", key_code,key->keycode);
	
	
			if (key_code == key->keycode && (key_mod == key->state || key_mod == DellModifier(key->state) || 	 	key_mod == AddCapsModifier(key->state) || key_mod == AddCapsNumModifier(key->state) || key_code == AddNumModifier(key->state)))
			{
				/*if (xev->type != KeyPress || KeyRelease)
					return GDK_FILTER_CONTINUE;*/
				sven->screen->current_screen = event_screen;
				
					do_action (sven,type,command,description);
					return GDK_FILTER_REMOVE;
			}
			list = g_list_next(list);
		}
	}
	
	return GDK_FILTER_CONTINUE;
}

