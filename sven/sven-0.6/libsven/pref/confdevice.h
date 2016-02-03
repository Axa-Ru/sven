/* Sven multimedia keyboard deamon
 * confdevice.h
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: morenko@ukr.net
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
#ifndef __SCONFDEVICE_H__
#define __SCONFDEVICE_H__


#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <X11/X.h>
#include <gdk/gdkx.h>
#include "sven.h"



G_BEGIN_DECLS

#define SCONFDEVICE_TYPE            (sconfdevice_get_type ())
#define SCONFDEVICE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SCONFDEVICE_TYPE, Sconfdevice))
#define SCONFDEVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SCONFDEVICE_TYPE, SconfdeviceClass))
#define IS_SCONFDEVICE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCONFDEVICE_TYPE))
#define IS_SCONFDEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SCONFDEVICE_TYPE))


typedef struct _Sconfdevice       Sconfdevice;
typedef struct _SconfdeviceClass  SconfdeviceClass;

struct _Sconfdevice
{
GtkVBox box;

Sven *sven;
GtkWidget *list_tree_view;
GtkListStore *list_model;
GObject *list_selection;
int iter_count_new;

GtkWidget *prefs_new_wid;
GtkWidget *prefs_del_wid;
GtkWidget *prefs_edit_wid;

int device;

GtkWidget *configure_win_button_edit;
GtkWidget *configure_win_mouse;
GtkWidget *filechooser;
GtkWidget *mouse_code;
GtkWidget *desc_entry;
GtkWidget *key_entry;
GtkWidget *text_entry;
GtkWidget *runs_def_entry;
GtkWidget *runs_def_entry_type;
GtkWidget *runs_prog_entry;
GtkWidget *runs_spec_entry;
GtkWidget *runs_spec_entry_type;
GtkWidget *status_check;
GtkWidget *runs_radio;
GtkWidget *runs_radio1;
GtkWidget *runs_radio2;
};

struct _SconfdeviceClass
{
 GtkVBoxClass parent_class;

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


GType      sconfdevice_get_type (void) G_GNUC_CONST;
GtkWidget* sconfdevice_new (Sven *sven,gchar *name,int device);
void sconfdevice_save (Sconfdevice *scd);

G_END_DECLS

#endif /* __SCONFDEVICE_H__ */

