 /* Sven multimedia keyboard deamon
 * mixer.c module for control mixer
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
#include <X11/X.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/soundcard.h>

#include "../../libsven/sven.h"
#include "../../libsven/plugin.h"
#include "../../libsven/utils.h"

#define MIXER_DEVICE "/dev/mixer"

static unsigned int setting;
static char *sources[] = {"Volume", "Bass", "Trebl", "Synth", "Pcm", "Spkr", "Line", \
				 "Mic", "CD", "Mix", "Pcm2", "Rec", "IGain", "OGain", \
				 "Line1", "Line2", "Line3", "Digital1", "Digital2", "Digital3", \
				 "PhoneIn", "PhoneOut", "Video", "Radio", "Monitor"};
static gint mutecount[SOUND_MIXER_NRDEVICES]={0};

static int mixer;
static unsigned int j;

/*static int mix_open(char *dev);
static void mix_close(int mix_fd);*/
static int mix_read (int mix_fd,int busy);
static int mix_write (int mix_fd, int busy,int val);

static void command(gpointer sven,gpointer data,char *function,char *title);
static void about(gpointer data);
static GSList *get_function_list(gpointer data);

static SvenPlugin plugin =
{
	about,	/*about*/
	NULL, 	/*configure*/
	NULL,	/*create*/
	command,	/*command*/
	NULL, /*send_event*/
	NULL, /*send_event_key*/
	NULL, 	/*destroy*/
	get_function_list,	/*get_command_name*/
	NULL, /*get menu widget*/
	NULL,	/*priv_data*/
};



static void about(gpointer data)
{
	show_info("\
<b>	 Sven multimedia keyboard deamon</b>\n\
module for control mixer \n\
\n\
Copyright (C) 2005 Eugene Morenko(More)\n\
mailto: morenko@ukr.net\n\
web: http://sven.linux.kiev.ua/\n\
icq:325697438\n");
}

static GSList *get_function_list(gpointer data)
{
	GSList *l = NULL;
	
	if ( (mixer = open(MIXER_DEVICE, O_RDWR)) < 0)
	{
		printf("can't open /dev/mixer");
	}

	ioctl(mixer, MIXER_READ(SOUND_MIXER_DEVMASK), &setting);


	/* Determine supported devices */
	for (j = 0; j < SOUND_MIXER_NRDEVICES; j++)
	{
		if (setting & (1 << j)) /*Device sources[j] is supported.*/
		{
			l= g_slist_append(l,(g_strdup_printf("Mixer-%s Up",sources[j])));
			l= g_slist_append(l,(g_strdup_printf("Mixer-%s Down",sources[j])));
			l= g_slist_append(l,(g_strdup_printf("Mixer-%s Mute",sources[j])));
		}
	}
	
	close(mixer);
	
	return l;
}

