/* Sven multimedia keyboard deamon
 * emulate_input.h
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
#ifndef __EMULATE_INPUT_H_
#define __EMULATE_INPUT_H_

char *get_window_name (Display *display,Window	window);
char *get_focused_window_name (Display *display);
void send_key_event (Display *display,Window  window,unsigned int keysym,unsigned int state,int type);
void emulate_key (unsigned int keysym,unsigned int state,Display *display);
void emulate_key_to_wind (Window window,unsigned int keysym, unsigned int	state,Display	*display  );
void emulate_key_to_root_wind (unsigned int keysym, unsigned int state,Display *display  );
void emulate_button (unsigned int button);

#endif /* __EMULATE_INPUT_H_ */
