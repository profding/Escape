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

#include <sys/common.h>
#include <sys/vfs/vfs.h>

/**
 * Creates a server-node
 *
 * @param pid the process-id to use
 * @param parent the parent-node
 * @param name the node-name
 * @param type the device-type
 * @param ops the supported operations
 * @return the node
 */
sVFSNode *vfs_device_create(pid_t pid,sVFSNode *parent,char *name,uint type,uint ops);

/**
 * Tells the server that the given client has been removed. This way, it can reset the internal
 * state that stores which client will be served next.
 *
 * @param node the server-node
 * @param client the client-node
 */
void vfs_device_clientRemoved(sVFSNode *node,const sVFSNode *client);

/**
 * @param node the server-node
 * @param funcs the functions to check
 * @return true if the server supports the given functions
 */
bool vfs_device_supports(const sVFSNode *node,uint funcs);

/**
 * @param node the server-node
 * @param id the msg-id to check
 * @return true if the server accepts the given message
 */
bool vfs_device_accepts(const sVFSNode *node,uint id);

/**
 * @param node the server-node
 * @return true if data can be read from the server (is available)
 */
bool vfs_device_isReadable(const sVFSNode *node);

/**
 * Sets whether data is available
 *
 * @param node the server-node
 * @param readable the new value
 * @return 0 on success
 */
int vfs_device_setReadable(sVFSNode *node,bool readable);

/**
 * Increases the message-count for the given server
 *
 * @param node the server-node
 */
void vfs_device_addMsg(sVFSNode *node);

/**
 * Decreases the message-count for the given server
 *
 * @param node the server-node
 */
void vfs_device_remMsg(sVFSNode *node);

/**
 * @param node the server-node
 * @return true if there is work
 */
bool vfs_device_hasWork(sVFSNode *node);

/**
 * Searches for a client of the given server-node that should be served
 *
 * @param node the server-node
 * @param cont will be set to false (never to true!), if the caller should stop and use service
 * 	the returned client
 * @param retry will be set to true (never to false!), if the caller should check all device-nodes
 * 	again, after the current loop is finished
 * @return the client to serve or NULL if there is none
 */
sVFSNode *vfs_device_getWork(sVFSNode *node,bool *cont,bool *retry);

/**
 * Prints the given server
 *
 * @param n the server-node
 */
void vfs_device_print(sVFSNode *n);