/* Sven multimedia keyboard deamon
 * xlib.h
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
#ifndef __XUTIL_H_
#define __XUTIL_H_
/*general err*/
#define ERROR  -1
#define NULLSYM '\0'

#define MAX_PROPERTY_VALUE_LEN 4096
#define MAX_WORD_LENGTH 64

void GetAppWindow(Display *dpy,Window win,Window *core);
char *xObject_getWMClass_class (Display *disp,Window wind);
char *xObject_getWMClass_name (Display *disp,Window wind);

KeySym get_current_keysym_all (XEvent * event);
KeySym get_current_keysym (XEvent * event);
int get_current_modifier (XEvent * event);
int get_key_action (XEvent * xev);
Window get_active_window (Display *disp);
void delete_char_in_win(Window window,int N);
void backspace_char_in_win(Window window,int N);
int send_key_to_win (Window wind, KeyCode keycode, int keycode_modifiers);

#endif /* __XUTIL_H_ */
