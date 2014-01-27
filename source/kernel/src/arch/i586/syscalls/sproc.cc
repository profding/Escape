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

#include <sys/common.h>
#include <sys/arch/i586/gdt.h>
#include <sys/arch/i586/task/vm86.h>
#include <sys/arch/i586/task/ioports.h>
#include <sys/mem/pagedir.h>
#include <sys/task/proc.h>
#include <sys/syscalls.h>
#include <assert.h>
#include <errno.h>

int Syscalls::reqports(A_UNUSED Thread *t,IntrptStackFrame *stack) {
	uint16_t start = SYSC_ARG1(stack);
	size_t count = SYSC_ARG2(stack);

	/* check range */
	if(count == 0 || count > 0xFFFF || (uint32_t)start + count > 0xFFFF)
		SYSC_ERROR(stack,-EINVAL);

	int err = IOPorts::request(start,count);
	if(err < 0)
		SYSC_ERROR(stack,err);
	SYSC_RET1(stack,0);
}

int Syscalls::relports(A_UNUSED Thread *t,IntrptStackFrame *stack) {
	uint16_t start = SYSC_ARG1(stack);
	size_t count = SYSC_ARG2(stack);

	/* check range */
	if(count == 0 || count > 0xFFFF || (uint32_t)start + count > 0xFFFF)
		SYSC_ERROR(stack,-EINVAL);

	int err = IOPorts::release(start,count);
	if(err < 0)
		SYSC_ERROR(stack,err);
	SYSC_RET1(stack,0);
}

int Syscalls::vm86start(A_UNUSED Thread *t,A_UNUSED IntrptStackFrame *stack) {
	int res;
	if((res = VM86::create()) == 0) {
		/* not reached */
		return 0;
	}
	SYSC_ERROR(stack,res);
}

int Syscalls::vm86int(A_UNUSED Thread *t,IntrptStackFrame *stack) {
	uint16_t interrupt = (uint16_t)SYSC_ARG1(stack);
	VM86::Regs *regs = (VM86::Regs*)SYSC_ARG2(stack);
	VM86::Memarea *mArea = (VM86::Memarea*)SYSC_ARG3(stack);

	/* check args */
	if(!PageDir::isInUserSpace((uintptr_t)regs,sizeof(VM86::Regs)))
		SYSC_ERROR(stack,-EFAULT);
	if(mArea != NULL) {
		if(!PageDir::isInUserSpace((uintptr_t)mArea,sizeof(VM86::Memarea)))
			SYSC_ERROR(stack,-EFAULT);
		/* ensure that only memory from the real-mode-memory can be copied */
		if(mArea->dst + mArea->size < mArea->dst || mArea->dst + mArea->size >= (1 * M + 64 * K))
			SYSC_ERROR(stack,-EFAULT);
		if(!PageDir::isInUserSpace((uintptr_t)mArea->src,mArea->size))
			SYSC_ERROR(stack,-EFAULT);
		for(size_t j = 0; j < mArea->ptrCount; j++) {
			if(mArea->ptr[j].offset + sizeof(uintptr_t) > mArea->size)
				SYSC_ERROR(stack,-EINVAL);
			if(!PageDir::isInUserSpace(mArea->ptr[j].result,mArea->ptr[j].size))
				SYSC_ERROR(stack,-EFAULT);
		}
	}

	/* do vm86-interrupt */
	int res = VM86::interrupt(interrupt,regs,mArea);
	if(res < 0)
		SYSC_ERROR(stack,res);
	SYSC_RET1(stack,res);
}
