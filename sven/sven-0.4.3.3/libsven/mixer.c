/* Sven multimedia keyboard deamon
 * mixer.c
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
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#include "sven.h"
#include "mixer.h"


static int mix_open(char *dev);
static void mix_close(int mix_fd);
static int mix_read (int mix_fd,int busy);
static int mix_write (int mix_fd, int val,int busy);

static gint volmute=0;
static gint mutecount=0;

void select_mixer(Sven *sven,char *command,char *title)
{
	gchar **mixer = g_strsplit(command," ",2);
	if(g_strcasecmp (mixer[1],"Up")==0)
	{
		if(g_strcasecmp (mixer[0],"VOLUME")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_up(sven,SOUND_MIXER_VOLUME,title);
			else
				set_mixer_up(sven,SOUND_MIXER_VOLUME,command);
		}
		else if(g_strcasecmp (mixer[0],"BASS")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_up(sven,SOUND_MIXER_BASS,title);
			else
				set_mixer_up(sven,SOUND_MIXER_BASS,command);
		}
		else if(g_strcasecmp (mixer[0],"TREBLE")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_up(sven,SOUND_MIXER_TREBLE,title);
			else
				set_mixer_up(sven,SOUND_MIXER_TREBLE,command);
		}
		else if(g_strcasecmp (mixer[0],"PCM")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_up(sven,SOUND_MIXER_PCM,title);
			else
				set_mixer_up(sven,SOUND_MIXER_PCM,command);
		}
		else if(g_strcasecmp (mixer[0],"SPEAKER")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_up(sven,SOUND_MIXER_SPEAKER,title);
			else
				set_mixer_up(sven,SOUND_MIXER_SPEAKER,command);
		}
		else if(g_strcasecmp (mixer[0],"LINE")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_up(sven,SOUND_MIXER_LINE,title);
			else
				set_mixer_up(sven,SOUND_MIXER_LINE,command);
		}
		else if(g_strcasecmp (mixer[0],"MIC")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_up(sven,SOUND_MIXER_MIC,title);
			else
				set_mixer_up(sven,SOUND_MIXER_MIC,command);
		}
		else if(g_strcasecmp (mixer[0],"CD")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_up(sven,SOUND_MIXER_CD,title);
			else
				set_mixer_up(sven,SOUND_MIXER_CD,command);
		}
		else
		{
			printf("Not find mixer:%s\n",mixer[0]);
		}
	}
	else if (g_strcasecmp (mixer[1],"Down")==0)
	{
		if(g_strcasecmp (mixer[0],"VOLUME")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_down(sven,SOUND_MIXER_VOLUME,title);
			else
				set_mixer_down(sven,SOUND_MIXER_VOLUME,command);
		}
		else if(g_strcasecmp (mixer[0],"BASS")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_down(sven,SOUND_MIXER_BASS,title);
			else
				set_mixer_down(sven,SOUND_MIXER_BASS,command);
		}
		else if(g_strcasecmp (mixer[0],"TREBLE")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_down(sven,SOUND_MIXER_TREBLE,title);
			else
				set_mixer_down(sven,SOUND_MIXER_TREBLE,command);
		}
		else if(g_strcasecmp (mixer[0],"PCM")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_down(sven,SOUND_MIXER_PCM,title);
			else
				set_mixer_down(sven,SOUND_MIXER_PCM,command);
		}
		else if(g_strcasecmp (mixer[0],"SPEAKER")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_down(sven,SOUND_MIXER_SPEAKER,title);
			else
				set_mixer_down(sven,SOUND_MIXER_SPEAKER,command);
		}
		else if(g_strcasecmp (mixer[0],"LINE")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_down(sven,SOUND_MIXER_LINE,title);
			else
				set_mixer_down(sven,SOUND_MIXER_LINE,command);
		}
		else if(g_strcasecmp (mixer[0],"MIC")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_down(sven,SOUND_MIXER_MIC,title);
			else
				set_mixer_down(sven,SOUND_MIXER_MIC,command);
		}
		else if(g_strcasecmp (mixer[0],"CD")==0)
		{
			if(g_strcasecmp(title,"")!=0)
				set_mixer_down(sven,SOUND_MIXER_CD,title);
			else
				set_mixer_down(sven,SOUND_MIXER_CD,command);
		}
		else
		{
			printf("Not find mixer:%s\n",mixer[0]);
		}
	}
	else if (g_strcasecmp (mixer[1],"Mute")==0)
	{
		if(g_strcasecmp(title,"")!=0)
				set_mixer_mute(sven,title);
			else
				set_mixer_mute(sven,mixer[1]);
	}
	else
	{
		printf("Not find mixer:%s\n",command);
	}
}

void set_mixer_mute(Sven *sven,char *text)
{
	if(mutecount==0)
	{
		volmute=get_volume(sven,0);
		mutecount=1;
	}
	else
		mutecount=0;

	if(mutecount==1)
	{
		xosd_display (sven->osd->osd,0, XOSD_string,_(text));
		xosd_display (sven->osd->osd,1, XOSD_percentage,0);
		set_volume(sven,0,0);
		sven->vol_timer=0;
	}
	else
	{
		xosd_display (sven->osd->osd,0, XOSD_string, g_strdup_printf (_("Unmute:%d"),volmute));
		xosd_display (sven->osd->osd,1, XOSD_percentage,volmute);
		set_volume(sven,volmute,0);
		sven->vol_timer=volmute;
	}
}

void set_mixer_up(Sven *sven,int busy,char *text)
{
	int mix=0;
	
	if(mutecount==0 && busy==0)
	{
		sven->vol_timer=(get_volume(sven,busy)+2);
		if(sven->vol_timer>=100)
			sven->vol_timer=100;
		mix=sven->vol_timer;
	}
	else if(busy==0)
	{
		sven->vol_timer=sven->vol_timer+2;
		mix=sven->vol_timer;
		mutecount=0;
	}
	else 
	{
	}
	
	if(busy!=0)
		mix = (get_volume(sven,busy)+2);
		if(mix>=100)
			mix=100;
	xosd_display (sven->osd->osd,0, XOSD_string,g_strdup_printf ("%s:%d",text,mix));
	xosd_display (sven->osd->osd,1, XOSD_percentage,mix);
	set_volume(sven,mix,busy);
}
void set_mixer_down(Sven *sven,int busy,char *text)
{
	int mix=0;
	
	if(mutecount==0 && busy==0)
	{
		sven->vol_timer=(get_volume(sven,busy)-2);
		if(sven->vol_timer<=0)
			sven->vol_timer=0;
		mix=sven->vol_timer;
	}
	else if(busy==0)
	{
		sven->vol_timer=sven->vol_timer-2;
		mix=sven->vol_timer;
		mutecount=0;
	}
	else
	{
	}
	
	if(busy!=0)
		mix = (get_volume(sven,busy)-2);
		if(mix<=0)
			mix=0;
		
	xosd_display (sven->osd->osd,0, XOSD_string,g_strdup_printf ("%s:%d",text,mix));
	xosd_display (sven->osd->osd,1, XOSD_percentage,mix);
	set_volume(sven,mix,busy);
}


void set_volume(Sven *sven,int value,int busy)
{
	int mix_fd=0;
	 if(sven->mix->mixer!=NULL)
  		mix_fd = mix_open(sven->mix->mixer);
  	else
  		mix_fd = mix_open(DEFMIXER);
	if (mix_fd!=0) {
		mix_write(mix_fd, value,busy);
		mix_close(mix_fd);
	}
}

/* Get Volume */
int get_volume(Sven *sven,int busy)
{
	int vlm=0;
	int mix_fd=0;
	 if(sven->mix->mixer!=NULL)
  		mix_fd = mix_open(sven->mix->mixer);
  	else
  		mix_fd = mix_open(DEFMIXER);
	/*printf("mix_fd:%d",mix_fd);*/
	if (mix_fd!=0) 
	{
		vlm=mix_read(mix_fd,busy);
		mix_close(mix_fd);
	}
	return (vlm & 0x7f);
}

static int mix_open(char *dev)
{
	int mix_fd;

	/* No Mixer available, skip */
	if (strcmp(dev,"NO")==0)
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
}

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
	
	return (lval);
}

static int mix_write (int mix_fd, int val,int busy)
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
