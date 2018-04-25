/*
 * This file is part of the UCB release of Plan 9. It is subject to the license
 * terms in the LICENSE file found in the top-level directory of this
 * distribution and at http://akaros.cs.berkeley.edu/files/Plan9License. No
 * part of the UCB release of Plan 9, including this file, may be copied,
 * modified, propagated, or distributed except according to the terms contained
 * in the LICENSE file.
 */

#include <u.h>
#include <libc.h>
#include <draw.h>
#include <thread.h>
#include <cursor.h>
#include <mouse.h>
#include <keyboard.h>
#include <frame.h>
#include <fcall.h>
#include <jay.h>
#include "dat.h"
#include "fns.h"

Cursor crosscursor = {
	{-7, -7},
	{0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
	 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x7F, 0xFE,
	 0x7F, 0xFE, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
	 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, },
	{0x01, 0x80, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40,
	 0x02, 0x40, 0x02, 0x40, 0x7E, 0x7E, 0x80, 0x01,
	 0x80, 0x01, 0x7E, 0x7E, 0x02, 0x40, 0x02, 0x40,
	 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x01, 0x80, },
};

Cursor boxcursor = {
	{-7, -7},
	{0x00, 0x00, 0x01, 0x80, 0x03, 0xC0, 0x01, 0x80,
	 0x01, 0x80, 0x01, 0x80, 0x21, 0x84, 0x7E, 0x7E,
	 0x7E, 0x7E, 0x21, 0x84, 0x01, 0x80, 0x01, 0x80,
	 0x01, 0x80, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00, },
	{0x01, 0x80, 0x02, 0x40, 0x04, 0x20, 0x06, 0x60,
	 0x02, 0x40, 0x32, 0x4C, 0x5E, 0x7A, 0x80, 0x01,
	 0x80, 0x01, 0x5E, 0x7A, 0x32, 0x4C, 0x02, 0x40,
	 0x06, 0x60, 0x04, 0x20, 0x02, 0x40, 0x01, 0x80,}
};

Cursor sightcursor = {
	{-7, -7},
	{0x00, 0x00, 0x78, 0x1E, 0x40, 0x02, 0x40, 0x02,
	 0x41, 0x82, 0x01, 0x80, 0x01, 0x80, 0x0F, 0xF0,
	 0x0F, 0xF0, 0x01, 0x80, 0x01, 0x80, 0x41, 0x82,
	 0x40, 0x02, 0x40, 0x02, 0x78, 0x1E, 0x00, 0x00, },
	{0x78, 0x1E, 0x84, 0x21, 0xB8, 0x1D, 0xA1, 0x85,
	 0xA2, 0x45, 0x42, 0x42, 0x0E, 0x70, 0x10, 0x08,
	 0x10, 0x08, 0x0E, 0x70, 0x42, 0x42, 0xA2, 0x45,
	 0xA1, 0x85, 0xB8, 0x1D, 0x84, 0x21, 0x78, 0x1E, },
};

Cursor whitearrow = {
	{-2, 0},
	{0x20, 0x00, 0x30, 0x00, 0x28, 0x00, 0x24, 0x00,
	 0x22, 0x00, 0x21, 0x00, 0x20, 0x80, 0x20, 0x40,
	 0x20, 0x20, 0x20, 0x10, 0x21, 0xF8, 0x22, 0x00,
	 0x24, 0x00, 0x28, 0x00, 0x30, 0x00, 0x20, 0x00, },
	{0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x18, 0x00,
	 0x1C, 0x00, 0x1E, 0x00, 0x1F, 0x00, 0x1F, 0x80,
	 0x1F, 0xC0, 0x1F, 0xE0, 0x1E, 0x00, 0x1C, 0x00,
	 0x18, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, },
};

Cursor defcursor = {
	{-2, 0},
	{0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x18, 0x00,
	 0x1C, 0x00, 0x1E, 0x00, 0x1F, 0x00, 0x1F, 0x80,
	 0x1F, 0xC0, 0x1F, 0xE0, 0x1E, 0x00, 0x1C, 0x00,
	 0x18, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, },
	{0x20, 0x00, 0x30, 0x00, 0x28, 0x00, 0x24, 0x00,
	 0x22, 0x00, 0x21, 0x00, 0x20, 0x80, 0x20, 0x40,
	 0x20, 0x20, 0x20, 0x10, 0x21, 0xF8, 0x22, 0x00,
	 0x24, 0x00, 0x28, 0x00, 0x30, 0x00, 0x20, 0x00, },
};

