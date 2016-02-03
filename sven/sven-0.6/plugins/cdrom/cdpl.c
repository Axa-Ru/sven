#include <X11/X.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cdpl.h"
#include "../../libsven/sven.h"

int cdrom;

int cd_doing() {            
	struct cdrom_subchnl subchnl;
	subchnl.cdsc_format = CDROM_MSF;
	ioctl (cdrom, CDROMSUBCHNL, &subchnl);
	return subchnl.cdsc_audiostatus;
}

u_char current_track() {
	struct cdrom_subchnl subchnl;
	subchnl.cdsc_format = CDROM_MSF;
	ioctl(cdrom, CDROMSUBCHNL, &subchnl);
	if (cd_doing() == FALSE) subchnl.cdsc_trk = 0;
	return subchnl.cdsc_trk;
}

void cd_stop(void) 
{
	int ret;
	ret = ioctl(cdrom, CDROMSTOP);
	if(ret == -1)
		printf(("can't stop the cd/dvd drive"));
	close(cdrom);
}

int cd_prev(void)
{
	int track=0;
	int ret;
	
	ioctl(cdrom,CDROMREADTOCHDR,(void *) &header); // Get start and end tracks

	index1.cdti_trk0=header.cdth_trk0;   // Set first track
	index1.cdti_ind0=0;                  // Start of track
	
	index1.cdti_trk1=header.cdth_trk1;   // Set final track
	index1.cdti_ind1=255;                // End of track
	
	index1.cdti_trk0 = index1.cdti_trk0 - 1;
	track=(int) current_track()-1;
	if (track < 1) 
		track = 1;
	index1.cdti_trk0 = track;
	
	ret = ioctl(cdrom,CDROMPLAYTRKIND,(void *) &index1);
	close(cdrom);
	if(ret == -1)
	{
		printf(_("can't prev track the cd/dvd drive"));
		return 0;
	}

	return track;
}

int cd_next(void)
{
	int track=0;
	int ret;

	ioctl(cdrom,CDROMREADTOCHDR,(void *) &header); // Get start and end tracks

	index1.cdti_trk0=header.cdth_trk0;   // Set first track
	index1.cdti_ind0=0;                  // Start of track
	
	index1.cdti_trk1=header.cdth_trk1;   // Set final track
	index1.cdti_ind1=255;                // End of track
	
	index1.cdti_trk0 = index1.cdti_trk0;
	track=(int) current_track()+1;
	if (track > index1.cdti_trk1) 
		track = index1.cdti_trk1;
	index1.cdti_trk0 = track;
 
	ret = ioctl(cdrom,CDROMPLAYTRKIND,(void *) &index1);
	close(cdrom);
	if(ret == -1)
	{
		printf(_("can't next track the cd/dvd drive"));
		return 0;
	}

	return track;
}

int cd_resume(void) 
{
	int ret;
	
	ret = ioctl(cdrom, CDROMRESUME);
	close(cdrom);
	
	if(ret == -1) 
	{
		if(errno == EIO)
			printf(_("Try pausing a CD before you try this option."));
		
		printf(_("can't resume playing cd/dvd drive"));
		return 0;
	}
	
	return 1;
}

int cd_pause(void) 
{
	int ret;
	
	ret = ioctl(cdrom, CDROMPAUSE);
	if(ret == -1) 
	{
		printf(_("can't pause cd/dvd drive"));
		close(cdrom);
		return 0;
	}
	ret=(int)current_track();
	close(cdrom);
	
	return ret;
}

