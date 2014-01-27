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

#pragma once

#include <esc/common.h>
#include <esc/syscalls.h>
#include <esc/io.h>

/* the events we can wait for */
#define EV_NOEVENT				0			/* just wakeup on signals */
#define EV_CLIENT				1	/* if there's a client to be served */
#define EV_RECEIVED_MSG			2	/* if a device has a msg for us */
#define EV_DATA_READABLE		3	/* if we can read from a device (data available) */
#define EV_USER1				7	/* an event we can send */
#define EV_USER2				8	/* an event we can send */

/* the thread-entry-point-function */
typedef int (*fThreadEntry)(void *arg);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @return the id of the current thread
 */
static inline tid_t gettid(void) {
	return syscall0(SYSCALL_GETTID);
}

/**
 * @return the number of threads in the current process
 */
static inline size_t getthreadcnt(void) {
	return syscall0(SYSCALL_GETTHREADCNT);
}

/**
 * Starts a new thread
 *
 * @param entryPoint the entry-point of the thread
 * @param arg the argument to pass (just the pointer)
 * @return the new tid, < 0 if failed
 */
int startthread(fThreadEntry entryPoint,void *arg) A_CHECKRET;

/**
 * The syscall exit
 *
 * @param errorCode the error-code for the parent
 */
A_NORETURN static inline void _exit(int exitCode) {
	syscall1(SYSCALL_EXIT,exitCode);
	A_UNREACHED;
}

/**
 * @return the cpu-cycles of the current thread
 */
static inline uint64_t getcycles(void) {
	uint64_t res;
	syscall1(SYSCALL_GETCYCLES,(ulong)&res);
	return res;
}

/**
 * Releases the CPU (reschedule)
 */
static inline void yield(void) {
	syscall0(SYSCALL_YIELD);
}

/**
 * Notifies the thread in <msecs> milliseconds via signal (SIG_ALARM).
 *
 * @param msecs the number of milliseconds
 * @return 0 on success
 */
static inline int alarm(time_t msecs) {
	return syscall1(SYSCALL_ALARM,msecs);
}

/**
 * Puts the current thread to sleep for <msecs> milliseconds. If interrupted, -EINTR
 * is returned.
 *
 * @param msecs the number of milliseconds to wait
 * @return 0 on success
 */
static inline int sleep(time_t msecs) {
	return syscall1(SYSCALL_SLEEP,msecs);
}

/**
 * The same as wait(), but waits at most <max> milliseconds.
 *
 * @param event the event to wait for
 * @param object the object to wait for
 * @param max the maximum number of milliseconds to wait (0 = unlimited)
 * @return 0 on success and a negative error-code if failed
 */
static inline int waituntil(uint event,evobj_t object,time_t max) {
	return syscall3(SYSCALL_WAIT,event,object,max);
}

/**
 * Waits until <event> occurs for <object>. If <object> is 0, it will wakeup as soon as <event>
 * occurs for any object.
 *
 * @param event the event to wait for
 * @param object the object to wait for
 * @return 0 on success and a negative error-code if failed
 */
static inline int wait(uint event,evobj_t object) {
	return waituntil(event,object,0);
}

/**
 * Notifies the given thread about the given events. If it was waiting for them, it will be
 * waked up.
 *
 * @param tid the thread-id to notify
 * @param events the events on which you want to wake up
 * @return 0 on success and a negative error-code if failed
 */
static inline int notify(tid_t tid,uint events) {
	return syscall2(SYSCALL_NOTIFY,tid,events);
}

/**
 * Joins a thread, i.e. it waits until a thread with given tid has died (from the own process)
 *
 * @param tid the thread-id (0 = wait until all other threads died)
 * @return 0 on success
 */
static inline int join(tid_t tid) {
	return syscall1(SYSCALL_JOIN,tid);
}

/**
 * Suspends a thread from the own process. That means it is blocked until resume() is called.
 *
 * @param tid the thread-id
 * @return 0 on success
 */
static inline int suspend(tid_t tid) {
	return syscall1(SYSCALL_SUSPEND,tid);
}

/**
 * Resumes a thread from the own process that has been suspended previously
 *
 * @param tid the thread-id
 * @return 0 on success
 */
static inline int resume(tid_t tid) {
	return syscall1(SYSCALL_RESUME,tid);
}

/**
 * Sets the thread-value with given key to given value (for the current thread)
 *
 * @param key the key
 * @param val the value
 * @return true if successfull
 */
bool setThreadVal(uint key,void *val);

/**
 * Retrieves the value of the thread-value with given key (for the current thread)
 *
 * @param key the key
 * @return the value or NULL if not found
 */
void *getThreadVal(uint key);

#ifdef __cplusplus
}
#endif
