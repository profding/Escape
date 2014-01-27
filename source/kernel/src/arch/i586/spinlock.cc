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

#include <sys/spinlock.h>
#include <sys/task/thread.h>
#include <sys/cpu.h>
#include <sys/util.h>
#include <stdarg.h>

#if DEBUG_LOCKS
#define MAX_WAIT_SECS		2

static void panic(const char *msg,...) {
	static bool havePaniced = false;
	va_list ap;
	va_start(ap,msg);
	if(havePaniced)
		return;
	havePaniced = true;
	Util::vpanic(msg,ap);
	va_end(ap);
}

void SpinLock::acquire(klock_t *l) {
	if(Util::IsPanicStarted())
		return;

	Thread *t = Thread::getRunning();
	unsigned id = t->getTid() + 1;
	if(*l == id) {
		panic("Self-deadlock");
		return;
	}
	uint64_t max = CPU::rdtsc() + (MAX_WAIT_SECS * CPU::getSpeed());
	while(!Atomic::cmpnswap(l, 0U, id)) {
		if(CPU::rdtsc() > max) {
			panic("Acquiring spinlock %p took too long",l);
			return;
		}
		CPU::pause();
	}
}

bool SpinLock::tryAcquire(klock_t *l) {
	return Util::IsPanicStarted() || Atomic::cmpnswap(l, 0, 1);
}
#endif