Cursor query = {
	{-7,-7},
	{0x0f, 0xf0, 0x1f, 0xf8, 0x3f, 0xfc, 0x7f, 0xfe,
	 0x7c, 0x7e, 0x78, 0x7e, 0x00, 0xfc, 0x01, 0xf8,
	 0x03, 0xf0, 0x07, 0xe0, 0x07, 0xc0, 0x07, 0xc0,
	 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, },
	{0x00, 0x00, 0x0f, 0xf0, 0x1f, 0xf8, 0x3c, 0x3c,
	 0x38, 0x1c, 0x00, 0x3c, 0x00, 0x78, 0x00, 0xf0,
	 0x01, 0xe0, 0x03, 0xc0, 0x03, 0x80, 0x03, 0x80,
	 0x00, 0x00, 0x03, 0x80, 0x03, 0x80, 0x00, 0x00, }
};

Cursor tl = {
	{-6, -6},
	{0x00, 0x00, 0x78, 0x00, 0x60, 0x00, 0x50, 0x00,
	 0x48, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x40, 0x00, 0x20, 0x00, 0x12,
	 0x00, 0x0A, 0x00, 0x06, 0x00, 0x1E, 0x00, 0x00, },
	{0xFC, 0x00, 0x84, 0x00, 0x9C, 0x00, 0xAC, 0x00,
	 0xB6, 0x00, 0xFB, 0x00, 0x0D, 0x00, 0x07, 0x00,
	 0x00, 0xE0, 0x00, 0xB0, 0x00, 0xDF, 0x00, 0x6D,
	 0x00, 0x35, 0x00, 0x39, 0x00, 0x21, 0x00, 0x3F, },
};

Cursor t = {
	{-7, -8},
	{0x00, 0x00, 0x01, 0x80, 0x03, 0xC0, 0x01, 0x80,
	 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00,
	 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
	 0x01, 0x80, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00, },
	{0x01, 0x80, 0x02, 0x40, 0x04, 0x20, 0x06, 0x60,
	 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x01, 0x80,
	 0x01, 0x80, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40,
	 0x06, 0x60, 0x04, 0x20, 0x02, 0x40, 0x01, 0x80, }
};

Cursor tr = {
	{-9, -6},
	{0x00, 0x00, 0x00, 0x1E, 0x00, 0x06, 0x00, 0x0A,
	 0x00, 0x12, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00,
	 0x00, 0x00, 0x02, 0x00, 0x04, 0x00, 0x48, 0x00,
	 0x50, 0x00, 0x60, 0x00, 0x78, 0x00, 0x00, 0x00, },
	{0x00, 0x3F, 0x00, 0x21, 0x00, 0x39, 0x00, 0x35,
	 0x00, 0x6D, 0x00, 0xDF, 0x00, 0xB0, 0x00, 0xE0,
	 0x07, 0x00, 0x0D, 0x00, 0xFB, 0x00, 0xB6, 0x00,
	 0xAC, 0x00, 0x9C, 0x00, 0x84, 0x00, 0xFC, 0x00, },
};

Cursor r = {
	{-8, -7},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x7E, 0x7E,
	 0x7E, 0x7E, 0x20, 0x04, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x30, 0x0C, 0x5E, 0x7A, 0x81, 0x81,
	 0x81, 0x81, 0x5E, 0x7A, 0x30, 0x0C, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, }
};

Cursor br = {
	{-9, -9},
	{0x00, 0x00, 0x78, 0x00, 0x60, 0x00, 0x50, 0x00,
	 0x48, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x40, 0x00, 0x20, 0x00, 0x12,
	 0x00, 0x0A, 0x00, 0x06, 0x00, 0x1E, 0x00, 0x00, },
	{0xFC, 0x00, 0x84, 0x00, 0x9C, 0x00, 0xAC, 0x00,
	 0xB6, 0x00, 0xFB, 0x00, 0x0D, 0x00, 0x07, 0x00,
	 0x00, 0xE0, 0x00, 0xB0, 0x00, 0xDF, 0x00, 0x6D,
	 0x00, 0x35, 0x00, 0x39, 0x00, 0x21, 0x00, 0x3F, },
};

