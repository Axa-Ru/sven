/* Sven multimedia keyboard deamon
 * plugin.h
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: morenko@ukr.net
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
#ifndef __PLUGIN_H_
#define __PLUGIN_H_

typedef struct
{
	void (*about) (gpointer sven);	/* Show the about window */
	void (*configure) (gpointer sven,gpointer data);/*show the configure window*/
	gpointer (*create) (gpointer sven);
	void (*command) (gpointer sven,gpointer data,char *function,char *title); /* This function will be called when its time  for the plugin to  create itself   */
	int (*send_event) (gpointer sven,gpointer data,gpointer xev); /*send event  */
	int (*send_event_key) (gpointer sven,gpointer data,gpointer info); /*send event  */
	void (*destroy) (gpointer sven,gpointer data); /* This function will be destroy data */
	GSList *(*get_function_list)(gpointer sven); /* Get name function list */
	gpointer (*get_menu)(gpointer sven,gpointer data); /* Get menu widget*/
	gpointer	private_data; /* Eventually filled in by the plugin */
}
SvenPlugin;

typedef struct
{
	GModule *module;
	gchar *name;
	gchar *info;
	gchar *filename;
	int enable;
	SvenPlugin *func;
}
SvenPluginPriv;

#ifdef __cplusplus
extern "C" {
#endif

void sven_init_plugin(gpointer sven);
void sven_plugin_add(gpointer sven,gchar *filename,int enable);
void sven_plugin_load(gpointer sven,gchar *filename,int enable);
void sven_plugin_unload_free (gpointer sven,gchar *filename);
void sven_plugin_unload (gpointer sven,gchar *filename);
void sven_plugin_shutdown (gpointer sven);

void sven_read_plugins(gpointer sven);
SvenPluginPriv *sven_plugin_find(gpointer sven,const gchar *name);


#ifdef __cplusplus
};
#endif

#endif /* __PLUGIN_H_ */