int cd_play(void)
{
	int ret;
	
	if(cd_doing()==CD_STOP_INFO)
	{
		ioctl(cdrom,CDROMREADTOCHDR,(void *) &header); // Get start and end tracks

		index1.cdti_trk0=header.cdth_trk0;   // Set first track
		index1.cdti_ind0=0;                  // Start of track
	
		index1.cdti_trk1=header.cdth_trk1;   // Set final track
		index1.cdti_ind1=255;                // End of track
	
		index1.cdti_trk0 = index1.cdti_trk0;
		index1.cdti_trk0 = 1;
	 
		ret = ioctl(cdrom,CDROMPLAYTRKIND,(void *) &index1);
		if(ret == -1)
		{
			printf(_("can't playing the cd/dvd drive"));
			close(cdrom);
			return 0;
		}
		
		ret=(int)current_track();
		close(cdrom);
	
		return ret;
	}
	if(cd_doing()==CD_PAUSE_INFO)
	{
		ret = ioctl(cdrom, CDROMRESUME, 0);
		if(ret == -1) 
		{
			if(errno == EIO)
				printf(_("Try pausing a CD before you try this option."));
		
			printf(_("can't resume playing cd/dvd drive"));
			close(cdrom);
			return 0;
		}
		
		ret=(int)current_track();
		close(cdrom);
	
		return ret;
	}
	else if(cd_doing()==CD_PLAY_INFO)
	{
		ret = ioctl(cdrom, CDROMPAUSE);
		if(ret == -1) 
		{
			printf(_("can't pause cd/dvd drive"));
			close(cdrom);
			return 0;
		}
	
		ret=(int)current_track();
		close(cdrom);
	
		return ret;
	}
	
	return 0;
}

int cd_eject(void) 
{
printf("CD Status:%d\n",ioctl (cdrom, CDROM_DRIVE_STATUS, 0));
	if(ioctl (cdrom, CDROM_DRIVE_STATUS, 0)==CD_CLOSE_TRAY)
	{
		if(ioctl(cdrom,CDROMEJECT,0))
 		{
			printf(_("No disc in drive\n "));
			close(cdrom);
		}
		ioctl(cdrom, CDROMRESET);
		close(cdrom);
		return 0;
	}
	if(ioctl (cdrom, CDROM_DRIVE_STATUS, 0)==CD_OPEN_TRAY)
	{
		ioctl(cdrom, CDROMCLOSETRAY,0);
		close(cdrom);
		return 1;
	}
	if(ioctl (cdrom, CDROM_DRIVE_STATUS, 0)==-1)
	{
		ioctl(cdrom, CDROMCLOSETRAY,0);
		close(cdrom);
		return 1;
	}
	if(ioctl (cdrom, CDROM_DRIVE_STATUS, 0)==1)
	{
		if(ioctl(cdrom,CDROMEJECT,0))
 		{
			printf(_("No disc in drive\n "));
		}
		ioctl(cdrom, CDROMRESET);
		close(cdrom);
		return 0;
	}

	return -1;
}

void cd_start(gpointer data)
{
	Sven *sven = (Sven *)data;
	char *dev;
	if(sven==NULL)
		return;

	if(cdrom>0)
	{
		ioctl(cdrom, CDROMRESET);
		close(cdrom);
	}

	sven_cfg_read_string(sven->config->all,"cdrom","cdrom",&dev);
	if(dev!=NULL)
	{
		
		cdrom = open(dev,O_RDONLY | O_NONBLOCK);
	}
	else
		cdrom = open(CDDEVICE,O_RDONLY | O_NONBLOCK);
	
	free(dev);
	if(cdrom < 0)
	{
		printf(_("can't open cd/dvd drive\n "));
		printf("Error code: %d\n",errno);
		perror("Unable to create file\n");
		return;
	}
	ioctl(cdrom, CDROMRESET);
}

