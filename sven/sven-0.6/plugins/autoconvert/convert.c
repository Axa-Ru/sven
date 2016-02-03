 /* Sven multimedia keyboard deamon
 * autoconvert.c plugin for automatic convert text for russian and english layout
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
#include <X11/X.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pcre.h>

#include "rudict.h"
#include "endict.h"

#include "autoconvert.h"
#include "wlist.h"
#include "glist.h"
#include "convert.h"
#include "flag.h"
#include "xutil.h"

#include "../../libsven/sven.h"
#include "../../libsven/utils.h"
#include "../../libsven/xlib.h"
#include "../../libsven/emulate_input.h"


void xConvert_text_case (gpointer data)
{
	int mod=0;
	size_t slen;
	AConvert *aconv = (AConvert *)data;

	if ((aconv==NULL))
		return;

	aconv->keyboard->win=get_active_window(aconv->keyboard->dpy);

	WInfo *info = win_find(aconv->keyboard->win,aconv);
	if (!info) 
		return;
	
	slen =aconv->keyboard->gstr->len;//g_list_length (info->ks);

	if(slen<=0)
	{
		free_keysym_to_list(aconv);
		return ;
	}
	
	if(aconv->dbase->count_case > 2)
		aconv->dbase->count_case=0;
	
	
	if(aconv->dbase->count_case==0)
	{
		aconv->dbase->count_case++;
	
		mod=(8192*info->cur_group);

		delete_char_in_win(info->win,aconv->keyboard->iterator);
		backspace_char_in_win(info->win,slen-aconv->keyboard->iterator);
		aconv->keyboard->iterator=0;
		
		{
			GList *p = g_list_first (aconv->keyboard->ks);
	
			while (p)
			{
				Skeysym *sks=(Skeysym *)p->data;
				send_key_to_win(info->win,XKeysymToKeycode(aconv->keyboard->dpy,sks->key),mod+1);
				p = g_list_next (p);
			}
		}
		return ;
	}
	else if(aconv->dbase->count_case==1)
	{
		aconv->dbase->count_case++;
	
		mod=(8192*info->cur_group);
		
		delete_char_in_win(info->win,aconv->keyboard->iterator);
		backspace_char_in_win(info->win,slen-aconv->keyboard->iterator);
		aconv->keyboard->iterator=0;
		
		{
			int i=1;
			GList *p = g_list_first (aconv->keyboard->ks);
	
			while (p)
			{
				Skeysym *sks=(Skeysym *)p->data;
				send_key_to_win(info->win,XKeysymToKeycode(aconv->keyboard->dpy,sks->key),mod+i);
				p = g_list_next (p);
				i=0;
			}
		}
	}
	else if(aconv->dbase->count_case==2)
	{
		aconv->dbase->count_case++;
	
		mod=(8192*info->cur_group);

		delete_char_in_win(info->win,aconv->keyboard->iterator);
		backspace_char_in_win(info->win,slen-aconv->keyboard->iterator);
		aconv->keyboard->iterator=0;
		
		{
			GList *p = g_list_first (aconv->keyboard->ks);
	
			while (p)
			{
				Skeysym *sks=(Skeysym *)p->data;
				send_key_to_win(info->win,XKeysymToKeycode(aconv->keyboard->dpy,sks->key),mod);
				p = g_list_next (p);
			}
		}
		return ;
	}
	else
	   aconv->dbase->count_case=0;
}

void xConvert_text (gpointer data)
{
	int mod=0;
	size_t slen;
	AConvert *aconv = (AConvert *)data;
	if(aconv==NULL)
		return;
		
	aconv->keyboard->win=get_active_window(aconv->keyboard->dpy);

	WInfo *info = win_find(aconv->keyboard->win,aconv);
	if (!info) 
		return;
	
	slen =aconv->keyboard->gstr->len;//g_list_length (info->ks);

	if(slen<=0)
	{
		free_keysym_to_list(aconv);
		return ;
	}
		
	if(info->cur_group==(aconv->sxkb->ngroups-1))
		info->cur_group=0;
	else if (info->cur_group < (aconv->sxkb->ngroups-1))
		info->cur_group++;
	else
		info->cur_group=0;
			
	mod=(8192*info->cur_group);
	XkbLockGroup(aconv->keyboard->dpy, XkbUseCoreKbd,info->cur_group);
	update_flag(aconv->sxkb->group2info[aconv->sxkb->cur_group],aconv);
	
	delete_char_in_win(info->win,aconv->keyboard->iterator);
	backspace_char_in_win(info->win,slen-aconv->keyboard->iterator);
	aconv->keyboard->iterator=0;
	
	{
		GList *p = g_list_first (aconv->keyboard->ks);

		while (p)
	        {
			Skeysym *sks=(Skeysym *)p->data;
			send_key_to_win(info->win,XKeysymToKeycode(aconv->keyboard->dpy,sks->key),mod+sks->mod);
			p = g_list_next (p);
	        }
	}
}

void xConvert_text_eng (gpointer data,WInfo *info)
{

	int i=0;
	int mod=0;
	int  slen;
	AConvert *aconv = (AConvert *)data;
	
	if((aconv==NULL)||(info==NULL))
		return;
	
	slen =aconv->keyboard->gstr->len;//g_list_length (info->ks);

	if(slen<=0)
	{
		free_keysym_to_list(aconv);
		return ;
	}

	
	for (i = 0; i <= aconv->sxkb->ngroups; i++) 
	{
		if ((g_strcasecmp(aconv->sxkb->group2info[i]->name, "English") == 0)
		    || (g_strcasecmp(aconv->sxkb->group2info[i]->name, "US/ASCII") == 0)
		    || (g_strcasecmp(aconv->sxkb->group2info[i]->name, "en_US") == 0)
		    || (g_strcasecmp(aconv->sxkb->group2info[i]->name, "Maltese (US layout)") == 0)
		    || (g_strcasecmp(aconv->sxkb->group2info[i]->name, "U.S. English") == 0))
			break;
	}
	
	info->cur_group=i;
	mod=(8192*i);
	XkbLockGroup(aconv->keyboard->dpy, XkbUseCoreKbd,i);
	update_flag(aconv->sxkb->group2info[aconv->sxkb->cur_group],aconv);

	delete_char_in_win(info->win,aconv->keyboard->iterator);
	backspace_char_in_win(info->win,slen-aconv->keyboard->iterator);
	aconv->keyboard->iterator=0;
	
	{
		GList *p = g_list_first (aconv->keyboard->ks);

		while (p)
	        {
			Skeysym *sks=(Skeysym *)p->data;
			send_key_to_win(info->win,XKeysymToKeycode(aconv->keyboard->dpy,sks->key),mod+sks->mod);
			p = g_list_next (p);
	        }
	}
}

void xConvert_text_rus (gpointer data,WInfo *info)
{
	int i=1;
	int mod=0;
	size_t slen;
	AConvert *aconv = (AConvert *)data;
	
	if((aconv==NULL)||(info==NULL))
		return;
	
	slen =aconv->keyboard->gstr->len;//g_list_length (info->ks);

	if(slen<=0)
	{
		free_keysym_to_list(aconv);
		return ;
	}
	
	for (i = 0; i <= aconv->sxkb->ngroups; i++) 
	{
		DEBUG_MSG("%s\n",aconv->sxkb->group2info[0]->name);
		if ((g_strcasecmp(aconv->sxkb->group2info[i]->name, "Russian") == 0)
		    || (g_strcasecmp(aconv->sxkb->group2info[i]->name, "Russian(phonetic)") == 0)
		    || (g_strcasecmp(aconv->sxkb->group2info[i]->name, "Russia - Winkeys") == 0))
			break;
	}
	
	info->cur_group=i;
	mod=(8192*i);
	XkbLockGroup(aconv->keyboard->dpy, XkbUseCoreKbd,i);
	update_flag(aconv->sxkb->group2info[aconv->sxkb->cur_group],aconv);
	
	delete_char_in_win(info->win,aconv->keyboard->iterator);
	backspace_char_in_win(info->win,slen-aconv->keyboard->iterator);
	aconv->keyboard->iterator=0;
	
	{
		GList *p = g_list_first (aconv->keyboard->ks);

		while (p)
	        {
			Skeysym *sks=(Skeysym *)p->data;
			send_key_to_win(info->win,XKeysymToKeycode(aconv->keyboard->dpy,sks->key),mod+sks->mod);
			p = g_list_next (p);
	        }
	}
}

int auto_convert_text(gpointer data,WInfo *info)
{
	AConvert *aconv = (AConvert *)data;
	Sven *sven = (Sven *)aconv->sven;
	int ovector[30],ret=0;
	//gchar *text=NULL;
	int i=1;
	int f_name=0;

	if((sven==NULL)||(info==NULL)||(aconv==NULL))
		return-1;
	
	sven_cfg_read_int (sven->config->all, "autoconvert", "auto_switch",&i);
	if(i==0)
		return -1;
	sven_cfg_read_int(sven->config->all, "conv_win_list",info->name,&f_name);
	if(f_name==1)
		return -1;
		
	if(aconv->keyboard->gstr->str==NULL)
		return -1;

	//text=get_glist_string(info->ks);
	//printf("Sting convert:%s\n",text);
	/*if(aconv->keyboard->gstr->str!=NULL)
		DEBUG_MSG("Sting convert1:%s\n",aconv->keyboard->gstr->str);*/
		
	if ((g_strcasecmp(aconv->sxkb->group2info[info->cur_group]->name, "Russian") == 0)
	    || (g_strcasecmp(aconv->sxkb->group2info[info->cur_group]->name, "Russian(phonetic)") == 0)
	    || (g_strcasecmp(aconv->sxkb->group2info[info->cur_group]->name, "Russia - Winkeys") == 0))
	{
		ret =pcre_exec(aconv->dbase->english_regc,NULL,aconv->keyboard->gstr->str, aconv->keyboard->gstr->len,0,0,ovector,30);
		if(ret > 0)
			xConvert_text_eng(aconv,info);
		else 
		{
			ret =pcre_exec(aconv->dbase->english_regc1,NULL,aconv->keyboard->gstr->str,aconv->keyboard->gstr->len,0,0,ovector,30);
			if (ret > 0)
				xConvert_text_eng(aconv,info);	
		}
	}	
	else if ((g_strcasecmp(aconv->sxkb->group2info[info->cur_group]->name, "English") == 0)
		 || (g_strcasecmp(aconv->sxkb->group2info[info->cur_group]->name, "US/ASCII") == 0)
		 || (g_strcasecmp(aconv->sxkb->group2info[info->cur_group]->name, "en_US") == 0)
		 || (g_strcasecmp(aconv->sxkb->group2info[info->cur_group]->name, "Maltese (US layout)") == 0)
		 || (g_strcasecmp(aconv->sxkb->group2info[info->cur_group]->name, "U.S. English") == 0))
	{
		ret =pcre_exec(aconv->dbase->russian_regc,NULL,aconv->keyboard->gstr->str,aconv->keyboard->gstr->len,0,0,ovector,30);
		if(ret > 0)
			xConvert_text_rus(aconv,info);
		else 
		{	
			ret =pcre_exec(aconv->dbase->russian_regc1,NULL,aconv->keyboard->gstr->str,aconv->keyboard->gstr->len,0,0,ovector,30);
			if (ret > 0)
				xConvert_text_rus(aconv,info);
		}
	}
	//g_free(text);text=NULL;
	return 1;
}


