/* Sven multimedia keyboard deamon
 * cdpl.h
 *CD player function
 *
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
#ifndef __CDPL_H_
#define __CDPL_H_

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/cdrom.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define CDDEVICE "/dev/cdrom"
#define CD_OPEN_TRAY 2
#define CD_CLOSE_TRAY 4
#define CD_PLAY_INFO 17
#define CD_PAUSE_INFO 18
#define CD_STOP_INFO 21


struct cdrom_ti  index1;
struct cdrom_tochdr header;

void select_cd(Sven *sven,char *command,char *text);
void cd_start(Sven *sven);
int cd_play(void);
int cd_pause(void);
void cd_stop(void);
int cd_next(void);
int cd_prev(void);
int cd_eject(Sven *sven);
int cd_resume(void);

#endif /* __CDPL_H_ */

