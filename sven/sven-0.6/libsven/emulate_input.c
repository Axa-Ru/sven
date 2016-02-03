/*---------------------------------------------------------------------------------
Name               : handle_key.c
Author             : Marvin Raaijmakers
Description        : Handles key press events
Date of last change: 20-Aug-2005

    Copyright (C) 2005 Marvin Raaijmakers

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    You can contact me at: marvinr(at)users(dot)sf(dot)net
    (replace (at) by @ and (dot) by .)
-----------------------------------------------------------------------------------*/
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

static void emulate_press_modifiers (	unsigned int	state,
					Window		window,
					Display		*display  );
static void emulate_release_modifiers (	unsigned int	state,
					Window		window,
					Display		*display  );


char *get_window_name (Display *display,Window	window)
/*
Input:
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	The name of the currently focussed window. If no window is focussed NULL is
	returned. If the focussed window has no name it returns the first ancestor
	of it that has a name. If there is no such ancestor NULL is returned.
Description:
	This function returns the name of the currently focussed window.
*/
{
	unsigned int	num_children;
	char	*window_name;
	Window	parent, root, *children;
	
	/* If there is no focused window */
	if (window == 0)
	{
		return (NULL);
	}
	parent = window;
	/* Find the first ancestor with a window name */
	while (!XFetchName(display, window, &window_name) && parent != DefaultRootWindow(display))
	{
		window = parent;
		XQueryTree (display, window, &root, &parent, &children, &num_children);
		XFree (children);
	}
	
	return (window_name);
}

char *get_focused_window_name (Display *display)
/*
Input:
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	The name of the currently focussed window. If no window is focussed NULL is
	returned. If the focussed window has no name it returns the first ancestor
	of it that has a name. If there is no such ancestor NULL is returned.
Description:
	This function returns the name of the currently focussed window.
*/
{
	unsigned int	num_children;
	int	revert_to;
	Window	window,
		parent,
		root,
		*children;
	char	*window_name;
	
	XGetInputFocus (display, &window, &revert_to);
	/* If there is no focused window */
	if (window == 0)
	{
		return (NULL);
	}
	parent = window;
	/* Find the first ancestor with a window name */
	while (!XFetchName(display, window, &window_name) && parent != DefaultRootWindow(display))
	{
		window = parent;
		XQueryTree (display, window, &root, &parent, &children, &num_children);
		XFree (children);
	}
	
	return (window_name);
}


void send_key_event (	Display		*display,
			Window		window,
			unsigned int	keysym,
			unsigned int	state,
			int		type      )
/*
Input:
	display	- Specifies the connection to the X server
	window	- Specifies the window the window the event is to be sent to
	keysym	- The string of the keysym of the key to send
	state	- The state of the key event
	type	- KeyPress or KeyRelease
Output:
	-
Returns:
	-
Description:
	This function sends a key press or release event to window.
*/
{
	long      event_mask;
	XKeyEvent key_event;
	
	if (type == KeyPress)
	{
		event_mask = KeyPressMask;
	}
	else
	{
		event_mask = KeyReleaseMask;
	}
	key_event.type = type;
	key_event.display = display;
	key_event.window = window;
	key_event.root = DefaultRootWindow(display);
	key_event.state = state;
	key_event.keycode = keysym;
	
	XSendEvent (display, window, False, event_mask, (XEvent *) &key_event);
	usleep (80000); /* This is a value where mozilla doesn't ignore the event */
}


void emulate_release_modifiers (	unsigned int	state,
				Window		window,
				Display		*display  )
/*
Input:
	state	- The bitmask indication which modifiers are pressed
	window	- The window to send the key release event(s) to
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	-
Description:
	This function sends for every modifier whos bit is set in state a
	KeyRelease event. It only does this for the following modifiers:
	- Shift
	- Control
	- Mod1
*/
{
	unsigned int tmp_state;
	
	tmp_state = state;
	if (state & ControlMask)
	{
		send_key_event (display, window,XKeysymToKeycode(display, XStringToKeysym( "Control_L")), tmp_state, KeyPress);
		tmp_state ^= ControlMask;
	}
	if (state & ShiftMask)
	{
		send_key_event (display, window,XKeysymToKeycode(display, XStringToKeysym( "Shift_L")), tmp_state, KeyPress);
		tmp_state ^= ShiftMask;
	}
	if (state & Mod1Mask)
	{
		send_key_event (display, window, XKeysymToKeycode(display, XStringToKeysym("Alt_L")), tmp_state, KeyPress);
		tmp_state ^= Mod1Mask;
	}
}


