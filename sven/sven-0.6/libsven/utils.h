/* Sven multimedia keyboard deamon
 * utils.h
 *
 *Copyright (C) 2005 Eugene Morenko(More)
 *mailto: morenko@ukr.net
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

#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

void file_check_directory(char *dir);
void sven_read_config(Sven *sven);
void sven_write_config (Sven *sven);
void sven_exit (GtkMenuItem *menuitem,Sven *sven);

void show_error (const char *text);
void show_warning (const char *text);
void show_info (const char *text);
gboolean confirm (const char *text, const char *stock, const char *action);

void execute (char *cmd, gboolean sync);

GtkWidget *create_header(const char *text);

#ifdef __cplusplus
};
#endif

#endif /* __UTILS_H_ */

