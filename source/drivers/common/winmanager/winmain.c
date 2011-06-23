/**
 * $Id$
 * Copyright (C) 2008 - 2009 Nils Asmussen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <esc/common.h>
#include <esc/io.h>
#include <esc/debug.h>
#include <esc/driver.h>
#include <esc/keycodes.h>
#include <esc/proc.h>
#include <esc/thread.h>
#include <esc/messages.h>
#include <signal.h>
#include <stdio.h>
#include <error.h>

#include "window.h"

#define MOUSE_DATA_BUF_SIZE	128
#define KB_DATA_BUF_SIZE	128

/**
 * Reads from the mouse-driver
 */
static bool readMouse(tFD drvId,tFD mouse);
/**
 * Reads from the km-manager
 */
static bool readKeyboard(tFD drvId,tFD kmmng);
/**
 * Destroys the windows of a died thread
 */
static void deadThreadHandler(int sig);
/**
 * Handles a message from kmmng
 */
static void handleKbMessage(tFD drvId,sWindow *active,uchar keycode,bool isBreak,
		uchar modifier,char c);
/**
 * Handles a message from the mouse
 */
static void handleMouseMessage(tFD drvId,sMouseData *mdata);

/* mouse state */
static uchar buttons = 0;
static tCoord curX = 0;
static tCoord curY = 0;
static uchar cursor = CURSOR_DEFAULT;

static bool enabled = false;
static sMsg msg;
static tSize screenWidth;
static tSize screenHeight;
static sMouseData mouseData[MOUSE_DATA_BUF_SIZE];
static sKmData kbData[KB_DATA_BUF_SIZE];
static sWindow *mouseWin = NULL;

int main(void) {
	sWaitObject waits[3];
	tFD mouse,kmmng;
	tFD drvId;
	tMsgId mid;

	mouse = open("/dev/mouse",IO_READ | IO_NOBLOCK);
	if(mouse < 0)
		error("Unable to open /dev/mouse");

	kmmng = open("/dev/kmmanager",IO_READ | IO_NOBLOCK);
	if(kmmng < 0)
		error("Unable to open /dev/kmmanager");

	drvId = regDriver("winmanager",DRV_CLOSE);
	if(drvId < 0)
		error("Unable to create driver winmanager");

	waits[0].events = EV_CLIENT;
	waits[0].object = drvId;
	waits[1].events = EV_DATA_READABLE;
	waits[1].object = kmmng;
	waits[2].events = EV_DATA_READABLE;
	waits[2].object = mouse;

	if(!win_init(drvId))
		return EXIT_FAILURE;

	screenWidth = win_getScreenWidth();
	screenHeight = win_getScreenHeight();

	while(1) {
		tFD fd = getWork(&drvId,1,NULL,&mid,&msg,sizeof(msg),GW_NOBLOCK);
		if(fd >= 0) {
			switch(mid) {
				case MSG_WIN_CREATE_REQ: {
					tCoord x = (tCoord)(msg.args.arg1 >> 16);
					tCoord y = (tCoord)(msg.args.arg1 & 0xFFFF);
					tSize width = (tSize)(msg.args.arg2 >> 16);
					tSize height = (tSize)(msg.args.arg2 & 0xFFFF);
					tWinId tmpWinId = (tWinId)msg.args.arg3;
					uint style = msg.args.arg4;
					msg.args.arg1 = tmpWinId;
					msg.args.arg2 = win_create(x,y,width,height,getClientId(fd),style);
					send(fd,MSG_WIN_CREATE_RESP,&msg,sizeof(msg.args));
					if(style == WIN_STYLE_POPUP)
						win_setActive(msg.args.arg2,false,curX,curY);
				}
				break;

				case MSG_WIN_DESTROY: {
					tWinId wid = (tWinId)msg.args.arg1;
					if(win_exists(wid))
						win_destroy(wid,curX,curY);
				}
				break;

				case MSG_WIN_MOVE: {
					tWinId wid = (tWinId)msg.args.arg1;
					tCoord x = (tCoord)msg.args.arg2;
					tCoord y = (tCoord)msg.args.arg3;
					if(enabled && win_exists(wid) && x < screenWidth && y < screenHeight)
						win_moveTo(wid,x,y);
				}
				break;

				case MSG_WIN_RESIZE: {
					tWinId wid = (tWinId)msg.args.arg1;
					tSize width = (tSize)msg.args.arg2;
					tSize height = (tSize)msg.args.arg3;
					if(enabled && win_exists(wid))
						win_resize(wid,width,height);
				}
				break;

				case MSG_WIN_UPDATE_REQ: {
					tWinId wid = (tWinId)msg.args.arg1;
					tCoord x = (tCoord)msg.args.arg2;
					tCoord y = (tCoord)msg.args.arg3;
					tSize width = (tSize)msg.args.arg4;
					tSize height = (tSize)msg.args.arg5;
					sWindow *win = win_get(wid);
					if(enabled && win != NULL && x + width > x && y + height > y &&
						x + width <= win->width && y + height <= win->height) {
						win_update(wid,x,y,width,height);
					}
				}
				break;

				case MSG_WIN_ENABLE:
					win_setVesaEnabled(true);
					if(!enabled)
						win_updateScreen();
					enabled = true;
					break;

				case MSG_WIN_DISABLE:
					win_setVesaEnabled(false);
					enabled = false;
					break;

				case MSG_DRV_CLOSE:
					win_destroyWinsOf(getClientId(fd),curX,curY);
					break;

				default:
					msg.args.arg1 = ERR_UNSUPPORTED_OP;
					send(fd,MSG_DEF_RESPONSE,&msg,sizeof(msg.args));
					break;
			}
			close(fd);
		}
		else {
			bool hasRead = false;
			if(enabled) {
				hasRead |= readMouse(drvId,mouse);
				hasRead |= readKeyboard(drvId,kmmng);
			}
			if(!hasRead) {
				if(enabled)
					waitm(waits,ARRAY_SIZE(waits));
				else
					waitm(waits,1);
			}
		}
	}

	close(drvId);
	close(kmmng);
	close(mouse);
	return EXIT_SUCCESS;
}