Cursor b = {
	{-7, -7},
	{0x00, 0x00, 0x01, 0x80, 0x03, 0xC0, 0x01, 0x80,
	 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00,
	 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
	 0x01, 0x80, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00, },
	{0x01, 0x80, 0x02, 0x40, 0x04, 0x20, 0x06, 0x60,
	 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x01, 0x80,
	 0x01, 0x80, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40,
	 0x06, 0x60, 0x04, 0x20, 0x02, 0x40, 0x01, 0x80, }
};

Cursor bl = {
	{-6, -9},
	{0x00, 0x00, 0x00, 0x1E, 0x00, 0x06, 0x00, 0x0A,
	 0x00, 0x12, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00,
	 0x00, 0x00, 0x02, 0x00, 0x04, 0x00, 0x48, 0x00,
	 0x50, 0x00, 0x60, 0x00, 0x78, 0x00, 0x00, 0x00, },
	{0x00, 0x3F, 0x00, 0x21, 0x00, 0x39, 0x00, 0x35,
	 0x00, 0x6D, 0x00, 0xDF, 0x00, 0xB0, 0x00, 0xE0,
	 0x07, 0x00, 0x0D, 0x00, 0xFB, 0x00, 0xB6, 0x00,
	 0xAC, 0x00, 0x9C, 0x00, 0x84, 0x00, 0xFC, 0x00, }
};

Cursor l = {
	{-7, -7},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x7E, 0x7E,
	 0x7E, 0x7E, 0x20, 0x04, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x30, 0x0C, 0x5E, 0x7A, 0x81, 0x81,
	 0x81, 0x81, 0x5E, 0x7A, 0x30, 0x0C, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, }
};

Cursor *corners[9] = {
	&tl,	&t,	&tr,
	&l,	nil,	&r,
	&bl,	&b,	&br,
};

int
gettitleheight(){
	Point p = stringsize(font, "Á");
	return 2 * Borderwidth + p.y;
}

void
initclosebutton(){
	int n = gettitleheight() - (2 * Borderwidth);
	Rectangle r = Rect(0, 0, n, n);
	//closebutton = allocimage(display, r, CMAP8, 1, DRed);
	closebutton = allocimage(display, r, CMAP8, 1, jayconfig->windowTitleColor);
	closebuttonhoover = allocimage(display, r, CMAP8, 1, 0xD8D8D8FF);
	fillellipse(closebutton, Pt((r.min.x + r.max.x)/2,(r.min.y + r.max.y)/2), Dx(r)/2 - 1, Dx(r)/2 - 1, closebuttonhoover, r.min);
	//border3d(closebutton, r, 1, wht, blk, r.min);
	line(closebutton, Pt(r.min.x + Borderwidth, r.min.y + Borderwidth ), Pt(r.max.x - Borderwidth, r.max.y - Borderwidth), 0, 0, 0, blk, r.min);
	line(closebutton, Pt(r.max.x - Borderwidth, r.min.y + Borderwidth ), Pt(r.min.x + Borderwidth, r.max.y - Borderwidth), 0, 0, 0, blk, r.min);
	draw(closebuttonhoover, closebutton->r, closebutton, nil, closebutton->r.min);
	border3d(closebuttonhoover, r, 1, blk, wht, r.min);
}

