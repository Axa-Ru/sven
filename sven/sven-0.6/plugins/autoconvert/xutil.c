/* Sven multimedia keyboard deamon
 * sxkb.c
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
 
 #include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xlibint.h>
#include <X11/Xatom.h>
 #include <X11/keysym.h>
#include <X11/X.h>
#include <gdk/gdkx.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <locale.h>

#include "keyboard.h"
#include "xutil.h"
#include "../../libsven/emulate_input.h"

#define BASE(w,b)	(w & b)

void GetAppWindow(Display *dpy,Window win,Window *core)
{
	Window rwin, parent, *childrens, *child;
	unsigned int n;
	if (!XQueryTree(dpy, win, &rwin, &parent, &childrens, &n)) return;
		child = childrens;
	while(n) 
	{
		if (BASE(*child, ~(dpy->resource_mask)) != BASE(win, ~(dpy->resource_mask))) 
		{
			*core = *child;
			break;
		}
		GetAppWindow(dpy,*child, core);
		if (*core) break;
		child++; n--;
	}
	if (childrens) XFree(childrens);
}
/****************** Get class name for window *******************************/

Window xObject_findChildByAtom (Display * xDisplay, Window xWindow, Atom atom)
{
	Atom type = None;
	int format;
	unsigned long nitems, after;
	unsigned char *data = NULL;
	Window tmp_window = 0, root_window, parent_window;
	Window *children_window_list;
	unsigned int nchildren;
	unsigned int i;

	if (xWindow <= 0)
		return ERROR;

	if (!XQueryTree (xDisplay, xWindow, &root_window, &parent_window,
			 &children_window_list, &nchildren))
		return 0;

	for (i = 0; !tmp_window && (i < nchildren); i++) {
		data = NULL;

		XGetWindowProperty (xDisplay, children_window_list[i], atom,
				    0, 0, False, AnyPropertyType, &type,
				    &format, &nitems, &after, &data);
		if (data)
			XFree (data);

		if (type)
			tmp_window = children_window_list[i];
	}

	for (i = 0; !tmp_window && (i < nchildren); i++)
		tmp_window = xObject_findChildByAtom (xDisplay, children_window_list[i], atom);

	if (children_window_list)
		XFree (children_window_list);

	return tmp_window;
}

Window xObject_getAppRootWindow (Display *disp,Window wind)
{
	Window backup_window = 0, root_window, parent_window, window = wind;
	Window *children_window_list;
	unsigned int nchildren;
	gboolean exit_flag = FALSE;
	if (window == ERROR)
		return ERROR;
	while (!exit_flag) {
		if (!XQueryTree (disp, window, &root_window,
		     &parent_window, &children_window_list, &nchildren))
			return 0;
		if (children_window_list)
			XFree (children_window_list);
		if (parent_window == root_window)
			exit_flag = TRUE;
		else {
			backup_window = window;
			window = parent_window;
		}
	}

	return backup_window;
}


Window xObject_findWindowByAtomName (Display *disp,Window wind, char *atom_name)
{
	Atom atom;
	Atom type = None;
	int format;
	unsigned long nitems, after;
	unsigned char *data = NULL;
	Window app_root_window;
	Window tmp_window = 0, win;

	win = wind;
	if (win == ERROR)
		return ERROR;

	if (win ==
	    RootWindow (disp, DefaultScreen (disp)))
		return win;

	atom = XInternAtom (disp, atom_name, True);
	if (!atom)
		return wind;

	app_root_window = xObject_getAppRootWindow (disp,wind);
	if (app_root_window == ERROR)
		return ERROR;
	else if (app_root_window == 0)
		return 0;

	XGetWindowProperty (disp, app_root_window,
			    atom, 0, 0, False, AnyPropertyType,
			    &type, &format, &nitems, &after, &data);

	if (data)
		XFree (data);

	if (type)
		return app_root_window;

	tmp_window = xObject_findChildByAtom (disp, app_root_window, atom);

	if (tmp_window == ERROR)
		return ERROR;
	else if (!tmp_window)
		tmp_window = wind;

	return tmp_window;
}


