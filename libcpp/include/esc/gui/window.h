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

#ifndef WINDOW_H_
#define WINDOW_H_

#include <esc/common.h>
#include <esc/io.h>
#include <esc/messages.h>
#include <esc/gui/common.h>
#include <esc/gui/graphics.h>
#include <esc/gui/uielement.h>
#include <esc/gui/application.h>
#include <esc/gui/control.h>
#include <esc/string.h>

namespace esc {
	namespace gui {
		class Window : public UIElement {
		private:
			static const tColor bgColor = 0x808080;
			static const tColor titleBgColor = 0x0000FF;
			static const tColor titleFgColor = 0xFFFFFF;
			static const tColor borderColor = 0x707070;

			typedef struct {
				u16 x;
				u16 y;
				u16 width;
				u16 height;
			} sRectangle;

			typedef struct {
				sMsgHeader header;
				sMsgDataWinMoveReq data;
			} __attribute__((packed)) sMsgWinMoveReq;

			typedef struct {
				sMsgHeader header;
				sMsgDataWinCreateReq data;
			} __attribute__((packed)) sMsgWinCreateReq;

		public:
			Window(const String &title,tCoord x,tCoord y,tSize width,tSize height);
			virtual ~Window();

			inline tWinId getId() const {
				return _id;
			};
			inline tSize getTitleBarHeight() const {
				return _titleBarHeight;
			};
			inline String getTitle() const {
				return _title;
			};
			inline void setTitle(const String &title) {
				_title = title;
				paint();
			};

			virtual void paint();
			void move(s16 x,s16 y);
			void moveTo(tCoord x,tCoord y);

			void add(Control &c);

		private:
			tWinId _id;
			String _title;
			tSize _titleBarHeight;
			Vector<Control*> _controls;
		};
	}
}

#endif /* WINDOW_H_ */
