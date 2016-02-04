/* Sven multimedia keyboard deamon
 * xlib.h
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
#ifndef __XLIB_H_
#define __XLIB_H_

#define NULLSYM '\0'

char * KeyEventToString (XEvent * ev);
char xEvent_getCurrentChar (XEvent * ev);

int send_key_to_win (Window wind, KeyCode keycode, int keycode_modifiers);
int send_key_root_win(int key,int mod);
int send_key_focus_win(int key,int mod);
int delete_char_in_win(Window wind,int N);

KeySym get_current_keysym (XEvent * event);
int get_current_modifier (XEvent * event);
int get_key_action (XEvent * xev);

#endif /* __XLIB_H_ */