XClassHint *xObject_getWMClass (Display *disp,Window wind)
{
	int dummyI;
	unsigned int dummyU;
	XClassHint *wm_class = NULL;
	Window root_window, window;

	window = xObject_findWindowByAtomName (disp,wind, "WM_CLASS");
	if (window == ERROR)
		return NULL;
	if (!window)
		window = wind;

	wm_class = XAllocClassHint ();
	if (!XGetClassHint (disp, window, wm_class)) {
		printf ("Can't get WM_CLASS\n");
		XFree (wm_class->res_class);
		XFree (wm_class->res_name);
		XFree (wm_class);
		return NULL;
	}
	return wm_class;
}

//NOTE: malloc! Don't forget to free after use
char *xObject_getWMClass_name (Display *disp,Window wind)
{
	char *string = NULL;
	XClassHint *wm_class;

	wm_class = xObject_getWMClass (disp,wind);
	if (!wm_class)
		return NULL;

	string = malloc (strlen (wm_class->res_name) + 1);

	if (string) {
		string = memset (string, NULLSYM, strlen (wm_class->res_name) + 1);
		string = memcpy (string, wm_class->res_name, strlen (wm_class->res_name));
	}

	XFree (wm_class->res_class);
	XFree (wm_class->res_name);
	XFree (wm_class);

	return string;
}

//NOTE: malloc! Don't forget to free after use
char *xObject_getWMClass_class (Display *disp,Window wind)
{
	char *string = NULL;
	XClassHint *wm_class;

	wm_class = xObject_getWMClass (disp,wind);
	if (!wm_class)
		return NULL;

	string = malloc (strlen (wm_class->res_class) + 1);

	if (string) {
		string = memset (string, NULLSYM, strlen (wm_class->res_class) + 1);
		string = memcpy (string, wm_class->res_class, strlen (wm_class->res_class));
	}

	XFree (wm_class->res_class);
	XFree (wm_class->res_name);
	XFree (wm_class);

	return string;
}



KeySym get_current_keysym_all (XEvent * event)
{
	KeySym ks;
	int nbytes;
	char *str = (char *) malloc (MAX_WORD_LENGTH);
	nbytes = XLookupString (&(event->xkey), str, MAX_WORD_LENGTH, &ks, NULL);
	free (str);

	return ks;
}
KeySym get_current_keysym (XEvent * event)
{
	return XKeycodeToKeysym (GDK_DISPLAY(),event->xkey.keycode,get_current_modifier(event));
}

int get_current_modifier (XEvent * event)
{
	int ks;

	if ((event->xkey.state) & ControlMask)
		ks = ControlMask;//ControlMask;//XK_Control_R;
	else if ((event->xkey.state) & ShiftMask)
		ks = ShiftMask;//ShiftMask;// XK_Shift_R;
	else
		ks = 0;
	return ks;
}

