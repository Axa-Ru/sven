/* Sven multimedia keyboard deamon
 * wininfo.h
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: more@irpin.com
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
#ifndef __WININFO_H_
#define __WININFO_H_


typedef struct {
	GtkWidget *info_win;
	GtkWidget *table;
	/*StatLine=Characters:|Words:|Edits:|Time:|Chars/min.:|Words/min.:|% of edits:*/
	GtkWidget *characters;
	int char_count;
	GtkWidget *words;
	int words_count;
	GtkWidget *chars_edits;
	int chars_edits_count;
	GtkWidget *chars_time;
	int chars_time_count;
	GtkWidget *chars_min;
	int chars_min_count;
	GtkWidget *words_min;
	int words_min_count;
	GtkWidget *edits;
	int edits_count;
	/*Time info*/
	char *time_format;
	GtkWidget *time;
	int timer;
	/*Timer info*/
	int timer_h; /*set hour*/
	int timer_m; /*set minute*/
	int timer_s; /*set seconds*/
	int timer_stat; /*timer on=1/off=0 */
	/*flag image*/
	GtkWidget *box_image;
	GtkWidget *flag_image;
	GtkTooltips *flag_tooltip;
	GtkWidget *eventbox;
} Wininfo;

Wininfo *create_wininfo (gpointer data);
gchar* locale_to_utf8 (gchar *s);
gchar* locale_from_utf8 (gchar *s);

#endif /* __WININFO_H_ */
