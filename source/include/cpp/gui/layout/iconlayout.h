/**
 * $Id$
 * Copyright (C) 2008 - 2011 Nils Asmussen
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

#pragma once

#include <esc/common.h>
#include <gui/layout/layout.h>
#include <gui/control.h>

namespace gui {
	/**
	 * The iconlayout tries to put as much controls as possible in the horizontal/vertical
	 * direction side by side and continues with the next row/column if its full.
	 */
	class IconLayout : public Layout {
		typedef void (*layout_func)(Control *c,gpos_t x,gpos_t y,gsize_t width,gsize_t height);

	public:
		enum Preferred {
			HORIZONTAL,
			VERTICAL
		};

		static const gsize_t DEF_GAP	= 2;

		/**
		 * Constructor
		 *
		 * @param pref the preferred direction
		 * @param gap the gap between the controls (default 2)
		 */
		IconLayout(Preferred pref,gsize_t gap = DEF_GAP)
			: Layout(), _pref(pref), _gap(gap), _p(), _ctrls() {
		};

		virtual void add(Panel *p,Control *c,pos_type pos);
		virtual void remove(Panel *p,Control *c,pos_type pos);

		virtual gsize_t getPreferredWidth() const;
		virtual gsize_t getPreferredHeight() const;
		virtual std::pair<gsize_t,gsize_t> getUsedSize(gsize_t width,gsize_t height) const;

		virtual void rearrange();

	private:
		void doLayout(size_t cols,size_t rows,gsize_t &width,gsize_t &height,
		              layout_func layout = 0) const;
		static void layoutCtrl(Control *c,gpos_t x,gpos_t y,gsize_t width,gsize_t height);

	private:
		Preferred _pref;
		gsize_t _gap;
		Panel *_p;
		std::vector<Control*> _ctrls;
	};
}
