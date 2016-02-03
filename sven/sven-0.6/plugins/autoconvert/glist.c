/* Sven multimedia keyboard deamon
 * wlist.c
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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "autoconvert.h"
#include "glist.h"
#include "xutil.h"
#include "../../libsven/xlib.h"

void add_keysym_to_list(XEvent * event,gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	if (! aconv)
		return;
		
	Skeysym *sks;
	sks= g_slice_new0(Skeysym);
	
	if (sks) 
	{
		sks->key= get_current_keysym(event);
		sks->key_all= get_current_keysym_all(event);
		sks->mod= get_current_modifier(event);
		aconv->keyboard->ks = g_list_append(aconv->keyboard->ks,(gpointer ) sks);
	}
	if(get_current_keysym(event)=='[')
		aconv->keyboard->gstr=g_string_append_c(aconv->keyboard->gstr,'{');
	else
		aconv->keyboard->gstr=g_string_append_c(aconv->keyboard->gstr,get_current_keysym(event));

	return ;
}

void add_keysym_to_list_num(XEvent * event,gpointer data,int num)
{
	AConvert *aconv = (AConvert *)data;
	if (! aconv)
		return;
		
	Skeysym *sks;
	sks= g_slice_new0(Skeysym);
	
	if (sks) 
	{
		sks->key= get_current_keysym(event);
		sks->key_all= get_current_keysym_all(event);
		sks->mod= get_current_modifier(event);
		aconv->keyboard->ks = g_list_insert(aconv->keyboard->ks,(gpointer ) sks,num);
	}
	if(get_current_keysym(event)=='[')
		aconv->keyboard->gstr=g_string_insert_c(aconv->keyboard->gstr,num,'{');
	else
		aconv->keyboard->gstr=g_string_insert_c(aconv->keyboard->gstr,num,get_current_keysym(event));

	return ;
}
void add_keysym_to_list_num_insert(XEvent * event,gpointer data,int num)
{
	AConvert *aconv = (AConvert *)data;
	if (! aconv)
		return;
		
	Skeysym *sks = (Skeysym *) g_list_nth_data(aconv->keyboard->ks,num);

	if (sks) 
	{
		sks->key= get_current_keysym(event);
		sks->key_all= get_current_keysym_all(event);
		sks->mod= get_current_modifier(event);
/*		winf->ks = g_list_insert(winf->ks,(gpointer ) sks,num);*/
		aconv->keyboard->ks= g_list_remove (aconv->keyboard->ks, (gpointer ) g_list_nth_data(aconv->keyboard->ks,num+1));
	}
	if(get_current_keysym(event)=='[')
		aconv->keyboard->gstr=g_string_insert_c(aconv->keyboard->gstr,num,'{');
	else
		aconv->keyboard->gstr=g_string_insert_c(aconv->keyboard->gstr,num,get_current_keysym(event));
	aconv->keyboard->gstr=g_string_erase(aconv->keyboard->gstr,num+1,1);

	return ;
}

void dell_keysym_to_list(gpointer data,int num)
{
	AConvert *aconv = (AConvert *)data;
	if (! aconv)
		return;
		
	if (aconv->keyboard->ks) 
	{
		gpointer *temp = g_list_nth_data(aconv->keyboard->ks,num-1);
		aconv->keyboard->ks= g_list_remove (aconv->keyboard->ks, temp);
		g_slice_free (Skeysym, (Skeysym *)temp);
		//g_free(temp);
	}
	aconv->keyboard->gstr=g_string_erase(aconv->keyboard->gstr,num-1,1);
}

void free_keysym_to_list(gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	
	if (! aconv)
		return;
	
	if ((! aconv->keyboard->ks)||(!aconv->keyboard->gstr))
		return;

	GList *l = NULL ;

	for (l = aconv->keyboard->ks; l; l=l->next)
	{
		Skeysym *sks=(Skeysym *)l->data;
		if (sks)
		{
			g_slice_free (Skeysym, sks);
			//g_free(sks);
			
		}
	}
	g_list_free (l);
	g_list_free (aconv->keyboard->ks);
	aconv->keyboard->ks = NULL;
	
	aconv->keyboard->gstr=g_string_erase(aconv->keyboard->gstr,0,-1);
	
	aconv->keyboard->iterator=0;
	//DEBUG_MSG("Free List\n");
}

 

