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

#ifndef IO_H_
#define IO_H_

#include <esc/common.h>
#include <esc/fsinterface.h>
#include <stdarg.h>
#include <errors.h>

/* IO flags */
#define IO_ACCESSMODE	   		3
#define IO_READ					1
#define IO_WRITE				2
#define IO_CREATE				4
#define IO_TRUNCATE				8
#define IO_APPEND				16
#define IO_NOBLOCK				32	/* don't block when reading or receiving a msg from drivers */

/* file descriptors for stdin, stdout and stderr */
#define STDIN_FILENO			0
#define STDOUT_FILENO			1
#define STDERR_FILENO			2

/* fcntl-commands */
#define F_GETFL					0
#define F_SETFL					1
#define F_SETDATA				2
#define F_GETACCESS				3

/* seek-types */
#define SEEK_SET				0
#define SEEK_CUR				1
#define SEEK_END				2

/* retry a syscall until it succeded, skipping tries that failed because of a signal */
#define RETRY(expr)				({ \
		int __err; \
		do { \
			__err = (expr); \
		} \
		while(__err == ERR_INTERRUPTED); \
		__err; \
	})

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opens the given path with given mode and returns the associated file-descriptor
 *
 * @param path the path to open
 * @param mode the mode
 * @return the file-descriptor; negative if error
 */
tFD open(const char *path,uint mode) A_CHECKRET;

/**
 * Creates a pipe with 2 separate files for reading and writing.
 *
 * @param readFd will be set to the fd for reading
 * @param writeFd will be set to the fd for writing
 * @return 0 on success
 */
int pipe(tFD *readFd,tFD *writeFd) A_CHECKRET;

/**
 * Retrieves information about the given file
 *
 * @param path the path of the file
 * @param info will be filled
 * @return 0 on success
 */
int stat(const char *path,sFileInfo *info) A_CHECKRET;

/**
 * Retrieves information about the file behind the given file-descriptor
 *
 * @param fd the file-descriptor
 * @param info will be filled
 * @return 0 on success
 */
int fstat(tFD fd,sFileInfo *info) A_CHECKRET;

/**
 * Asks for the current file-position
 *
 * @param fd the file-descriptor
 * @param pos will point to the current file-position on success
 * @return 0 on success
 */
int tell(tFD fd,int *pos) A_CHECKRET;

/**
 * Manipulates the given file-descriptor, depending on the command
 *
 * @param fd the file-descriptor
 * @param cmd the command (F_GETFL or F_SETFL)
 * @param arg the argument (just used for F_SETFL)
 * @return >= 0 on success
 */
int fcntl(tFD fd,uint cmd,int arg);

/**
 * The  isterm()  function  tests  whether  fd  is an open file descriptor
 * referring to a terminal.
 *
 * @param fd the file-descriptor
 * @return true if it referrs to a terminal
 */
bool isterm(tFD fd);

/**
 * Changes the position in the given file
 *
 * @param fd the file-descriptor
 * @param offset the offset
 * @param whence the seek-type: SEEK_SET, SEEK_CUR or SEEK_END
 * @return the new position on success, of the negative error-code
 */
int seek(tFD fd,int offset,uint whence) A_CHECKRET;

/**
 * Reads count bytes from the given file-descriptor into the given buffer and returns the
 * actual read number of bytes. You may be interrupted by a signal (ERR_INTERRUPTED)!
 *
 * @param fd the file-descriptor
 * @param buffer the buffer to fill
 * @param count the number of bytes
 * @return the actual read number of bytes; negative if an error occurred
 */
ssize_t read(tFD fd,void *buffer,size_t count) A_CHECKRET;

/**
 * Writes count bytes from the given buffer into the given fd and returns the number of written
 * bytes.
 *
 * @param fd the file-descriptor
 * @param buffer the buffer to read from
 * @param count the number of bytes to write
 * @return the number of bytes written; negative if an error occurred
 */
ssize_t write(tFD fd,const void *buffer,size_t count) A_CHECKRET;

/**
 * Sends a message to the driver identified by <fd>.
 *
 * @param fd the file-descriptor
 * @param id the msg-id
 * @param msg the message (may be NULL)
 * @param size the size of the message
 * @return 0 on success or < 0 if an error occurred
 */
ssize_t send(tFD fd,tMsgId id,const void *msg,size_t size);

