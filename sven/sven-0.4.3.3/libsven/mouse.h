/* Sven multimedia keyboard deamon
 * mouse.h
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
#ifndef __MOUSE_H_
#define __MOUSE_H_

void init_mouse (Sven *sven);
void grab_mouse (MScreen *mscreen, int key_code,int state);
void grab_mouse_real (int key_code,int state,GdkWindow *root);
void update_mouse (Sven *sven);
void ungrab_mouse (MScreen *mscreen, int key_code);
void ungrab_mouse_real (int key_code, GdkWindow *root);

int ModifierToStringMouse (unsigned int modifier, char * str);
int StringToModifier_Mouse (char * str,unsigned int * modifier);
char* GetNameMouseButton (unsigned int button);

#endif /* __MOUSE_H_ */