int get_key_action (XEvent * xev)
{
	int action;
	KeySym keysym = get_current_keysym (xev);

	//printf("keysym:\"%c\"\n",keysym);
	//Cursor keys
	if (IsCursorKey (keysym))
		switch (keysym) {
			case XK_Right:
				action = KLB_MOVE_RIGHT;
				break;
			case XK_Left:
				action = KLB_MOVE_LEFT;
				break;
			default:
				action = KLB_CLEAR;
				break;
		}
	else
		//KeyPad keys
	if (IsKeypadKey (keysym))
		switch (keysym) {
			case XK_KP_Right:
				action = KLB_MOVE_RIGHT;
				break;
			case XK_KP_Left:
				action = KLB_MOVE_LEFT;
				break;
			case XK_KP_Enter:
				action = KLB_ENTER;
				break;
			case XK_KP_Space:
				action = KLB_FLUSH;
				break;
			case XK_KP_Delete:
				action = KLB_DEL_SYM_RIGHT;
				break;
			case XK_KP_Insert:
				action = KLB_SWITCH_MODE;
				break;
			default:
				action = KLB_CLEAR;
				break;
		}
	else
		//Func, Mod, PF, PrivateKeypad keys
	if (IsFunctionKey (keysym) || IsModifierKey (keysym) ||
		    IsPFKey (keysym) || IsPrivateKeypadKey (keysym))
		action = KLB_NO_ACTION;
	else
		//MiscFunc keys
	if (IsMiscFunctionKey (keysym))
		if (keysym == XK_Insert) {
			action = KLB_SWITCH_MODE;
		}
		else
			action = KLB_NO_ACTION;
	else
		//del, bkspace, tab, space, return, alpha & num keys
		switch (keysym) {
			case XK_Delete:
				action = KLB_DEL_SYM_RIGHT;
				break;
			case XK_BackSpace:
				action = KLB_DEL_SYM_LEFT;
				break;
			case XK_Tab:
				action = KLB_CLEAR;
				break;
			case XK_space:
				action = KLB_FLUSH;
				break;
			case XK_Return:
				action = KLB_ENTER;
				break;
			case XK_Pause:
				action = KLB_NO_ACTION;
				break;
			case XK_Escape:
				action = KLB_NO_ACTION;
				break;
			case XK_Sys_Req:
				action = KLB_NO_ACTION;
				break;
			case XK_exclam:
				action = KLB_FLUSH;
				break;
			case XK_at:
				action = KLB_FLUSH;
				break;
			case XK_numbersign:
				action = KLB_FLUSH;
				break;
			case XK_ssharp:
				action = KLB_FLUSH;
				break;
			case XK_dollar:
				action = KLB_FLUSH;
				break;
			case XK_percent:
				action = KLB_FLUSH;
				break;
			case XK_asciicircum:
				action = KLB_FLUSH;
				break;
			case XK_ampersand:
				action = KLB_FLUSH;
				break;
			case XK_asterisk:
				action = KLB_FLUSH;
				break;
			case XK_parenleft:
				action = KLB_FLUSH;
				break;
			case XK_parenright:
				action = KLB_FLUSH;
				break;
			case XK_hyphen:
				action = KLB_FLUSH;
				break;
			case XK_underscore:
				action = KLB_FLUSH;
				break;
			case XK_equal:
				action = KLB_FLUSH;
				break;
			case XK_plus:
				action = KLB_FLUSH;
				break;
			case XK_minus:
				action = KLB_FLUSH;
				break;
			case XK_slash:
				action = KLB_FLUSH;
				break;
			case XK_bar:
				action = KLB_FLUSH;
				break;
			case XK_backslash:
				action = KLB_FLUSH;
				break;
			case XK_question:
				action = KLB_FLUSH;
				break;
			default:
				action = KLB_ADD_SYM;
				break;
		}

	return action;
}

Window get_active_window (Display *disp)
{
  Atom type;
  int format;
  unsigned long nitems;
  unsigned long bytes_after;
  unsigned char *data;
  int result;
  Window win=None;
  gint err;
  Atom net_active_window;
    
  type = None;
  
   net_active_window= XInternAtom(disp, "_NET_ACTIVE_WINDOW", False);
  
  gdk_error_trap_push();
  result = XGetWindowProperty (disp, DefaultRootWindow(disp),
         net_active_window,
         0,  0x7fffffff, False,XA_WINDOW, &type, &format, &nitems,
         &bytes_after, &data); 
  
  err=gdk_error_trap_pop();
   
  if (err!=0 || result != Success)
     return None;
        
  if (type != XA_WINDOW) {
      XFree (data);
      return None;
  }
  
  if (data) {
    win= *((Window *) data);
    XFree (data);
  }
  
  return win;
}

void delete_char_in_win(Window window,int N)
{
	int ind;

	for(ind = 0; ind < N; ind++)
	{
		send_key_to_win(window,XKeysymToKeycode(GDK_DISPLAY (),XStringToKeysym ("Delete")),0);
	}
} 

void backspace_char_in_win(Window window,int N)
{
	int ind;

	for(ind = 0; ind < N; ind++)
	{
		send_key_to_win(window,XKeysymToKeycode(GDK_DISPLAY (),XStringToKeysym ("BackSpace")),0);
	}
}

int send_key_to_win (Window wind, KeyCode keycode, int keycode_modifiers)
{
	int err = 0;
	XKeyEvent kevent;

	if (!wind)
	{
		printf("Not window\n");
		return -1;
	}

	kevent.display = GDK_DISPLAY ();
	kevent.window = wind;
	kevent.root = gdk_x11_get_default_root_xwindow();////DefaultRootWindow(GDK_DISPLAY ());
	kevent.subwindow = None;
	kevent.time = CurrentTime;
	kevent.x = 1;
	kevent.y = 1;
	kevent.x_root = 1;
	kevent.y_root = 1;
	kevent.same_screen = TRUE;
	kevent.type = KeyPress;
	kevent.keycode = keycode;
	kevent.state = keycode_modifiers;  

	err=XSendEvent(GDK_DISPLAY (),wind, TRUE, KeyPressMask, (XEvent *)&kevent);

	XSync(GDK_DISPLAY (), False);
	
	return err;
}
