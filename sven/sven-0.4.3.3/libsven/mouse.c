/* Sven multimedia keyboard deamon
 * mouse.c
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
#include "utils.h"
#include "prefswin.h"
#include "plugin.h"
#include "key.h"
#include "mouse.h"


static char * modifier_string [] = {"Control", "Shift", "Alt","Win"};
static char * modifier_string_mouse [] = {"Mouse Left", "Mouse Center","Mouse Right","Scrool Up","Scrool Down","Scrool Left","Scrool Right","Button Any"};


char* GetNameMouseButton (unsigned int button)
{
	if(button>=7 || button==-1 || button==0)
		return g_strdup_printf("%d",button);
	else
	return modifier_string_mouse[button-1];
}

int ModifierToStringMouse (unsigned int modifier, char * str)
{
	str[0] = '\0';
	int n=0;

/*printf("ShiftMask:%d\n",ShiftMask);
printf("LockMask:%d\n",LockMask);
printf("ControlMask:%d\n",ControlMask);
printf("Mod1Mask:%d\n",Mod1Mask);
printf("Mod2Mask:%d\n",Mod2Mask);
printf("Mod3Mask:%d\n",Mod3Mask);
printf("Mod4Mask:%d\n",Mod4Mask);
printf("Mod5Mask:%d\n",Mod5Mask);
printf("Button1Mask:%d\n",Button1Mask);
printf("Button2Mask:%d\n",Button2Mask);
printf("Button3Mask:%d\n",Button3Mask);
printf("Button4Mask:%d\n",Button4Mask);
printf("Button5Mask:%d\n",Button5Mask);
printf("AnyModifier:%d\n",AnyModifier);*/

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
	if (modifier & Button1Mask)
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string_mouse[0]);
		n++;
	}
	if (modifier & Button2Mask)
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string_mouse[1]);
		n++;
	}
	if (modifier & Button3Mask)
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string_mouse[2]);
		n++;
	}
	if (modifier & Button4Mask)
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string_mouse[3]);
		n++;
	}
	if (modifier & Button5Mask)
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string_mouse[4]);
		n++;
	}
	if (modifier & AnyModifier)
	{
		if (str[0])
		strcat (str, "+");
		strcat (str, modifier_string_mouse[7]);
		n++;
	}
     return n;
}

int StringToModifier_Mouse (char * str,unsigned int * modifier)
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
		else if (strcasecmp (temp, "mouse left") == 0)
		{
		  *modifier += Button1Mask;
		  key=1;
		}
		else if (strcasecmp (temp, "mouse center") == 0)
		{
		  *modifier += Button2Mask;
		  key=2;
		}
		else if (strcasecmp (temp, "mouse right") == 0)
		{
		  *modifier += Button3Mask;
		  key=3;
		}
		else if (strcasecmp (temp, "scrool up") == 0)
		{
		  *modifier += Button4Mask;
		  key=4;
		}
		else if (strcasecmp (temp, "scrool down") == 0)
		{
		  *modifier += Button5Mask;
		  key=5;
		}
		else if (strcasecmp (temp, "button any") == 0)
		  *modifier += AnyModifier;
		else
		 {
		    key=atoi(temp);
		 }

		temp=strtok(NULL,"+");
	}
	g_free(temp);
	
	return key;
}

void ungrab_mouse_real (int key_code, GdkWindow *root)
{
	gdk_error_trap_push ();
	XUngrabButton (GDK_DISPLAY (), key_code, AnyModifier,(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())));
	XUngrabButton (GDK_DISPLAY (), AnyButton, AnyModifier, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())));


	gdk_flush ();
	if (gdk_error_trap_pop ()) {
		char *error;

		error = g_strdup_printf
			(_("There was an error removing access to the "
			   "mouse buttons.\n Buttons %d couldn't be unbound."),
			 key_code);
		show_error (error);
		printf(RED"EROR!:%s",error);
		printf(NORMAL"\n");
		g_free (error);
		exit (1);
	}
}

void ungrab_mouse (MScreen *mscreen, int key_code)
{
	GList *l;

	for (l = mscreen->screens; l != NULL; l = l->next)
	{
		GdkScreen *screen;

		screen = (GdkScreen *) l->data;
		ungrab_mouse_real (key_code, gdk_screen_get_root_window (screen));
	}
}

void update_mouse (Sven *sven)
{
	ConfigSection *section;
	GList *list;

	g_return_if_fail( sven != NULL);

	list = sven->cfgfilemouse->sections;
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
		ungrab_mouse(sven->screen, atoi(key_stat[0]));
	}
}


