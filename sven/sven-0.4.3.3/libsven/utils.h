/* Sven multimedia keyboard deamon
 * utils.h
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
#ifndef __UTILS_H_
#define __UTILS_H_

typedef struct {
	int pid;
	char *name;
	long uid;
	float mem;
	char *short_cmd;
} proc_info;

proc_info * proc_info_scan(void);

void file_check_directory(void);

void show_error (const char *text);
void show_warning (const char *text);
void show_info (const char *text);
gboolean confirm (const char *text, const char *stock, const char *action);

void printerror (void);
void sven_exit (GtkMenuItem *menuitem,Sven *sven);
void execute (char *cmd, gboolean sync);

MXosd *greate_osd (MXosd  *osd,int enable);
void apply_xosd_config (MXosd  *osd);

void sven_read_config(Sven *conf);
void sven_write_config (Sven *conf);

GdkPixbuf *create_pixbuf (gint type, int width, int height);
GtkWidget *new_pixmap_mod(gint type, int width, int height);
GdkPixbuf *new_pixbuf_flag(gchar *langs, int width, int height);
GtkWidget *new_pixmap_flag(gchar *lang, int width, int height);
GtkWidget* create_menu_icon (Sven *sven);

void init_tray (Sven *sven);
void init_icon (Sven *sven);
void restart_tray (Sven *sven);
void restart_icon (Sven *sven);

gboolean tray_icon_press (GtkWidget *widget, GdkEventButton *event, Sven *sven);
gboolean icon_press_all (GtkWidget *widget, GdkEventButton *event, Sven *sven);

void load_startup_prog(Sven *sven);

void clipman_new(Sven *sven);
#endif /* __UTILS_H_ */

