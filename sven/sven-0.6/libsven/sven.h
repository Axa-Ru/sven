/* Sven multimedia keyboard deamon
 * sven.h
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

#ifndef __SVEN_H_
#define __SVEN_H_


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

#ifdef DEBUG
#define DEBUG_MSG g_print
#else /* not DEBUG */
#ifdef __GNUC__
#define DEBUG_MSG(args...)
 /**/
#else/* notdef __GNUC__ */
extern void g_none(gchar *first, ...);
#define DEBUG_MSG g_none
#endif /* __GNUC__ */
#endif /* DEBUG */

#include "configfile.h"
#include "gtkosd.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	ConfigFile *all;
	ConfigFile *key;
	ConfigFile *mouse;
} ConfigSven;

typedef struct 
{
	/* Plugins name and list file */
	gchar *arg_plugin_file;
	GSList *list;
} SPlugin;

typedef struct 
{
	ConfigSven *config;
	SPlugin *plugin;
	GtkStatusIcon *tray_icon;
	int osd_enable;
	GtkOSD *osd;
} Sven;

#ifdef __cplusplus
};
#endif

#endif /* __SVEN_H_ */