void dictionary_init(gpointer data) 
{
	const char *error;
	int erroffset;
	AConvert *aconv = (AConvert *)data;
	
	aconv->dbase = g_new0 (DictBase, 1);
	
	aconv->dbase->russian_regc = pcre_compile(RUDICTIONARY,PCRE_CASELESS, &error,&erroffset,NULL);
	if (!aconv->dbase->russian_regc) 
	{
		printf("dictionary_init_RU, ERROR, %s\n",error);
	}
	aconv->dbase->russian_regc1 = pcre_compile(RUDICTIONARY1,PCRE_CASELESS,&error,&erroffset,NULL);
	if (!aconv->dbase->russian_regc1) 
	{
		printf("dictionary_init_RU, ERROR, %s\n",error);
	}
	
	aconv->dbase->english_regc = pcre_compile(ENDICTIONARY,PCRE_CASELESS, &error,&erroffset,NULL);
	if (!aconv->dbase->english_regc) 
	{
		printf("dictionary_init_EN, ERROR0, %s\n",error);
	}
	
	aconv->dbase->english_regc1 = pcre_compile(ENDICTIONARY1,PCRE_CASELESS, &error,&erroffset,NULL);
	if (!aconv->dbase->english_regc1) 
	{
		printf("dictionary_init_EN, ERROR1, %s\n",error);
	}
}



