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
#ifndef __XLIB_H_
#define __XLIB_H_

#define SELECTION_NAME "_SVEN_SELECTION"

#define MAX_PROPERTY_VALUE_LEN 4096

gboolean sven_get_lock (gpointer data);
int sven_kill_lock (gpointer data);

int DellModifier (unsigned int modifier);
int AddCapsModifier (unsigned int modifier);
int AddNumModifier (unsigned int modifier);
int AddCapsNumModifier (unsigned int modifier);

int ModifierToString (unsigned int modifier, char * str);
int StringToModifier_Key (char * str,unsigned int * modifier);

int ModifierToStringMouse (unsigned int modifier, char * str);
int StringToModifier_Mouse (char * str,unsigned int * modifier);
char* GetNameMouseButton (unsigned int button);

#endif /* __XLIB_H_ */
