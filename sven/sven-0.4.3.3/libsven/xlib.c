/* Sven multimedia keyboard deamon
 * xlib.c
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
#include "xlib.h"
#include "utils.h"
 
 
#define KEY_BUFF_SIZE 256
static char key_buff[KEY_BUFF_SIZE];

char xEvent_KeysymToChar (KeySym keysym)
{
	int iterator;
	char res = NULLSYM;
	KeySym ks[] = { XK_quotedbl, XK_apostrophe, XK_bracketleft, XK_bracketright,
		XK_comma, XK_period, XK_colon, XK_semicolon,
		XK_less, XK_greater, XK_braceleft, XK_braceright,
		XK_space, XK_exclam, XK_at, XK_numbersign,
		XK_ssharp, XK_dollar, XK_percent, XK_degree,
		XK_ampersand, XK_asterisk, XK_parenleft, XK_parenright,
		XK_underscore, XK_equal, XK_plus, XK_minus,
		XK_slash, XK_bar, XK_backslash, XK_question
	};
	char ch[] = { '"', '\'', '{', '}',
		',', '.', ':', ';',
		'<', '>', '[', ']',
		' ', '!', '@', '#',	//TODO
		'#', '$', '%', '^',
		'&', '*', '(', ')',
		'_', '=', '+', '-',
		'/', '|', '\\', '?'
	};

	for (iterator = 0; iterator < 32; iterator++)
		if (keysym == ks[iterator]) {
			res = ch[iterator];
			break;
		}
	return res;
}

char xEvent_getCurrentChar (XEvent * ev)
{
	KeySym ks;
	int nbytes;
	char str[MAX_WORD_LENGTH];
	str[0] = NULLSYM;
	char *strPtr = NULL;

	nbytes = XLookupString ((XKeyEvent *) ev, str, MAX_WORD_LENGTH, &ks, NULL);
	if ((str[0] == NULLSYM)) 
	{

		str[0] = xEvent_KeysymToChar (ks);
		if (str[0] == NULLSYM) 
		{
			strPtr = XKeysymToString (ks);
			str[0] = strPtr[0];
		}
	}
	return str[0];
}

char * KeyEventToString (XEvent * ev)
{
	int count;
	char *tmp;
	KeySym ks;
	
	ks = 0;
	
	if (ev->type == KeyPress) 
	{
		count = XLookupString ((XKeyEvent *) ev, key_buff, KEY_BUFF_SIZE, &ks, NULL);
		key_buff[count] = '\0';
	
		if (count == 0) 
		{
			tmp = XKeysymToString (ks);
			if (tmp)
			   strcpy (key_buff, tmp);
			else
			    key_buff[0] = 0;
		}
		return key_buff;
	}
	else
	
    return NULL;
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


int send_key_focus_win(int key,int mod)
{
	XKeyEvent kevent;
	Window focuswin;
	int revert_to;

	XGetInputFocus(GDK_DISPLAY (), &focuswin, &revert_to);
	if (focuswin == 0) 
	{
		printf("No focused window!\n");
		return 0;
	}

	kevent.display = GDK_DISPLAY ();
	kevent.window = focuswin;
	kevent.root = gdk_x11_get_default_root_xwindow();////DefaultRootWindow(GDK_DISPLAY ());
	kevent.subwindow = None;
	kevent.time = CurrentTime;
	kevent.x = 1;
	kevent.y = 1;
	kevent.x_root = 1;
	kevent.y_root = 1;
	kevent.same_screen = TRUE;
	kevent.type = KeyPress;
	kevent.keycode = key;
	kevent.state = mod;  

	revert_to=XSendEvent(GDK_DISPLAY (),focuswin, TRUE, KeyPressMask, (XEvent *)&kevent);

	XSync(GDK_DISPLAY (), False);
	/* Wonder if we might ever need the key release --
	* but in some contexts, that actually gets interpreted
	* as another key press!
	XSendEvent(gdk_x11_get_default_xdisplay(), focuswin, TRUE, KeyReleaseMask, (XEvent *)&kevent);
	*/
	DEBUG_MSG("send_key_focus_win\n");
	return revert_to;
}
int send_key_root_win(int key,int mod)
{
	XKeyEvent kevent;
	int revert_to;
	kevent.display = GDK_DISPLAY ();
	kevent.send_event = TRUE;	/* true if this came from a SendEvent request */
	kevent.window =DefaultRootWindow(GDK_DISPLAY ());
	kevent.root = DefaultRootWindow(GDK_DISPLAY ()); //gdk_x11_get_default_root_xwindow();////DefaultRootWindow(GDK_DISPLAY ());
	kevent.subwindow = DefaultRootWindow(GDK_DISPLAY ());//None;
	kevent.time = CurrentTime;
	kevent.x = 1;
	kevent.y = 1;
	kevent.x_root = 1;
	kevent.y_root = 1;
	kevent.same_screen = TRUE;
	kevent.type = KeyPress;
	kevent.keycode = key;
	kevent.state = mod; 

	revert_to=XSendEvent(GDK_DISPLAY (),DefaultRootWindow(GDK_DISPLAY ()), TRUE, KeyPressMask, (XEvent *)&kevent);
	XSync(GDK_DISPLAY (), False);	
	/* Wonder if we might ever need the key release --
	* but in some contexts, that actually gets interpreted
	* as another key press!
	XSendEvent(gdk_x11_get_default_xdisplay(), DefaultRootWindow(gdk_x11_get_default_xdisplay()), TRUE, KeyReleaseMask, (XEvent *)&kevent);
	*/
	DEBUG_MSG("send_key_root_win\n");
	return revert_to;
}

int delete_char_in_win(Window wind,int N)
{
	int err = 0;
	int ind;

	for(ind = 0; ind < N; ind++)
	{
		err=send_key_to_win(wind,XKeysymToKeycode(GDK_DISPLAY (),XStringToKeysym ("BackSpace")),0);
	}

	//XFlush (dpy);
	return err;
} 

KeySym get_current_keysym (XEvent * event)
{
	KeySym ks;
	int nbytes;
	char *str = (char *) malloc (MAX_WORD_LENGTH);
	nbytes = XLookupString (&(event->xkey), str, MAX_WORD_LENGTH, &ks, NULL);
	free (str);

	return ks;
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

