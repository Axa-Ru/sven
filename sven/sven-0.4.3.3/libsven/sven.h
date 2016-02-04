/* Sven multimedia keyboard deamon
 * sven.h
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


#define BOLD                     "\E[1m"
#define RED                      "\E[31m"
#define GREEN                    "\E[32m"
#define YELLOW                   "\E[33m"
#define BLUE                     "\E[34m"
#define FIOLETE                  "\E[35m"
#define LIME                     "\E[36m"
#define NORMAL                   "\E[0m"

#define walk_until(x)   while((*buf != '\0') && (*buf != x)) buf++
#define walk_until_inclusive(x) walk_until(x); buf++

#define get_int_val(var)  { \
		walk_until_inclusive(':'); buf++; \
		var = atoi(buf); \
		continue; \
	}

#define get_str_val(var)  { \
		walk_until_inclusive(':'); buf++; \
		var = g_strdup(buf); \
		continue; \
	}

#include <assert.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pcre.h>

#include "xosd.h"
#include "configfile.h"
#include "eggtrayicon.h"
#include "clipman.h"
#include "wininfo.h"
#include "grabkeyboard.h"
#include "sxkb.h"


typedef struct {
	/* Multihead stuff */
	GdkDisplay *display;
	GdkScreen *current_screen;
	GList *screens;
} MScreen;
typedef struct {
	/*xosd preference value*/
	xosd *osd;
	gchar *font;
	gchar *colour;
	gint timeout;
	gint offset;
	gint h_offset;
	gint shadow_offset;
	gint outline_offset;
	gint pos;
	gint align;
} MXosd;

typedef struct {
	/*cd , mixer and other preference value*/
	gchar *cdrom;
	gchar *mixer;
	gint x_icon;
	gint y_icon;
	gint procces_view;
	gint icon_check;
	gint icon_check_desk;
	gchar *startup_prog;
	gint startup_prog_check;
	gint enable_xosd;
	gint enable_plugin;
} Mmix;


typedef struct {
	MScreen *screen;
	MXosd  *osd;
	ConfigFile *cfgfile;
	ConfigFile *cfgfilemouse;
	Mmix *mix;
	t_clipman *clipman;
	Mkeyboard *keyboard;
	Wininfo *wininfo;
	Sxkb *sxkb;
	/* Plugins name and list file */
	GSList *plugins_name;
	GSList *plugins;
	/* Tray icon */
	EggTrayIcon *tray_icon;
	GtkTooltips *tray_icon_tooltip;
	GtkWidget *tray_flag;
	GtkWidget *popup_menu;
	GtkWidget *shell_menu;
	GtkWidget *shell;
	int vol_timer;
} Sven;



#endif /* __SVEN_H_ */

