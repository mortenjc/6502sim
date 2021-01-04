/*
  A simple graphics library for CSE 20211 by Douglas Thain

  This work is licensed under a Creative Commons Attribution 4.0 International License.
  https://creativecommons.org/licenses/by/4.0/

  For complete documentation, see:
  http://www.nd.edu/~dthain/courses/cse20211/fall2013/gfx
  Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
  Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/

/// 2020 12: Additions by Morten Jagd Christensen: Wrapped the C api in a C++
/// class. No functional changes.

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <src/pet/gfx.h>

/*
gfx_open creates several X11 objects, and stores them in globals
for use by the other functions in the library.
*/

/* Open a new graphics window. */
void GFX::gfx_open( int width, int height, const char *title )
{
	gfx_display = XOpenDisplay(0);
	if(!gfx_display) {
		fprintf(stderr,"gfx_open: unable to open the graphics window.\n");
		fprintf(stderr,"gfx_open: try 'export DISPLAY=:0' and retry\n");
		return;
	}

	Visual *visual = DefaultVisual(gfx_display,0);
	// if(visual && (visual->class)==TrueColor) {
		gfx_fast_color_mode = 1;
	// } else {
		// gfx_fast_color_mode = 0;
	// }

	int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
	int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));

	gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display), 0, 0, width, height, 0, blackColor, blackColor);

	XSetWindowAttributes attr;
	attr.backing_store = Always;

	XChangeWindowAttributes(gfx_display,gfx_window,CWBackingStore,&attr);

	XStoreName(gfx_display,gfx_window,title);

	XSelectInput(gfx_display, gfx_window, StructureNotifyMask|KeyPressMask|ButtonPressMask);

	XMapWindow(gfx_display,gfx_window);

	gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);

	gfx_colormap = DefaultColormap(gfx_display,0);

	XSetForeground(gfx_display, gfx_gc, whiteColor);

	// Wait for the MapNotify event

	for(;;) {
		XEvent e;
		XNextEvent(gfx_display, &e);
		if (e.type == MapNotify)
			break;
	}

	initialized = true;
}

/// Draw a single point at (x,y)
void GFX::gfx_point( int x, int y )
{
	if (not initialized)
		return;
	XDrawPoint(gfx_display,gfx_window,gfx_gc,x,y);
}

/// Draw a line from (x1,y1) to (x2,y2)
void GFX::gfx_line( int x1, int y1, int x2, int y2 )
{
	if (not initialized)
		return;

	XDrawLine(gfx_display,gfx_window,gfx_gc,x1,y1,x2,y2);
}

/// Change the current drawing color.
void GFX::gfx_color( int r, int g, int b )
{
	if (not initialized)
	  return;

	XColor color;

	if(gfx_fast_color_mode) {
		/* If this is a truecolor display, we can just pick the color directly. */
		color.pixel = ((b&0xff) | ((g&0xff)<<8) | ((r&0xff)<<16) );
	} else {
		/* Otherwise, we have to allocate it from the colormap of the display. */
		color.pixel = 0;
		color.red = r<<8;
		color.green = g<<8;
		color.blue = b<<8;
		XAllocColor(gfx_display,gfx_colormap,&color);
	}

	XSetForeground(gfx_display, gfx_gc, color.pixel);
}

/// Clear the graphics window to the background color.
void GFX::gfx_clear()
{
	if (not initialized)
		return;
	XClearWindow(gfx_display,gfx_window);
}

/* Change the current background color. */

void GFX::gfx_clear_color( int r, int g, int b )
{
	if (not initialized)
		return;
	XColor color;
	color.pixel = 0;
	color.red = r<<8;
	color.green = g<<8;
	color.blue = b<<8;
	XAllocColor(gfx_display,gfx_colormap,&color);

	XSetWindowAttributes attr;
	attr.background_pixel = color.pixel;
	XChangeWindowAttributes(gfx_display,gfx_window,CWBackPixel,&attr);
}

int GFX::gfx_event_waiting()
{
	if (not initialized)
		return 0;

	XEvent event;

	gfx_flush();

	while (1) {
		if(XCheckMaskEvent(gfx_display,-1,&event)) {
			if(event.type==KeyPress) {
				XPutBackEvent(gfx_display,&event);
				return 1;
			} else if (event.type==ButtonPress) {
				XPutBackEvent(gfx_display,&event);
				return 1;
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}
}

/* Wait for the user to press a key or mouse button. */

char GFX::gfx_wait()
{
	if (not initialized)
		return ' ';
	XEvent event;

	gfx_flush();

	while(1) {
		XNextEvent(gfx_display,&event);

		if(event.type==KeyPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;
			return XLookupKeysym(&event.xkey,0);
		} else if(event.type==ButtonPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;
			return event.xbutton.button;
		}
	}
}

/* Return the X and Y coordinates of the last event. */

int GFX::gfx_xpos()
{
	return saved_xpos;
}

int GFX::gfx_ypos()
{
	return saved_ypos;
}

/* Flush all previous output to the window. */

void GFX::gfx_flush()
{
	if (not initialized)
		return;
	XFlush(gfx_display);
}
