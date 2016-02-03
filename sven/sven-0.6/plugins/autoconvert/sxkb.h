/* Sven multimedia keyboard deamon
 * sxkb.h
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
#ifndef __SXKB_H_
#define __SXKB_H_
#include <X11/XKBlib.h>

typedef struct _xkb_info {
	gchar *name;
	GdkPixbuf *flag_img;
} xkb_info;

typedef struct {
	/* X11 common stuff */
	int xkb_event_type;
	/* internal state mashine */
	int cur_group;
	int def_group;
	int ngroups;
	xkb_info* group2info[XkbNumKbdGroups];
	
	int view_flag;
	int view_flag_tray;
	GtkWidget *flag_win;
	GtkWidget *flag_image;
	gint         timeId;
	
	GtkWidget *flag_menu;
	GtkWidget *submenu;
} Sxkb;


void init_sxkb(gpointer sdata,gpointer data);
int sxkb_init_event(gpointer data);
void sxkb_destroy(void);

gpointer flag_menu_create(gpointer data);
void flag_menu_destroy(gpointer data);

#endif /* __SXKB_H_ */
