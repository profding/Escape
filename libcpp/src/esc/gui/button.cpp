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
#include <esc/gui/common.h>
#include <esc/gui/button.h>
#include <esc/gui/control.h>

namespace esc {
	namespace gui {
		Color Button::FGCOLOR = Color(0xFF,0xFF,0xFF);
		Color Button::BGCOLOR = Color(0x80,0x80,0x80);
		Color Button::LIGHT_BORDER_COLOR = Color(0x60,0x60,0x60);
		Color Button::DARK_BORDER_COLOR = Color(0x20,0x20,0x20);

		void Button::onFocusGained() {
			_focused = true;
			paint();
		}
		void Button::onFocusLost() {
			_focused = false;
			paint();
		}

		void Button::onMousePressed(const MouseEvent &e) {
			UNUSED(e);
			if(!_pressed) {
				_pressed = true;
				paint();
			}
		}
		void Button::onMouseReleased(const MouseEvent &e) {
			UNUSED(e);
			if(_pressed) {
				_pressed = false;
				paint();
			}
		}

		void Button::paint() {
			Control::paint();

			_g->setColor(BGCOLOR);
			_g->fillRect(1,1,getWidth() - 2,getHeight() - 2);

			_g->setColor(LIGHT_BORDER_COLOR);
			_g->drawLine(0,0,getWidth() - 1,0);
			if(_focused)
				_g->drawLine(0,1,getWidth() - 1,1);
			_g->drawLine(0,0,0,getHeight() - 1);
			if(_focused)
				_g->drawLine(1,0,1,getHeight() - 1);

			_g->setColor(DARK_BORDER_COLOR);
			_g->drawLine(getWidth() - 1,0,getWidth() - 1,getHeight() - 1);
			if(_focused)
				_g->drawLine(getWidth() - 2,0,getWidth() - 2,getHeight() - 1);
			_g->drawLine(0,getHeight() - 1,getWidth() - 1,getHeight() - 1);
			if(_focused)
				_g->drawLine(0,getHeight() - 2,getWidth() - 1,getHeight() - 2);

			_g->setColor(FGCOLOR);
			if(_pressed) {
				_g->drawString((getWidth() - _g->getFont().getStringWidth(_text)) / 2 + 1,
						(getHeight() - _g->getFont().getHeight()) / 2 + 1,_text);
			}
			else {
				_g->drawString((getWidth() - _g->getFont().getStringWidth(_text)) / 2,
						(getHeight() - _g->getFont().getHeight()) / 2,_text);
			}

			_g->update();
		}
	}
}
