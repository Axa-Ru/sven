/* Sven multimedia keyboard deamon
 * plugin.c
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
 
#include <gtk/gtk.h>
#include <time.h> /* nanosleep() */
#include <string.h> /* strchr() */
#include <unistd.h> /* exit() */
#include <stdlib.h> /* exit() on Solaris */
#include <gmodule.h>

#include "sven.h"
#include "plugin.h"

/**
 * sven_plugin_is_dupe:
 * @name: a #gchar
 *
 * check(verify) here dublicate plugins
 **/
static gboolean sven_plugin_is_dupe(gpointer data,const gchar *name)
{
	Sven *sven = (Sven *)data;
	if((sven==NULL)|| (name==NULL))
		return FALSE;

	if(sven_plugin_find(sven,name)!=NULL)
		return TRUE;
	
	return  FALSE;
}

/**
 * sven_read_plugins:
 *
 *find plugins
 **/
void sven_read_plugins(gpointer data)
{
	Sven *sven = (Sven *)data;
	gint i;
	gchar *dirname=NULL;
	gchar *dirs[] = { 0,0,NULL};
	const gchar *filename=NULL;
	GError *error = NULL;
	GPatternSpec* ps;
	GDir* gd;

	if (sven==NULL)
		return;
	
	dirs[0] = g_strconcat(g_get_home_dir(),"/",".sven/plugins",NULL);
	dirs[1] = LIBDIR;
	for (i = 0; dirs[i] != NULL; ++i) 
	{
		dirname = dirs[i];

		printf("DIR:%s\n",dirname);	   

		if (!g_file_test(dirname, G_FILE_TEST_IS_DIR))
			continue;

		ps = g_pattern_spec_new("*.so");
		gd = g_dir_open(dirname ,0,&error);
		filename = g_dir_read_name(gd);
		while (filename)
		{
			if (g_pattern_match(ps, strlen(filename), filename, NULL))
			{
				if (!sven_plugin_is_dupe(sven,filename))
				{
					int i=0;
					char *temp1=NULL;
					char *temp=g_strdup(filename);
					sven_cfg_read_int(sven->config->all,"plugins",temp,&i);
					temp1=g_strconcat(dirname,"/",filename,NULL);
					sven_plugin_add(sven,temp1,i);
					g_free(temp);temp=NULL;
					g_free(temp1);temp1=NULL;
				}
			     	else
					printf("plugin %s, its already loaded but appears in another location\n",filename);
			}
			filename = g_dir_read_name(gd);
		}
		g_dir_close(gd);
		g_pattern_spec_free(ps);
	}
	g_free(dirs[0]);
	//free(dirname);
}
 
void sven_init_plugin(gpointer data)
{
	Sven *sven = (Sven *)data;
	gchar *temp, *temp2=NULL;
	if(sven==NULL)
		return;

	if (sven->plugin->arg_plugin_file)
	{
		temp = sven->plugin->arg_plugin_file;
		while ((temp2 = strchr(temp, ',')) != NULL)
		{
			(*temp2) = '\0';
			sven_plugin_add(sven,temp,1);
			temp = temp2 + 1;
		}
		sven_plugin_add(sven,temp,1);
		
		free(temp);temp=NULL;
		free(temp2);temp2=NULL;
	}

	
	
	sven_read_plugins(sven);
	
	//sven_plugin_shutdown(sven);
/*	SvenPluginPriv *priv=sven_plugin_find(sven,"time.so");
	printf("Priv:%s\n",priv->name);
	if(priv->func==NULL)
		printf("Priv=NULL:\n");
	sven_plugin_load(sven,"/home/more/.sven/plugins/time.so",0);
	SvenPluginPriv *priv1=sven_plugin_find(sven,"/home/more/.sven/plugins/time.so");
	printf("Priv1:%s\n",priv1->name);
	if(priv1->func==NULL)
		printf("Priv1=NULL:\n");
	sven_plugin_load(sven,"/home/more/.sven/plugins/time.so",1);
	SvenPluginPriv *priv2=sven_plugin_find(sven,"/home/more/.sven/plugins/time.so");
	printf("Priv2:%s\n",priv2->name);
	if(priv2->func==NULL)
		printf("Priv1=NULL:\n");*/
//	sven_plugin_unload (sven,"/home/more/.sven/plugins/time.so");
	/*sven_plugin_unload_free (sven,"/home/more/.sven/plugins/time.so");
	sven_plugin_shutdown (sven);*/

//	sven_plugin_unload(sven,"time.so");
}

