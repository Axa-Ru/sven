/* Sven multimedia keyboard deamon
 * plugin.h
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
#ifndef __PLUGIN_H_
#define __PLUGIN_H_

typedef struct
{
	int sistem;
	gchar *name;
	gchar *filename;
	void (*about) (void);	/* Show the about window */
	void (*configure) (Sven *sven);/*show the configure window*/
	void (*create_plugin) (Sven *sven,char *function);/* This function will be called when its time  for the plugin to  create itself   */
	GSList *(*add_function_list)(void);
}
SvenPlugin;

#ifdef __cplusplus
extern "C" {
#endif

void sven_plugin_start(Sven *sven);
SvenPlugin *load_plugin(Sven *sven,gchar *plugin_name);
gboolean plugin_find_pos(Sven *sven,char *t);
int d_iplist_compare(const void *a, const void *b);

#ifdef __cplusplus
};
#endif

#endif /* __PLUGIN_H_ */

