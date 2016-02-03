/* Sven multimedia keyboard deamon
 * pixmap.h
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
#ifndef __PIXMAP_H_
#define __PIXMAP_H_

GdkPixbuf *create_pixbuf_type (gint type, int width, int height);
GtkWidget *create_pixmap_type(gint type, int width, int height);

GdkPixbuf *create_pixbuf_from_file (gchar *file, gint width, gint height);
GtkWidget *create_pixmap_from_file(gchar *file,int width, int height);

#endif /* __PIXMAP_H_ */
