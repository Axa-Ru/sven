/* Sven multimedia keyboard deamon
 * confswitch.h
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: more@irpin.com
 * web: http://sven.linux.kiev.ua/
 * icq:325697438
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef __SCONFSWITCH_H__
#define __SCONFSWITCH_H__


#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <X11/X.h>
#include <gdk/gdkx.h>
#include "sven.h"



G_BEGIN_DECLS

#define SCONFSWITCH_TYPE            (sconfswitch_get_type ())
#define SCONFSWITCH(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SCONFSWITCH_TYPE, Sconfswitch))
#define SCONFSWITCH_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SCONFSWITCH_TYPE, SconfswitchClass))
#define IS_SCONFSWITCH(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCONFSWITCH_TYPE))
#define IS_SCONFSWITCH_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SCONFSWITCH_TYPE))


typedef struct _Sconfswitch       Sconfswitch;
typedef struct _SconfswitchClass  SconfswitchClass;

struct _Sconfswitch
{
GtkVBox box;

Sven *sven;
GtkWidget *undo_key;
GtkWidget *focus_key;
GtkWidget *case_key;
GtkWidget *check_switch;
int auto_switch;
};

struct _SconfswitchClass
{
 GtkVBoxClass parent_class;

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


GType      sconfswitch_get_type (void) G_GNUC_CONST;
GtkWidget* sconfswitch_new (Sven *sven,gchar *name);
void sconfswitch_save (Sconfswitch *scsw);

G_END_DECLS

#endif /* __SCONFSWITCH_H__ */

