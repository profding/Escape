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
#include <esc/syscalls.h>

/* protection-flags */
#define PROT_READ			0
#define PROT_WRITE			4UL
#define PROT_EXEC			8UL

/* mapping flags */
#define MAP_PRIVATE			0
#define MAP_GROWABLE		1UL
#define MAP_SHARED			2UL
#define MAP_STACK			16UL
#define MAP_NOFREE			32UL
#define MAP_TLS				64UL
#define MAP_GROWSDOWN		128UL
#define MAP_POPULATE		256UL
#define MAP_NOMAP			512UL
#define MAP_FIXED			1024UL

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Changes the size of the process's data-area. If <count> is positive <count> pages
 * will be added to the end of the data-area. Otherwise <count> pages will be removed at the
 * end.
 *
 * @param count the number of pages to add / remove
 * @return the *old* end of the data-segment. NULL indicates an error
 */
void *chgsize(ssize_t count) A_CHECKRET;

/**
 * Creates a new mapping in the virtual address space of the calling process. This might be anonymous
 * memory or a file, denoted by <fd>.
 *
 * @param addr the desired address (might be ignored)
 * @param length the number of bytes
 * @param loadLength the number of bytes that should be loaded from the given file (the rest is zero'd)
 * @param prot the protection flags (PROT_*)
 * @param flags the mapping flags (MAP_*)
 * @param fd optionally, a file descriptor of the file to map
 * @param offset the offset in the file
 * @return the virtual address or NULL if the mapping failed
 */
void *mmap(void *addr,size_t length,size_t loadLength,int prot,int flags,int fd,off_t offset);

/**
 * Maps <count> bytes at *<phys> into the virtual user-space and returns the start-address.
 *
 * @param phys will be set to the chosen physical address; if *phys != 0, this address will
 *   be used.
 * @param count the number of bytes to map
 * @param aligh the alignment (in bytes); if not 0, contiguous physical memory, aligned by <align>
 *   will be used.
 * @return the virtual address where it has been mapped or NULL if an error occurred
 */
void *regaddphys(uintptr_t *phys,size_t count,size_t align) A_CHECKRET;

/**
 * Changes the protection of the region denoted by the given address.
 *
 * @param addr the virtual address
 * @param prot the new protection-setting (PROT_*)
 * @return 0 on success
 */
static inline int mprotect(void *addr,uint prot) {
	return syscall2(SYSCALL_MPROTECT,(ulong)addr,prot);
}

/**
 * Unmaps the region denoted by <addr>
 *
 * @param addr the address of the region
 * @return 0 on success
 */
static inline int munmap(void *addr) {
	return syscall1(SYSCALL_MUNMAP,(ulong)addr);
}

/**
 * Creates a file in /system/shm/ with given name and opens it with <oflag>. If <oflag> contains
 * IO_CREATE, <mode> is used for the permissions. This file is intended to be mapped with mmap().
 *
 * @param name the filename
 * @param oflag the open flags
 * @param mode the mode to set
 * @return the file descriptor on success
 */
int shm_open(const char *name,int oflag,mode_t mode);

/**
 * Unlinks the file previously created by shm_open.
 *
 * @parma name the filename
 */
int shm_unlink(const char *name);

#ifdef __cplusplus
}
#endif
