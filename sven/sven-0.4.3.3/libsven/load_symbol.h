/* Sven multimedia keyboard deamon
 * load_symbol.h
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
#ifndef __LOAD_SYMBOL_H_
#define __LOAD_SYMBOL_H_

/*struct student 
{ 
	KeySym key;
	int mod;
};*/

void load_symbol_conv_to_ru(gpointer data);
char *get_load_symbol_conv_to_ru(gpointer data,gchar ch);
gchar *convert_symbol_to_ru(gpointer data,gchar *text);
void load_symbol_init(gpointer data);
char *get_load_symbol_translete(gpointer data,gchar ch);
void load_symbol_translete(gpointer data);
gchar *convert_symbol_to_translete(gpointer data,gchar *text);
KeySym get_load_symbol(gpointer data,gchar ch);

gchar* conv_utfchar_to_en (gpointer data,gchar *buf);
gchar* conv_utfstr_to_en (gpointer data,gchar *buf);
int get_modifer_to_char (gpointer data,gchar *ch,gchar *flag);

#endif /* __LOAD_SYMBOL_H_ */