/**
 * Receives a message from the driver identified by <fd>. Blocks if no message is available.
 * You may be interrupted by a signal (ERR_INTERRUPTED)!
 *
 * @param fd the file-descriptor
 * @param id will be set to the msg-id (may be NULL to skip the message)
 * @param msg the message (may be NULL to skip the message)
 * @param size the (max) size of the message
 * @return the size of the message
 */
ssize_t receive(tFD fd,tMsgId *id,void *msg,size_t size) A_CHECKRET;

/**
 * Sends the given data in the data-part of the standard-message. Doesn't wait for the response.
 *
 * @param fd the file-descriptor
 * @param id the msg-id
 * @param data the data to send
 * @param size the size of the data (if too big an error will be reported)
 * @return >= 0 on success
 */
ssize_t sendMsgData(tFD fd,tMsgId id,const void *data,size_t size);

/**
 * Sends a message with given id and optionally data to the given driver. Afterwards it receives
 * the response and returns the result in msg.args.arg1.
 *
 * @param fd the file-descriptor
 * @param id the message-id
 * @param data the data (NULL = no data)
 * @param size if data is not NULL, the size of data
 * @return the result
 */
ssize_t sendRecvMsgData(tFD fd,tMsgId id,const void *data,size_t size);

/**
 * Sends a message with given id and no data to the given driver, receives a message and
 * copies the data-part of size msg.data.arg1 into <data> with at most <size> bytes.
 *
 * @param fd the file-descriptor
 * @param id the msg-id
 * @param data the buffer where to copy the received data to
 * @param size the size of the buffer
 * @return the number of copied bytes on success
 */
ssize_t recvMsgData(tFD fd,tMsgId id,void *data,size_t size) A_CHECKRET;

/**
 * The same as recvMsgData, except that it expects <argc> arguments behind <argc>, which are
 * put in the given order into an argument-message. This message is send to the driver.
 *
 * @param fd the file-descriptor
 * @param id the msg-id
 * @param data the buffer where to copy the received data to
 * @param size the size of the buffer
 * @param argc the number of args
 * @return the number of copied bytes on success
 */
ssize_t vrecvMsgData(tFD fd,tMsgId id,void *data,size_t size,size_t argc,...) A_CHECKRET;

/**
 * Duplicates the given file-descriptor
 *
 * @param fd the file-descriptor
 * @return the error-code or the new file-descriptor
 */
tFD dupFd(tFD fd);

/**
 * Redirects <src> to <dst>. <src> will be closed. Note that both fds have to exist!
 *
 * @param src the source-file-descriptor
 * @param dst the destination-file-descriptor
 * @return the error-code or 0 if successfull
 */
int redirFd(tFD src,tFD dst);

/**
 * Creates a hardlink at <newPath> which points to <oldPath>
 *
 * @param oldPath the link-target
 * @param newPath the link-path
 * @return 0 on success
 */
int link(const char *oldPath,const char *newPath) A_CHECKRET;

/**
 * Unlinks the given path. That means, the directory-entry will be removed and if there are no
 * more references to the inode, it will be removed.
 *
 * @param path the path
 * @return 0 on success
 */
int unlink(const char *path) A_CHECKRET;

/**
 * Creates the given directory. Expects that all except the last path-component exist.
 *
 * @param path the path
 * @return 0 on success
 */
int mkdir(const char *path) A_CHECKRET;

/**
 * Removes the given directory. Expects that the directory is empty (except '.' and '..')
 *
 * @param path the path
 * @return 0 on success
 */
int rmdir(const char *path) A_CHECKRET;

/**
 * Mounts <device> at <path> with fs <type>
 *
 * @param device the device-path
 * @param path the path to mount at
 * @param type the fs-type
 * @return 0 on success
 */
int mount(const char *device,const char *path,uint type) A_CHECKRET;

/**
 * Unmounts the device mounted at <path>
 *
 * @param path the path
 * @return 0 on success
 */
int unmount(const char *path) A_CHECKRET;

/**
 * Writes all dirty objects of the filesystem to disk
 *
 * @return 0 on success
 */
int sync(void) A_CHECKRET;

/**
 * Closes the given file-descriptor
 *
 * @param fd the file-descriptor
 */
void close(tFD fd);

/**
 * Checks wether the given path points to a regular file
 *
 * @param path the (absolute!) path
 * @return true if its a file; false if not or an error occurred
 */
bool is_file(const char *path);

/**
 * Checks wether the given path points to a directory
 *
 * @param path the (absolute!) path
 * @return true if its a directory; false if not or an error occurred
 */
bool is_dir(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* IO_H_ */