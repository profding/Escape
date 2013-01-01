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
#include <gui/control.h>
#include <algorithm>
#include <utility>

namespace gui {
	class Panel;

	/**
	 * The interface for all layouts. Layouts are used to auto-position and -size the controls on
	 * a panel.
	 */
	class Layout {
	public:
		typedef int pos_type;

	public:
		/**
		 * Constructor
		 */
		Layout() {
		};
		/**
		 * Destructor
		 */
		virtual ~Layout() {
		};

		/**
		 * Adds the given control to the layout
		 *
		 * @param p the panel of the control
		 * @param c the control
		 * @param pos specifies the position (layout-implementation-dependend)
		 */
		virtual void add(Panel *p,Control *c,pos_type pos) = 0;
		/**
		 * Removes the given control/position from the layout
		 *
		 * @param p the panel of the control
		 * @param c the control
		 * @param pos specifies the position (layout-implementation-dependend)
		 */
		virtual void remove(Panel *p,Control *c,pos_type pos) = 0;

		/**
		 * @return the total preferred width of this layout without padding
		 */
		virtual gsize_t getPreferredWidth() const = 0;
		/**
		 * @return the total preferred height of this layout without padding
		 */
		virtual gsize_t getPreferredHeight() const = 0;

		/**
		 * Determines what size would be used if <width>X<height> is available. By default, this
		 * is always the maximum of preferredSize and <width>/<height>.
		 * In contrast to getPreferred*(), padding should be included here.
		 *
		 * @param width the available width
		 * @param height the available height
		 * @return the size that would be used
		 */
		virtual std::pair<gsize_t,gsize_t> getUsedSize(gsize_t width,gsize_t height) const {
			return std::make_pair(std::max(getPreferredWidth(),width),
			                      std::max(getPreferredHeight(),height));
		};

		/**
		 * Rearranges the controls, i.e. determines the position and sizes again. Does not
		 * perform a repaint!
		 */
		virtual void rearrange() = 0;

	protected:
		inline void configureControl(Control *c,gpos_t x,gpos_t y,gsize_t width,gsize_t height) const {
			c->moveTo(x,y);
			c->resizeTo(width,height);
		};

	private:
		// no cloning
		Layout(const Layout& bl);
		Layout& operator=(const Layout& bl);
	};
}
