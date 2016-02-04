/* Sven multimedia keyboard deamon
 * clipman.h
 *
 *Copyright (C) 2005 Eugene Morenko(More)
 *mailto: more@irpin.com
 *web: http://sven.linux.kiev.ua/
 *icq:325697438
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
#ifndef __CLIPMAN_H_
#define __CLIPMAN_H_

#include <assert.h>

#define MAXHISTORY 20
#define MENUTXT_LEN 30
#define CLIPMAN_TIMER_INTERVAL 500

typedef struct
{
	GtkWidget   *submenu;
	GtkWidget   *menu;
	GString     *content[MAXHISTORY];
	guint        iter;
	gint         timeId;
	gboolean     killing;
} t_clipman;

typedef struct
{
	t_clipman  *clip;
	guint       idx;
} t_action;

void clipman_press(t_clipman *clipman);
void clicked_cb(GtkWidget *button, gpointer data);
gchar* get_text_select ();
 

#endif /* __CLIPMAN_H_ */