void grab_mouse_real (int key_code,int state,GdkWindow *root)
{
	gdk_error_trap_push ();
	DEBUG_MSG("grab_button_real:%d - state:%d\n",key_code,state);

	/*XGrabButton(GDK_DISPLAY (),3,256,GDK_WINDOW_XID (root),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, None, None);*/
	  /* XGrabButton (gdk_x11_get_default_xdisplay (),4,256,
		 (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (gdk_x11_get_default_xdisplay())),
		 False, ButtonPressMask | ButtonReleaseMask,
		 GrabModeAsync, GrabModeAsync, None, None);*/
	/*XGrabButton(GDK_DISPLAY (),4,AnyModifier,GDK_WINDOW_XID (root),False,ButtonPressMask,GrabModeAsync, GrabModeAsync, GDK_WINDOW_XID (root), None);*/
  
	if(state>0)
	{
		XGrabButton(GDK_DISPLAY (),key_code,state, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
		XGrabButton(GDK_DISPLAY (),key_code,DellModifier(state),(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
		XGrabButton(GDK_DISPLAY (),key_code,AddCapsModifier(state),(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
	
		XGrabButton(GDK_DISPLAY (),key_code,AddNumModifier(state),(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
		XGrabButton(GDK_DISPLAY (),key_code,AddCapsNumModifier(state),(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
	}
	else
	{
		XGrabButton(GDK_DISPLAY (),key_code,AnyModifier,(GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())),TRUE,ButtonPressMask | ButtonReleaseMask,GrabModeAsync, GrabModeAsync, (GDK_WINDOW_XID (root) ? GDK_WINDOW_XID (root) : DefaultRootWindow (GDK_DISPLAY())), None);
		
	}

	gdk_flush ();
	if (gdk_error_trap_pop ())
	{
		char *error;
		char str[1000];
		char *but="";

		ModifierToStringMouse (state, str);
		but=g_strdup_printf ("%s%s%d",str,str[0] ? "+" : "", key_code);
		
		error = g_strdup_printf
			(_("It seems that another application already has"
			   " access to the you mouse.\n"
			   "Buttons combination %s couldn't be bound.\n"
			   "Is another daemon already running ?\n"),
			 but);
		show_error(error);
		printf(RED"EROR!:%s",error);
		printf(NORMAL"\n");
		g_free (error);
	}
}

void grab_mouse (MScreen *mscreen, int but_code,int state)
{
	GList *l;

	for (l = mscreen->screens; l != NULL; l = l->next)
	{
		GdkScreen *screen;

		screen = (GdkScreen *) l->data;
		grab_mouse_real (but_code,state, gdk_screen_get_root_window (screen));
	}
}

void init_mouse (Sven *sven)
{
	ConfigSection *section;
	gchar *filename;
	GList *list;


	filename= g_strconcat(g_get_home_dir(),"/.sven/mouse_config", NULL);
	
	if (!g_file_test(filename, G_FILE_TEST_EXISTS)) 
	{
		g_free(filename);
		filename= g_strconcat(PKGDATADIR,"mouse_config", NULL);
	}
	
	sven->cfgfilemouse = sven_cfg_open_file(filename);
	DEBUG_MSG("Start read sven->cfgfilemouse mouse_config\n");
	DEBUG_MSG("init_mouse:%s\n",filename);
	if (!sven->cfgfilemouse)
			sven->cfgfilemouse = sven_cfg_new();
	
	if(sven->cfgfilemouse!=NULL)
	{
		list = sven->cfgfilemouse->sections;
		while (list)
		{
			int enable=1;
			int stat=0;
	
			section = (ConfigSection *) list->data;
			gchar **key_stat = g_strsplit(section->name,":",2);
			if(key_stat[1]==NULL)
				stat=0;
			else
				stat=atoi(key_stat[1]);
			DEBUG_MSG("button_code:%s - mod:%s\n",key_stat[0],stat);
			sven_cfg_read_int(sven->cfgfilemouse,section->name, "enable",&enable);

			if(enable)
				grab_mouse (sven->screen, atoi(key_stat[0]),stat);
			
			list = g_list_next(list);
		}
	}
	else
	{
		char *error;
		error = g_strdup_printf
		(_("Sorry this file: %s not found.\nPlease greate new configuration mouse.\n"),
		 filename);
		show_error (error);
		g_free (error);
	}
	
	g_free(filename);
	
	return;
}