void select_cd(gpointer data,char *command,char *text)
{
	Sven *sven = (Sven *)data;

	if(g_strcasecmp (command,"PLAY")==0)
	{
		DEBUG_MSG("Play:%s\n",text);
		cd_start(sven);
		
		gchar **text_cd = g_strsplit(text,"/",3);
		
		if(cd_doing()==CD_STOP_INFO)
		{
			DEBUG_MSG("CD_STOP_INFO\n");
			if(g_strcasecmp(text,"")!=0)
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_(text_cd[0]),cd_play()),-1);
			}
			else
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_("CD Play"),cd_play()),-1);
			}
		}
		else if(cd_doing()==CD_PLAY_INFO)
		{
			DEBUG_MSG("CD_PLAY_INFO\n");
			if(g_strcasecmp(text,"")!=0)
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_(text_cd[1]),cd_play()),-1);
			}
			else
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_("CD Pause "),cd_play()),-1);
			}
		}
		else if(cd_doing()==CD_PAUSE_INFO)
		{
			DEBUG_MSG("CD_PAUSE_INFO\n");
			if(g_strcasecmp(text,"")!=0)
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_(text_cd[2]),cd_play()),-1);
			}
			else
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_("CD Resume play"),cd_play()),-1);
			}
		}
		else
		{
			if(g_strcasecmp(text,"")!=0)
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_(text),cd_play()),-1);
			}
			else
			{	
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_(command),cd_play()),-1);
			}
		}
		g_strfreev(text_cd);
	}
	else if(g_strcasecmp (command,"PAUSE")==0)
	{
		DEBUG_MSG("PAUSE\n");
		cd_start(sven);
		
		if(g_strcasecmp(text,"")!=0)
		{
			gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_(text),cd_pause()),-1);
		}
		else
		{
			gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_("CD Pause "),cd_pause()),-1);
		}
	}
	else if(g_strcasecmp (command,"NEXT")==0)
	{
		DEBUG_MSG("NEXT\n");
		cd_start(sven);
		
		if(g_strcasecmp(text,"")!=0)
		{
			gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_(text),cd_next()),-1);
		}
		else
		{
			gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_("CD Next "),cd_next()),-1);
		}
	}
	else if(g_strcasecmp (command,"PREV")==0)
	{
		DEBUG_MSG("PREV\n");
		cd_start(sven);
		
		if(g_strcasecmp(text,"")!=0)
		{
			gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_(text),cd_prev()),-1);
		}
		else
		{
			gtk_osd_print (sven->osd,GTK_OSD_TEXT,g_strdup_printf("%s:%d",_("CD Prev "),cd_prev()),-1);
		}
	}
	else if(g_strcasecmp (command,"STOP")==0)
	{
		DEBUG_MSG("STOP\n");
		cd_start(sven);
		if(g_strcasecmp(text,"")!=0)
		{
			gtk_osd_print (sven->osd,GTK_OSD_TEXT,_(text),-1);
		}
		else
		{
			gtk_osd_print (sven->osd,GTK_OSD_TEXT,_("CD Stop"),-1);
		}
		cd_stop();
	}
	else if(g_strcasecmp (command,"EJECT")==0)
	{
		DEBUG_MSG("EJECT\n");
		cd_start(sven);
		
		gchar **text_cd = g_strsplit(text,"/",2);
		
		if(ioctl (cdrom, CDROM_DRIVE_STATUS, 0)==CD_CLOSE_TRAY)
		{
			if(g_strcasecmp(text,"")!=0)
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,_(text_cd[0]),-1);
			}
			else
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,_("Open Cd-Rom"),-1);
			}
		}
		else if(ioctl (cdrom, CDROM_DRIVE_STATUS, 0)==CD_OPEN_TRAY)
		{
			if(g_strcasecmp(text,"")!=0)
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,_(text_cd[1]),-1);
			}
			else
			{
				gtk_osd_print (sven->osd,GTK_OSD_TEXT,_("Close Cd-Rom"),-1);
			}
		}
/*		else if(ioctl (cdrom, CDROM_DRIVE_STATUS, 0)==-1)
		{
			if(g_strcasecmp(text,"")!=0)
			{
				gtk_osd_display (sven->osd,_(text_cd[1]));
				gtk_osd_display (sven->osd,1, GTK_OSD_string, "");
			}
			else
			{
				gtk_osd_display (sven->osd,_("Close Cd-Rom"));
				gtk_osd_display (sven->osd,1, GTK_OSD_string, "");
			}
		}*/
		else
		{
			if(g_strcasecmp(text,"")!=0)
			{
				gtk_osd_print (sven->osd,_(text),-1);
			}
			else
			{
				gtk_osd_print (sven->osd,_(command),-1);
			}
		}
		
		cd_eject();
		g_strfreev(text_cd);
	}
	else
	{
		printf("Not find CD:%s\n",command);
	}
}
	
