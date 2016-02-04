/* Sven multimedia keyboard deamon
 * mixer.h
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
#ifndef __MIXER_H_
#define __MIXER_H_

#include <sys/soundcard.h>

#define DEFMIXER "/dev/mixer"

void select_mixer(Sven *sven,char *command,char *title);

void set_mixer_mute(Sven *sven,char *text);
void set_mixer_up(Sven *sven,int busy,char *text);
void set_mixer_down(Sven *sven,int busy,char *text);

void set_volume(Sven *sven,int value,int busy);
int get_volume(Sven *sven,int busy);

#endif /* __MIXER_H_ */