void sven_plugin_add(gpointer data,gchar *filename,int enable)
{
	Sven *sven = (Sven *)data;
	if((sven==NULL)|| (filename==NULL))
		return;
	
	SvenPluginPriv *plug= g_slice_new0(SvenPluginPriv);
	
	printf("Plugin Add[%d]:%s\n",enable,filename);
	if (plug) 
	{
		char *(*get_plugin_name)();
		char *(*get_plugin_info)();
		gboolean test;
		
		plug->module = g_module_open(filename,0);

		if (!plug->module)
		{
			g_print("Could not load plugin: \"%s\" \nDynamic Loader Error: %s\n",filename, g_module_error());
			g_slice_free (SvenPluginPriv, plug);
			return;
		}
	
		test = g_module_symbol(plug->module, "get_plugin_name",(gpointer *)&get_plugin_name);
		if (!test)
		{
			g_print("Dynamic loader error while loading plugin: \"%s\":%s\n",filename,g_module_error());
			g_module_close(plug->module);
			g_slice_free (SvenPluginPriv, plug);
			return;
		}
		plug->filename=g_strdup(filename);
		plug->name=g_strdup((char *)(*get_plugin_name)());

		test = g_module_symbol(plug->module, "get_plugin_info",(gpointer *)&get_plugin_info);
		if (!test)
		{
			plug->info=_("Not information");
		}
		else
			plug->info=g_strdup((char *)(*get_plugin_info)());
		
		printf("File:%s;\n Name:%s;\n Info:%s\n Enable:%d\n",plug->filename,plug->name,plug->info,enable); 	

		
		if(enable)
		{
			SvenPlugin *(*init_plugin)();
			test = g_module_symbol(plug->module, "init_plugin",(gpointer*)&init_plugin);
			if (!test)
			{
				g_print("Dynamic loader error while loading plugin: \"%s\":%s\n",filename,g_module_error());
				g_module_close(plug->module);
				g_slice_free (SvenPluginPriv, plug);
				return;
			}
			plug->func=(*init_plugin)();
			if (plug->func->create!=NULL)
				plug->func->private_data=plug->func->create(sven);
			else
				plug->func->private_data=NULL;
			
			if(plug->func)
				printf("Ok. Module load\n");
			plug->enable=1;
		}
		else
		{
			plug->func=NULL;
			plug->enable=0;
			g_module_close(plug->module);
		}
		
		sven->plugin->list = g_slist_append(sven->plugin->list,(gpointer ) plug);

	//	sven_plugin_find(sven,"test");
		
	}
	else
		printf("Not memory alloc\n");
}

void sven_plugin_load(gpointer data,gchar *filename,int enable)
{
	Sven *sven = (Sven *)data;
	if((sven==NULL)|| (filename==NULL))
		return;
	
	if (enable==0) 
	{
		sven_plugin_unload (sven,filename);
	}
	else if(enable==1)
	{
		SvenPluginPriv *plug=sven_plugin_find(sven,filename);

		gboolean test;
		
		plug->module = g_module_open(filename,0);

		if (!plug->module)
		{
			g_print("Could not load plugin: \"%s\" \nDynamic Loader Error: %s\n",filename, g_module_error());
			g_slice_free (SvenPluginPriv, plug);
		}
	
		{
			SvenPlugin *(*init_plugin)();
			test = g_module_symbol(plug->module, "init_plugin",(gpointer*)&init_plugin);
			if (!test)
			{
				g_print("Dynamic loader error while loading plugin: \"%s\":%s\n",filename,g_module_error());
				g_module_close(plug->module);
				g_slice_free (SvenPluginPriv, plug);
				return;
			}
			plug->func=(*init_plugin)();
			if (plug->func->create!=NULL)
				plug->func->private_data=plug->func->create(sven);
			else
				plug->func->private_data=NULL;

			plug->enable=1;
		}
	}
}

void sven_plugin_unload (gpointer data,gchar *filename)
{
	Sven *sven = (Sven *)data;
	if((sven==NULL)|| (filename==NULL))
		return;
	
	SvenPluginPriv *plug=sven_plugin_find(sven,filename);
	
	if (plug->func->destroy != NULL)
		plug->func->destroy(sven,plug->func->private_data);
	
	plug->func=NULL;
	plug->enable=0;	
	if (plug->module != NULL)
		g_module_close (plug->module);
//	g_slice_free (SvenPluginPriv, plug);	
}
void sven_plugin_unload_free (gpointer data,gchar *filename)
{
	Sven *sven = (Sven *)data;
	if((sven==NULL)|| (filename==NULL))
		return;
	
	SvenPluginPriv *plug=sven_plugin_find(sven,filename);
	
	sven->plugin->list = g_slist_remove (sven->plugin->list, (gpointer ) plug);
	
	if (plug->func->destroy != NULL)
		plug->func->destroy(sven,plug->func->private_data);
	
	plug->enable=0;	
	
	if (plug->module != NULL)
		g_module_close (plug->module);
	
	g_free (plug->name);
	g_free (plug->info);
	g_free (plug->filename);
	
	g_slice_free (SvenPluginPriv, plug);
}

void sven_plugin_shutdown (gpointer data)
{
	Sven *sven = (Sven *)data;
	GSList *l = NULL;

	if(sven==NULL)
		return;
	
	for (l = sven->plugin->list; l; l=l->next)
	{
		SvenPluginPriv *plug = (SvenPluginPriv *)l->data;
		
		if (plug->func->destroy != NULL)
			plug->func->destroy(sven,plug->func->private_data);
			
		if (plug->module != NULL)
			g_module_close (plug->module);
		
		g_free (plug->name);
		g_free (plug->info);
		g_free (plug->filename);

		g_slice_free (SvenPluginPriv, plug);
	}
	
	g_slist_free (sven->plugin->list);
	sven->plugin->list = NULL;
}


SvenPluginPriv *sven_plugin_find(gpointer data,const gchar *name)
{
	Sven *sven = (Sven *)data;
	GSList *l = NULL;

	if((sven==NULL)|| (name==NULL))
		return NULL;

//	printf("Find name:%s\n", name);	

	for (l = sven->plugin->list; l; l=l->next)
	{
		SvenPluginPriv *plug = (SvenPluginPriv *)l->data;
		if(plug==NULL)
			return NULL;
			
//		printf("Find:%s - %s - %s\n",(gchar *)plug->name, (gchar *)plug->filename,g_basename((gchar *)plug->filename));

		if (!g_strcasecmp(g_basename((gchar *)plug->filename),g_basename(name)))
			return plug;	
	}
	
	return NULL;
}