static bool readMouse(tFD drvId,tFD mouse) {
	ssize_t count;
	while((count = RETRY(read(mouse,mouseData,sizeof(mouseData)))) > 0) {
		sMouseData *msd = mouseData;
		count /= sizeof(sMouseData);
		while(count-- > 0) {
			handleMouseMessage(drvId,msd);
			msd++;
		}
	}
	if(count < 0) {
		if(count != ERR_WOULD_BLOCK)
			printe("[WINM] Unable to read from mouse");
		return false;
	}
	return true;
}

static bool readKeyboard(tFD drvId,tFD kmmng) {
	sWindow *active = win_getActive();
	ssize_t count;
	while((count = RETRY(read(kmmng,kbData,sizeof(kbData)))) > 0) {
		sKmData *kbd = kbData;
		count /= sizeof(sKmData);
		while(count-- > 0) {
			/*printf("kc=%d, brk=%d\n",kbd->keycode,kbd->isBreak);*/
			handleKbMessage(drvId,active,kbd->keycode,kbd->isBreak,kbd->modifier,
					kbd->character);
			kbd++;
		}
	}
	if(count < 0) {
		if(count != ERR_WOULD_BLOCK)
			printe("[WINM] Unable to read from kmmanager");
		return false;
	}
	return true;
}

static void handleKbMessage(tFD drvId,sWindow *active,uchar keycode,bool isBreak,
		uchar modifier,char c) {
	tFD aWin;
	if(!active)
		return;
	msg.args.arg1 = keycode;
	msg.args.arg2 = isBreak;
	msg.args.arg3 = active->id;
	msg.args.arg4 = c;
	msg.args.arg5 = modifier;
	aWin = getClient(drvId,active->owner);
	if(aWin >= 0) {
		send(aWin,MSG_WIN_KEYBOARD,&msg,sizeof(msg.args));
		close(aWin);
	}
}

static void handleMouseMessage(tFD drvId,sMouseData *mdata) {
	tCoord oldx = curX,oldy = curY;
	bool btnChanged = false;
	sWindow *w;
	curX = MAX(0,MIN(screenWidth - 1,curX + mdata->x));
	curY = MAX(0,MIN(screenHeight - 1,curY - mdata->y));

	/* set active window */
	if(mdata->buttons != buttons) {
		btnChanged = true;
		buttons = mdata->buttons;
		if(buttons) {
			w = win_getAt(curX,curY);
			if(w->style != WIN_STYLE_DESKTOP) {
				if(w)
					win_setActive(w->id,true,curX,curY);
				else
					win_setActive(WINDOW_COUNT,false,curX,curY);
			}
			mouseWin = w;
		}
	}

	/* if no buttons are pressed, change the cursor if we're at a window-border */
	if(!buttons) {
		w = mouseWin ? mouseWin : win_getAt(curX,curY);
		cursor = CURSOR_DEFAULT;
		if(w && w->style != WIN_STYLE_POPUP && w->style != WIN_STYLE_DESKTOP) {
			bool left = curX < w->x + CURSOR_RESIZE_WIDTH;
			bool right = curX >= w->x + w->width - CURSOR_RESIZE_WIDTH;
			bool bottom = curY >= w->y + w->height - CURSOR_RESIZE_WIDTH;
			if(left && bottom)
				cursor = CURSOR_RESIZE_BL;
			else if(left)
				cursor = CURSOR_RESIZE_L;
			if(right && bottom)
				cursor = CURSOR_RESIZE_BR;
			else if(right)
				cursor = CURSOR_RESIZE_R;
			else if(bottom && !left)
				cursor = CURSOR_RESIZE_VERT;
		}
	}

	/* let vesa draw the cursor */
	if(curX != oldx || curY != oldy)
		win_setCursor(curX,curY,cursor);

	/* send to window */
	w = mouseWin ? mouseWin : win_getActive();
	if(w) {
		tFD aWin = getClient(drvId,w->owner);
		if(aWin >= 0) {
			msg.args.arg1 = curX;
			msg.args.arg2 = curY;
			msg.args.arg3 = mdata->x;
			msg.args.arg4 = -mdata->y;
			msg.args.arg5 = mdata->buttons;
			msg.args.arg6 = w->id;
			send(aWin,MSG_WIN_MOUSE,&msg,sizeof(msg.args));
			close(aWin);
		}
	}

	if(btnChanged && !buttons)
		mouseWin = NULL;
}
