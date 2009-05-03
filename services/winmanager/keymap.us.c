/**
 * $Id: keymap.us.c 202 2009-04-11 16:07:24Z nasmussen $
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
#include <esc/keycodes.h>
#include "keymap.h"
#include "keymap.us.h"

static sKeymapEntry keymap[] = {
	/* - none - */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_ACCENT */		{'`',		'~',		NPRINT	},
	/* VK_0 */			{'0',		')',		NPRINT	},
	/* VK_1 */			{'1',		'!',		NPRINT	},
	/* VK_2 */			{'2',		'@',		NPRINT	},
	/* VK_3 */			{'3',		'#',		NPRINT	},
	/* VK_4 */			{'4',		'$',		NPRINT	},
	/* VK_5 */			{'5',		'%',		NPRINT	},
	/* VK_6 */			{'6',		'^',		NPRINT	},
	/* VK_7 */			{'7',		'&',		NPRINT	},
	/* VK_8 */			{'8',		'*',		NPRINT	},
	/* VK_9 */			{'9',		'(',		NPRINT	},
	/* VK_MINUS */		{'-',		'_',		NPRINT	},
	/* VK_EQ */			{'=',		'+',		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_BACKSP */		{'\b',		'\b',		NPRINT	},
	/* VK_TAB */		{'\t',		'\t',		NPRINT	},
	/* VK_Q */			{'q',		'Q',		NPRINT	},
	/* VK_W */			{'w',		'W',		NPRINT	},
	/* VK_E */			{'e',		'E',		NPRINT	},
	/* VK_R */			{'r',		'R',		NPRINT	},
	/* VK_T */			{'t',		'T',		NPRINT	},
	/* VK_Y */			{'y',		'Y',		NPRINT	},
	/* VK_U */			{'u',		'U',		NPRINT	},
	/* VK_I */			{'i',		'I',		NPRINT	},
	/* VK_O */			{'o',		'O',		NPRINT	},
	/* VK_P */			{'p',		'P',		NPRINT	},
	/* VK_LBRACKET */	{'[',		'{',		NPRINT	},
	/* VK_RBRACKET */	{']',		'}',		NPRINT	},
	/* VK_BACKSLASH */	{'\\',		'|',		NPRINT	},
	/* VK_CAPS */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_A */			{'a',		'A',		NPRINT	},
	/* VK_S */			{'s',		'S',		NPRINT	},
	/* VK_D */			{'d',		'D',		NPRINT	},
	/* VK_F */			{'f',		'F',		NPRINT	},
	/* VK_G */			{'g',		'G',		NPRINT	},
	/* VK_H */			{'h',		'H',		NPRINT	},
	/* VK_J */			{'j',		'J',		NPRINT	},
	/* VK_K */			{'k',		'K',		NPRINT	},
	/* VK_L */			{'l',		'L',		NPRINT	},
	/* VK_SEM */		{';',		':',		NPRINT	},
	/* VK_APOS */		{'\'',		'"',		NPRINT	},
	/* non-US-1 */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_ENTER */		{'\n',		'\n',		NPRINT	},
	/* VK_LSHIFT */		{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_Z */			{'z',		'Z',		NPRINT	},
	/* VK_X */			{'x',		'X',		NPRINT	},
	/* VK_C */			{'c',		'C',		NPRINT	},
	/* VK_V */			{'v',		'V',		NPRINT	},
	/* VK_B */			{'b',		'B',		NPRINT	},
	/* VK_N */			{'n',		'N',		NPRINT	},
	/* VK_M */			{'m',		'M',		NPRINT	},
	/* VK_COMMA */		{',',		'<',		NPRINT	},
	/* VK_DOT */		{'.',		'>',		NPRINT	},
	/* VK_SLASH */		{'/',		'?',		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_RSHIFT */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_LCTRL */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_LSUPER */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_LALT */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_SPACE */		{' ',		' ',		NPRINT	},
	/* VK_RALT */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_APPS */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_RCTRL */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_RSUPER */		{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_INSERT */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_DELETE */		{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_LEFT */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_HOME */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_END */		{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_UP */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_DOWN */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_PGUP */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_PGDOWN */		{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_RIGHT */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_NUM */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_KP7 */		{'7',		'7',		NPRINT	},
	/* VK_KP4 */		{'4',		'4',		NPRINT	},
	/* VK_KP1 */		{'1',		'1',		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_KPDIV */		{'/',		'/',		NPRINT	},
	/* VK_KP8 */		{'8',		'8',		NPRINT	},
	/* VK_KP5 */		{'5',		'5',		NPRINT	},
	/* VK_KP2 */		{'2',		'2',		NPRINT	},
	/* VK_KP0 */		{'0',		'0',		NPRINT	},
	/* VK_KPMUL */		{'*',		'*',		NPRINT	},
	/* VK_KP9 */		{'9',		'9',		NPRINT	},
	/* VK_KP6 */		{'6',		'6',		NPRINT	},
	/* VK_KP3 */		{'3',		'3',		NPRINT	},
	/* VK_KPDOT */		{'.',		'.',		NPRINT	},
	/* VK_KPSUB */		{'-',		'-',		NPRINT	},
	/* VK_KPADD */		{'+',		'+',		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_KPENTER */	{'\n',		'\n',		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_ESC */		{NPRINT,	NPRINT,		NPRINT	},
	/* --- */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F1 */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F2 */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F3 */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F4 */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F5 */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F6 */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F7 */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F8 */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F9 */			{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F10 */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F11 */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_F12 */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_PRINT */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_SCROLL */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_PAUSE */		{NPRINT,	NPRINT,		NPRINT	},
	/* VK_PIPE */		{NPRINT,	NPRINT,		NPRINT	},
};

sKeymapEntry *keymap_us_get(u8 keyCode) {
	if(keyCode >= ARRAY_SIZE(keymap))
		return NULL;
	return keymap + keyCode;
}
