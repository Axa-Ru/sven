/* Sven multimedia keyboard deamon
 * plugin.c
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
 
#include <gtk/gtk.h>
#include <time.h> /* nanosleep() */
#include <string.h> /* strchr() */
#include <unistd.h> /* exit() */
#include <stdlib.h> /* exit() on Solaris */
#include <gmodule.h>

#include "sven.h"
#include "plugin.h"

static gint plugins_read = 0;
static GSList *plugins_list_file = NULL, *plugin_names=NULL;

static void sven_read_plugins(Sven *sven);

int d_iplist_compare(const void *a, const void *b)
{
	return strcmp(((char *) a), ((char *) b));
}

/**
 * sven_plugin_is_dupe:
 * @name: a #gchar
 *
 * check(verify) here dublicate plugins
 **/
static gboolean sven_plugin_is_dupe(const gchar *name)
{
	GSList *l;
	if (plugin_names == NULL)
	{
		plugin_names = g_slist_append(plugin_names,g_strdup(name));
		return FALSE;
	}
	for (l = plugin_names; l ; l = l->next)
	{
		if (!g_strcasecmp((gchar *)l->data, name))
		return TRUE;
	}
	plugin_names = g_slist_append(plugin_names, g_strdup(name));
	return  FALSE;
}

void sven_plugin_start(Sven *sven)
{
     	g_return_if_fail(sven != NULL);

	if (!plugins_read)
	{
		sven_read_plugins(sven);
		plugins_read = 1;
	}
}

/**
 * sven_read_plugins:
 *
 *find and load plugins
 **/
static void sven_read_plugins(Sven *sven)
{
	gint i;
	GSList *l = NULL;
	gchar *dirname=NULL;
	gchar *dirs[] = { 0,0,NULL};
	const gchar *filename=NULL;
	GError *error = NULL;
	GPatternSpec* ps;
	GDir* gd;

	dirs[0] = g_strconcat(g_get_home_dir(),"/",".sven/plugins",NULL);
	dirs[1] = LIBDIR;
	for (i = 0; dirs[i] != NULL; ++i) 
	{
		DEBUG_MSG("|---------------START find katalog %s -------------------------------|\n",dirs[i]);
		dirname = dirs[i];
	   
		if (!g_file_test(dirname, G_FILE_TEST_IS_DIR))
			continue;

		ps = g_pattern_spec_new("*.so");
		gd = g_dir_open(dirname ,0,&error);
		filename = g_dir_read_name(gd);
		while (filename)
		{
			if (g_pattern_match(ps, strlen(filename), filename, NULL))
			{
				gchar *path = g_strconcat(dirname, filename, NULL);
				DEBUG_MSG("filename %s has a match!\n",filename);
				DEBUG_MSG("dir %s\n",path);
				if (!sven_plugin_is_dupe(filename))
				{
					plugins_list_file = g_slist_append(plugins_list_file, g_strconcat(dirname,"/",filename,NULL));
					 DEBUG_MSG("Plugid detected: %s \n",(gchar *)plugins_list_file->data);
				}
			     	else
					DEBUG_MSG("plugin %s, its already loaded but appears in another location\n",filename);
				g_free(path);
			}
			filename = g_dir_read_name(gd);
		}
		g_dir_close(gd);
		g_pattern_spec_free(ps);
		DEBUG_MSG("|-----------------------------END find katalog %s ---------------------------|\n",dirs[i]);
	}
	g_free(dirs[0]);

	for (l = plugins_list_file; l; l=l->next)
	{
		load_plugin(sven,l->data);
	    
	}
}
/**
*load_plugin:
 * return struct SvenPlugin
 * @plugin_name: a #gchar
 *
 * Returns a struct load plugins
 *
 *load plugin
 */
 
SvenPlugin * load_plugin(Sven *sven,gchar *plugin_name)
{
	GModule *module;
	SvenPlugin *plugin = NULL;
	SvenPlugin *(*init_plugin)();
	gboolean test;

   	module = g_module_open(plugin_name,0);

     	if (!module)
	{
		g_print("Could not load plugin: \"%s\" \nDynamic Loader Error: %s\n",plugin_name, g_module_error());
		return NULL;
	}

	test = g_module_symbol(module, "init_plugin",(gpointer) &init_plugin);
	if (!test)
	{
		  g_print("Dynamic loader error while loading plugin: \"%s\":%s\n",plugin_name,g_module_error());
		  g_module_close(module);
		  return NULL;
	}

	plugin = (*init_plugin)();
	
	plugin->filename = g_strdup(plugin_name);
	if (!plugin)
	{
		  g_print("error when initilizing plugin: \"%s\"\n",plugin_name);
		  g_module_close(module);
		  return NULL;
	}

	sven->plugins = g_slist_append(sven->plugins, plugin);
	sven->plugins_name= g_slist_append(sven->plugins_name,g_strdup(g_basename(plugin_name)));
	if(plugin)
	     g_print(_("Plugin: \"%s\" loaded!\n"),plugin_name);
	if (!plugin)
	     g_print(_("Plugin: \"%s\" was not loaded, moving on!\n"),plugin_name);

	return plugin;
}

gboolean plugin_find_pos(Sven *sven,char *t)
{

	return (g_slist_position(sven->plugins_name,g_slist_find_custom(sven->plugins_name,g_basename(t), d_iplist_compare)));
}
