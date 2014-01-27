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
#include <esc/thread.h>
#include <esc/sync.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

extern void initHeap(void);

static uint *tlsCopy = NULL;
extern tUserSem __libc_sem;
extern char *__progname;

/**
 * TODO: Actually this is not exactly the model described in doc/thread-local-storage.pdf.
 * But linux seems to do it this way, dmd expects it to be that way (at least on linux) and it
 * works for gcc (c) and dmd (d). So its ok for now. But I don't know how it works with dynamic
 * linking...
 *
 * The TLS-system looks like this:
 *
 *                        %gs offset
 *                            |
 * TLS-region of a thread:    v
 * +--------+--------+--------+--------+
 * |  val3  |  val2  |  val1  |  TCB   | ---\
 * +--------+--------+--------+--------+    |
 *                            ^-------------/
 */

/* make gcc happy */
uintptr_t __libc_preinit(uintptr_t entryPoint,uint *tlsStart,size_t tlsSize,int argc,char *argv[]);

uintptr_t __libc_preinit(uintptr_t entryPoint,uint *tlsStart,size_t tlsSize,A_UNUSED int argc,char *argv[]) {
	static bool initialized = false;
	if(!initialized) {
		char *name = __progname = argv[0];
		while((name = strchr(name,'/')) != NULL) {
			name++;
			__progname = name;
		}

		if(usemcrt(&__libc_sem,1) < 0)
			error("Unable to create libc lock");
		initHeap();
		initialized = true;
	}

	if(tlsSize) {
		size_t i;
		usemdown(&__libc_sem);
		/* create copy if not already done */
		if(tlsCopy == NULL) {
			tlsCopy = (uint*)malloc(tlsSize);
			assert(tlsCopy);
			memcpy(tlsCopy,tlsStart,tlsSize);
		}

		/* copy values to TLS-region */
		tlsSize /= 4;
		for(i = 0; i < tlsSize - 1; i++)
			tlsStart[i] = tlsCopy[i];
		/* put pointer to TCB in TCB */
		tlsStart[tlsSize - 1] = (uint)(tlsStart + tlsSize - 1);
		usemup(&__libc_sem);
	}
	return entryPoint;
}