static void command(gpointer sdata,gpointer data,char *function,char *title)
{
	Sven *sven = (Sven *)sdata;
	int	set;
	gchar **mix = g_strsplit(function,"-",2);

	if((mix[0]==NULL)&&(mix[1]==NULL))
		return;
	
	if(g_strcasecmp (mix[0],"Mixer")!=0)
		return;
	
	//printf("%s - %s\n",mix[0],mix[1]);
		
	gchar **command = g_strsplit(mix[1]," ",2);
	if((command[0]==NULL)&&(command[1]==NULL))
		return;
	
	//printf("%s - %s\n",command[0],command[1]);
		
	if ( (mixer = open(MIXER_DEVICE, O_RDWR)) < 0)
	{
		printf("can't open /dev/mixer");
	}

	ioctl(mixer, MIXER_READ(SOUND_MIXER_DEVMASK), &setting);
	
	if(g_strcasecmp (command[1],"Up")==0)
	{
		for (j = 0; j < SOUND_MIXER_NRDEVICES; j++)
		{
			if (setting & (1 << j)) /*Device sources[j] is supported.*/
			{
				if(g_strcasecmp (command[0],sources[j])==0)
				{
					set=mix_read(mixer,j);
					
					if((set==0)&&(mutecount[j]>0))
						set=mutecount[j];
					
					set += 2;
					
					if(set>=100)
						set=100;
					
					mutecount[j]=set;
					gtk_osd_print (sven->osd,GTK_OSD_TEXT,sources[j],GTK_OSD_TEXT,":",GTK_OSD_INT,set,GTK_OSD_PERCENTAGE,set,-1);
					mix_write(mixer,j,set);
					break;
				}
			}
		}
	}
	else if(g_strcasecmp (command[1],"Down")==0)
	{
		for (j = 0; j < SOUND_MIXER_NRDEVICES; j++)
		{
			if (setting & (1 << j)) /*Device sources[j] is supported.*/
			{
				if(g_strcasecmp (command[0],sources[j])==0)
				{
					set=mix_read(mixer,j);
					
					if((set==0)&&(mutecount[j]>0))
						set=mutecount[j];
					
					set -= 2;

					if(set<=0)
						set=0;
						
					mutecount[j]=set;
					printf("Volume\n");
					gtk_osd_print (sven->osd,GTK_OSD_TEXT,sources[j],GTK_OSD_TEXT,":",GTK_OSD_INT,set,GTK_OSD_PERCENTAGE,set,-1);
					mix_write(mixer,j,set);
					break;
				}
			}
		}
	}	
	else if(g_strcasecmp (command[1],"Mute")==0)
	{
		for (j = 0; j < SOUND_MIXER_NRDEVICES; j++)
		{
			if (setting & (1 << j)) /*Device sources[j] is supported.*/
			{
				if(g_strcasecmp (command[0],sources[j])==0)
				{
					set=mix_read(mixer,j);
					
					
					if(set==0)
					{
						set=mutecount[j];
						gtk_osd_print (sven->osd,GTK_OSD_TEXT,sources[j],GTK_OSD_TEXT,":",GTK_OSD_INT,set,GTK_OSD_PERCENTAGE,set,-1);
						mix_write(mixer,j,set);
					}
					else	
					{	
						set=0;
						gtk_osd_print (sven->osd,GTK_OSD_TEXT,sources[j],GTK_OSD_TEXT,":",GTK_OSD_INT,set,GTK_OSD_PERCENTAGE,set,-1);
						mix_write(mixer,j,set);
					}

					break;
				}
			}
		}
	}

	close(mixer);
		
	g_strfreev(mix);
	g_strfreev(command);

}

G_MODULE_EXPORT SvenPlugin * init_plugin(void)
{
   return &plugin;
}

G_MODULE_EXPORT char * get_plugin_name(void)
{
   return "Mixer";
}

G_MODULE_EXPORT char * get_plugin_info(void)
{
   return "Module for control mixer";
}



/*
static int mix_open(char *dev)
{
	int mix_fd;

	// No Mixer available, skip 
	if (g_strcasecmp(dev,"NO")==0)
		return 0;

	if ((mix_fd = open(dev, O_RDWR)) < 0) 
	{
		mix_fd=0;
	}
	return mix_fd;
}

static void mix_close(int mix_fd)
{
	if (mix_fd!=0)
		close(mix_fd);
}*/

static int mix_read (int mix_fd,int busy)
{
	int lval;
	if (mix_fd==0)
		return 0;

		/*ioctl(mix_fd,SOUND_MIXER_MUTE, &lval);*/
	if ( -1 == ioctl(mix_fd, MIXER_READ(busy), &lval)) 
	{
		/*exit(1);*/
		printf("error\n");
	}
	
	return (lval & 0x7f);
}

static int mix_write (int mix_fd, int busy,int val)
{
	int lval;

	if (mix_fd==0)
		return 0;

	lval = val;
	lval |= (val << 8 );

	if (-1 == ioctl(mix_fd, MIXER_WRITE(busy), &lval) ) 
	{
		printf("error\n");
	}
	return val;
}

