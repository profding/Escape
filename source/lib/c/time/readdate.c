/**
 * $Id$
 * Copyright (C) 2008 - 2014 Nils Asmussen
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
#include <esc/driver.h>
#include <time.h>
#include "timeintern.h"

int readdate(sRTCInfo *info) {
	/* open CMOS and read date */
	int err;
	int fd = open(TIME_DEVICE,IO_READ);
	if(fd < 0)
		return fd;
	if((err = IGNSIGS(read(fd,info,sizeof(sRTCInfo)))) < 0) {
		close(fd);
		return err;
	}
	close(fd);
	return 0;
}
