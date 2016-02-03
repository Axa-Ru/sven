/* Sven multimedia keyboard deamon
 * glist.h
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
#ifndef __GLIST_H_
#define __GLIST_H_

void add_keysym_to_list(XEvent * event,gpointer data);
void add_keysym_to_list_num(XEvent * event,gpointer data,int num);
void add_keysym_to_list_num_insert(XEvent * event,gpointer data,int num);
void dell_keysym_to_list(gpointer data,int num);
void free_keysym_to_list(gpointer data);


#endif /* __GLIST_H_ */