void emulate_press_modifiers (	unsigned int	state,
				Window		window,
				Display		*display  )
/*
Input:
	state	- The bitmask indication which modifiers are pressed
	window	- The window to send the key press event(s) to
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	-
Description:
	This function sends for every modifier whos bit is set in state a
	KeyPress event. It only does this for the following modifiers:
	- Shift
	- Control
	- Mod1
*/
{
	unsigned int tmp_state;
	
	tmp_state = 0;
	if (state & ControlMask)
	{
		send_key_event (display, window, XKeysymToKeycode(display, XStringToKeysym("Control_L")), tmp_state, KeyPress);
		tmp_state |= ControlMask;
	}
	if (state & ShiftMask)
	{
		send_key_event (display, window, XKeysymToKeycode(display, XStringToKeysym("Shift_L")), tmp_state, KeyPress);
		tmp_state |= ShiftMask;
	}
	if (state & Mod1Mask)
	{
		send_key_event (display, window, XKeysymToKeycode(display, XStringToKeysym("Alt_L")), tmp_state, KeyPress);
		tmp_state |= Mod1Mask;
	}
}


void emulate_key (unsigned int keysym,unsigned int state,Display *display  )
/*
Input:
	key	- Information about the key events to emulate
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	-
Description:
	This function sends key events to the window that has input focus on
	display. First this function sends for every modifier whos bit is set in
	key->state a KeyPress event. Then it sends a KeyPress followed by a
	KeyRelease event for the key that has the keysym key->keysym. Then it sends
	for every modifier whos bit is set in key->state a KeyRelease event.
*/
{
	Window		focus_window;
	int		revert_to;
	
	XGetInputFocus (display, &focus_window, &revert_to);
	
	emulate_press_modifiers (state, focus_window, display);
	send_key_event (display, focus_window,keysym, state, KeyPress);
	send_key_event (display, focus_window,keysym, state, KeyRelease);
	emulate_release_modifiers (state, focus_window, display);
	XSync (display, False);
}

void emulate_key_to_wind (Window window,unsigned int keysym, unsigned int	state,Display	*display  )
/*
Input:
	key	- Information about the key events to emulate
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	-
Description:
	This function sends key events to the root window that has input focus on
	display. First this function sends for every modifier whos bit is set in
	key->state a KeyPress event. Then it sends a KeyPress followed by a
	KeyRelease event for the key that has the keysym key->keysym. Then it sends
	for every modifier whos bit is set in key->state a KeyRelease event.
*/
{

	
	emulate_press_modifiers (state,window, display);
	send_key_event (display,window, keysym, state, KeyPress);
	send_key_event (display,window, keysym, state, KeyRelease);
	emulate_release_modifiers (state,window, display);
	XSync (display, False);
}

void emulate_key_to_root_wind (unsigned int keysym, unsigned int	state,Display	*display  )
/*
Input:
	key	- Information about the key events to emulate
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	-
Description:
	This function sends key events to the root window that has input focus on
	display. First this function sends for every modifier whos bit is set in
	key->state a KeyPress event. Then it sends a KeyPress followed by a
	KeyRelease event for the key that has the keysym key->keysym. Then it sends
	for every modifier whos bit is set in key->state a KeyRelease event.
*/
{

	
	emulate_press_modifiers (state, DefaultRootWindow(display), display);
	send_key_event (display, DefaultRootWindow(display), keysym, state, KeyPress);
	send_key_event (display, DefaultRootWindow(display), keysym, state, KeyRelease);
	emulate_release_modifiers (state, DefaultRootWindow(display), display);
	XSync (display, False);
}

void emulate_button (unsigned int button)
/*
Input:
	button	- The button event to send
Description:
	This function sends a button press and release event to the the default X
	display.
*/
{
	Display	*display;
	int	i;
	
	/* Connect to the server specified in the DISPLAY evironment variable */
	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		printf ( "Cannot connect to X server %s.\n", XDisplayName(NULL));
		return;
	}
	if (!XQueryExtension (display, "XTEST", &i, &i, &i))
	{
		printf ( "Extension XTest unavailable on display '%s'.\n", XDisplayName(NULL));
		XCloseDisplay (display);
		return;
	}
	/* Send the button press event */
	XTestFakeButtonEvent (display, button, True, CurrentTime);
	/* Send the button release event */
        XTestFakeButtonEvent (display, button, False, CurrentTime);
	XCloseDisplay (display);
}
