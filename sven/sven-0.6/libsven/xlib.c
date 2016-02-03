/* Sven multimedia keyboard deamon
 * xlib.c
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
#include "xlib.h"
#include "utils.h"
#include "grabkeyboard.h"

/**************************************************************************************/
static gchar *get_property (Display *disp, Window win, /*{{{*/
        Atom xa_prop_type, gchar *prop_name, unsigned long *size) 
{
    Atom xa_prop_name;
    Atom xa_ret_type;
    int ret_format;
    unsigned long ret_nitems;
    unsigned long ret_bytes_after;
    unsigned long tmp_size;
    unsigned char *ret_prop;
    gchar *ret;
    
    xa_prop_name = XInternAtom(disp, prop_name, False);
    
    /* MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
     *
     * long_length = Specifies the length in 32-bit multiples of the
     *               data to be retrieved.
     */
    if (XGetWindowProperty(disp, win, xa_prop_name, 0, MAX_PROPERTY_VALUE_LEN / 4, False,
            xa_prop_type, &xa_ret_type, &ret_format,     
            &ret_nitems, &ret_bytes_after, &ret_prop) != Success) {
        printf("Cannot get %s property.\n", prop_name);
        return NULL;
    }
  
    if (xa_ret_type != xa_prop_type) {
        printf("Invalid type of %s property.\n", prop_name);
        XFree(ret_prop);
        return NULL;
    }

    /* null terminate the result to make string handling easier */
    tmp_size = (ret_format / 8) * ret_nitems;
    ret = g_malloc(tmp_size + 1);
    memcpy(ret, ret_prop, tmp_size);
    ret[tmp_size] = '\0';

    if (size) {
        *size = tmp_size;
    }
    
    XFree(ret_prop);
    return ret;
}/*}}}*/


/* Find child work sven, and kill if found*/
static void selection_get_func (GtkClipboard *clipboard, GtkSelectionData *selection_data,
		guint info, gpointer user_data_or_owner)
{
}

static void selection_clear_func (GtkClipboard *clipboard, gpointer user_data_or_owner)
{
	return;
}

gboolean sven_get_lock (gpointer data)
{
	Sven *sven = (Sven *)data;
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
	{
		g_free(clipboard);
		goto out;
	}

	result = TRUE;

out:
	XUngrabServer (GDK_DISPLAY());
	gdk_flush();
	
	return result;
}

int sven_kill_lock (gpointer data)
{
	Sven *sven = (Sven *)data;
	unsigned long *pid;
	Window wind;
	Atom clipboard_atom = gdk_x11_get_xatom_by_name (SELECTION_NAME);

	XGrabServer (GDK_DISPLAY());

	wind=XGetSelectionOwner (GDK_DISPLAY(), clipboard_atom);
	
	if (wind != None)
	{
		/* pid */
        	pid =  (unsigned long *)get_property(GDK_DISPLAY(),wind,XA_CARDINAL, "_NET_WM_PID", NULL);
	}

	if(pid>0)
	{
		gchar *output = NULL;
		gchar *temp = NULL;
	
		temp=g_strdup_printf("kill %-6lu", pid ? *pid : 0);
		g_spawn_command_line_sync(temp,NULL,&output,NULL,NULL);
		if(g_strcasecmp(output,"")!=0)
			printf("Not Kill Sven children\n");
		else
			 printf("Kill Sven Pid: %-6lu \n", pid ? *pid : 0);
		g_free(temp);temp=NULL;
		g_free(output);output=NULL;
	}
	 
	XUngrabServer (GDK_DISPLAY());
	gdk_flush();
	
	XFree(pid);
	
	sven_get_lock(sven);
	return 1;
}
/*************************************************************************/
/* modifer */

static char * modifier_string [] = {"Control", "Shift", "Alt","Win"};
static char * modifier_string_mouse [] = {"Mouse Left", "Mouse Center","Mouse Right","Scrool Up","Scrool Down","Scrool Left","Scrool Right"};

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
	temp=NULL;
	
	return key;
}

char* GetNameMouseButton (unsigned int button)
{
	if(button>=6 || button==-1 || button==0)
		return g_strdup_printf("%d",button);
	else
	return modifier_string_mouse[button-1];
}

int ModifierToStringMouse (unsigned int modifier, char * str)
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
		else if (strcasecmp (temp, "scrool left") == 0)
		{
		  key=6;
		}
		else if (strcasecmp (temp, "scrool right") == 0)
		{
		  key=7;
		}
		else
		 {
		    key=atoi(temp);
		 }

		temp=strtok(NULL,"+");
	}
	g_free(temp);
	
	return key;
}

