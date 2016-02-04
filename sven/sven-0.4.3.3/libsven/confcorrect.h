/* Sven multimedia keyboard deamon
 * confcorrect.h
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
#ifndef __SCONFCORRECT_H__
#define __SCONFCORRECT_H__


#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <X11/X.h>
#include <gdk/gdkx.h>
#include "sven.h"



G_BEGIN_DECLS

#define SCONFCORRECT_TYPE            (sconfcorrect_get_type ())
#define SCONFCORRECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SCONFCORRECT_TYPE, Sconfcorrect))
#define SCONFCORRECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SCONFCORRECT_TYPE, SconfcorrectClass))
#define IS_SCONFCORRECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCONFCORRECT_TYPE))
#define IS_SCONFCORRECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SCONFCORRECT_TYPE))


typedef struct _Sconfcorrect       Sconfcorrect;
typedef struct _SconfcorrectClass  SconfcorrectClass;

struct _Sconfcorrect
{
GtkVBox box;

Sven *sven;

GtkWidget *list_tree_view;
GtkListStore *list_model;
GObject *list_selection;

GtkWidget *prefs_new_wid;
GtkWidget *prefs_del_wid;
GtkWidget *prefs_edit_wid;

int device;
gchar *lang;

GtkWidget *lang_combo;
GtkWidget *check_enable_lang;
int corr_ru_init;
int corr_en_init;
GtkWidget *misspelled;
GtkWidget *correct;
};

struct _SconfcorrectClass
{
 GtkVBoxClass parent_class;

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


GType      sconfcorrect_get_type (void) G_GNUC_CONST;
GtkWidget* sconfcorrect_new (Sven *sven,gchar *name,int device);
void sconfcorrect_save (Sconfcorrect *sccor);

G_END_DECLS

#endif /* __SCONFCORRECT_H__ */

