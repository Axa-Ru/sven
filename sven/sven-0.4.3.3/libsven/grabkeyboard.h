/* Sven multimedia keyboard deamon
 * grabkeyboard.h
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
#ifndef __GRABKEYBOARD_H_
#define __GRABKEYBOARD_H_

//BufferActions
#define KLB_NO_ACTION			0	//modifier, function etc
#define KLB_ADD_SYM				1	//alpha
#define KLB_DEL_SYM_RIGHT	2	//del, bkspace etc
#define KLB_DEL_SYM_LEFT	3	// -//-
#define KLB_MOVE_RIGHT		4	//cursor right
#define KLB_MOVE_LEFT			5	//cursor left
#define KLB_FLUSH					6	//word end (space etc)
#define KLB_CLEAR					7	//Home, End etc
#define KLB_SWITCH_MODE		8	//Insert
#define KLB_ENTER		9

//mode
#define KLB_INSERT_MODE		10
#define KLB_REPLACE_MODE	11

#define MAX_WORD_LENGTH 64

#define STRING_SIZE 40

typedef struct 
{
	KeySym key;
	int mod;
} Skeysym;

typedef struct 
{
	Display *dpy;
	Window win;
	//Window work_win;
	gchar  *text;
	GSList *ks;
	gchar  *alltext;
	int cur_group;
	int mode;
	int iterator;
} WInfo;

typedef struct
{
	GSList *list;
	int coun_case;
	
	gchar  *conv_lang;
	gchar  *conv_focus;
	gchar  *conv_case;
	
	GHashTable *htsym;
	GHashTable *httrans;
	GHashTable *ht_convru;
	
	pcre *russian_regc;
	pcre *russian_regc1;
	pcre *english_regc;
	pcre *english_regc1;
	int auto_switch;
	
	ConfigFile *utf_char;
	
	ConfigFile *corr_ru;
	int corr_ru_init;
	ConfigFile *corr_en;
	int corr_en_init;
} Mkeyboard;

//char *KeyEventToString (XEvent * ev);
void grab_keyboard_init (int   argc,char *argv[],gpointer data);
gboolean get_key_win(gpointer data);
char xEvent_getCurrentChar (XEvent * event);
void xConvert_text(gpointer data);
void xConvert_text_focus(gpointer data);
void xConvert_text_case (gpointer data);
void xConvert_text_rus (gpointer data);
void xConvert_text_eng (gpointer data);
void corect_init(gpointer data);

#endif /* __GRABKEYBOARD_H_ */
