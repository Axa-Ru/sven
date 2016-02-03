/* Sven multimedia keyboard deamon
 * grabkeyboard.h
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
#ifndef __GRABKEYBOARD_H_
#define __GRABKEYBOARD_H_

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef __cplusplus
extern "C" {
#endif

GdkFilterReturn window_event_filter(GdkXEvent *xevent, GdkEvent *gdkevent, gpointer data);
void do_action (gpointer data,char *type,char *command,char *title);

#ifdef __cplusplus
};
#endif

#endif /* __GRABKEYBOARD_H_ */
