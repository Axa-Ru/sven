/* Sven multimedia keyboard deamon
 * about.h
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: more@irpin.com
 * web: http://sven.linux.kiev.ua/
 * icq:325697438
 *
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
#ifndef __ABOUT_H_
#define __ABOUT_H_

#define INFO  _("Sven it is the good program for multimedia buttons. \nYou can adjust it for any keyboard.\n\
	It is possible to adjust for performance \nof different programs for each button.\n")
	
#define AUTHORS  _("Project leader:\n  Eugene Morenko(More) \n <more@irpin.com>")

#define TRANSLATORS  _("Current translators:\n\nRussian - Eugene Morenko(More)\n\
   <more@irpin.com>\n\n")
   
#define THANKS  _("Vladimir Lettiev aka crux\n\
   <crux@syktsu.ru>\n\
   Misc Bugfixes\n")

void create_about_gui(Sven *sven,GtkWidget *vbox1);

#endif /* __ABOUT_H_ */