void
initmaximizebutton() {
	int n = gettitleheight() - (2 * Borderwidth);
	Rectangle r = Rect(0, 0, n, n);
	//maximizebutton = allocimage(display, r, CMAP8, 1, DGreen);
	maximizebutton = allocimage(display, r, CMAP8, 1, jayconfig->windowTitleColor);
	maximizebuttonhoover = allocimage(display, r, CMAP8, 1, 0xD8D8D8FF);
	fillellipse(maximizebutton, Pt((r.min.x + r.max.x)/2,(r.min.y + r.max.y)/2), Dx(r)/2 - 1, Dx(r)/2 - 1, maximizebuttonhoover, r.min);
	ellipse(maximizebutton, Pt((r.min.x + r.max.x)/2,(r.min.y + r.max.y)/2), Dx(r)/2 - 4, Dx(r)/2 - 4, 0, display->black, r.min);
	//border3d(maximizebutton, r, 1, wht, blk, r.min);
	/*Point x1 = Pt((r.min.x + r.max.x)/2 -1, r.min.y + 2);
	Point y1 = Pt((r.min.x + r.max.x)/2 -1, r.max.y - 3);
	Point x2 = Pt(r.min.x + 2, (r.min.y + r.max.y)/2 - 1);l
	Point y2 = Pt(r.max.x - 3, (r.min.y + r.max.y)/2 - 1);
	line(maximizebutton, x1, y1, Endsquare, 0, 1, blk, r.min);
	line(maximizebutton, x2, y2, Endsquare, 0, 1, blk, r.min);
	line(maximizebutton, Pt(r.min.x+4, r.min.y + 3), Pt(r.max.x-4, r.min.y + 3),Endsquare, 0, 1, blk, r.min);
	line(maximizebutton, Pt(r.min.x+4, r.max.y - 3), Pt(r.max.x-4, r.max.y - 3),Endsquare, 0, 1, blk, r.min);
	line(maximizebutton, Pt(r.min.x+4, r.min.y + 3), Pt(r.min.x+4, r.max.y - 3),Endsquare, 0, 1, blk, r.min);
	line(maximizebutton, Pt(r.max.x-4, r.min.y + 3), Pt(r.max.x-4, r.max.y - 3),Endsquare, 0, 1, blk, r.min);*/
	draw(maximizebuttonhoover, maximizebutton->r, maximizebutton, nil, maximizebutton->r.min);
	border3d(maximizebuttonhoover, r, 1, blk, wht, r.min);
}

void
initminimizebutton(){
	int n = gettitleheight() - (2 * Borderwidth);
	Rectangle r = Rect(0, 0, n, n);
	//minimizebutton = allocimage(display, r, CMAP8, 1, DYellow);
	minimizebutton = allocimage(display, r, CMAP8, 1, jayconfig->windowTitleColor);
	minimizebuttonhoover = allocimage(display, r, CMAP8, 1, 0xD8D8D8FF);
	fillellipse(minimizebutton, Pt((r.min.x + r.max.x)/2,(r.min.y + r.max.y)/2), Dx(r)/2 - 1, Dx(r)/2 - 1, minimizebuttonhoover, r.min);
	//border3d(minimizebutton, r, 1, wht, blk, r.min);
	int y = r.max.y - Borderwidth;
	line(minimizebutton, Pt(r.min.x + Borderwidth, y), Pt(r.max.x - Borderwidth, y), 0, 0, 0, blk, r.min);
	draw(minimizebuttonhoover, minimizebutton->r, minimizebutton, nil, minimizebutton->r.min);
	border3d(minimizebuttonhoover, r, 1, blk, wht, r.min);
}

static int
setbackimg(){
	if (jayconfig->backgroundimgpath == nil){
		return 0;
	}
	int fd = open(jayconfig->backgroundimgpath, OREAD);
	if (fd > 0){
		background = readimage(display, fd, 0);
		close(fd);
		return 1;
	}
	return 0;
}

static void
seticoninit(void){
	initclosebutton();
	initmaximizebutton();
	initminimizebutton();
	if (!setbackimg()){
		background = allocimage(display, Rect(0,0,1,1), RGB24, 1, jayconfig->backgroundColor);
	}
	setmenucolor(jayconfig->menuBackColor, jayconfig->menuHighColor, jayconfig->menuBorderColor, jayconfig->menuTextColor, jayconfig->menuSelTextColor);
}


void
iconinit(void)
{
	initdefaultconfig();
	blk = allocimage(display, Rect(0,0,1,1), CMAP8, 1, DBlack);
	wht = allocimage(display, Rect(0,0,1,1), CMAP8, 1, DWhite);
	red = allocimage(display, Rect(0,0,1,1), RGB24, 1, 0xDD0000FF);
	seticoninit();
}

void
iconreinit(void) {
	freeimage(background);
	freeimage(closebutton);
	freeimage(closebuttonhoover);
	freeimage(minimizebutton);
	freeimage(maximizebutton);
	freeimage(maximizebuttonhoover);
	seticoninit();
}