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
#include <esc/debug.h>
#include <esc/io.h>
#include <esc/proc.h>
#include <esc/sllist.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lookup.h"
#include "loader.h"
#include "setup.h"

#define LIB_PATH	"/lib/"

static void load_library(sSharedLib *dst);
static sSharedLib *load_addLib(sSharedLib *lib);
static uintptr_t load_addSeg(int binFd,Elf32_Phdr *pheader,size_t loadSegNo,bool isLib);
static void load_read(int binFd,uint offset,void *buffer,size_t count);

void load_doLoad(int binFd,sSharedLib *dst) {
	Elf32_Ehdr eheader;
	Elf32_Phdr pheader;
	sFileInfo info;
	uint8_t const *datPtr;
	ssize_t textOffset = -1;
	size_t j;

	/* build bindesc */
	if(fstat(binFd,&info) < 0)
		load_error("stat() for binary failed");
	dst->fd = binFd;
	dst->loadAddr = 0;

	/* read header */
	load_read(binFd,0,&eheader,sizeof(Elf32_Ehdr));

	/* check magic-number */
	if(memcmp(eheader.e_ident,ELFMAG,4) != 0)
		load_error("Invalid ELF-magic");

	/* read segments */
	datPtr = (uint8_t const*)(eheader.e_phoff);
	for(j = 0; j < eheader.e_phnum; datPtr += eheader.e_phentsize, j++) {
		/* read pheader */
		load_read(binFd,(uint)datPtr,&pheader,sizeof(Elf32_Phdr));

		/* in shared libraries the text is @ 0x0 (will be relocated anyway), but the entry
		 * DT_STRTAB in the dynamic table is the virtual address, not the file offset. Therefore
		 * we have to add the difference of the text in the file and in virtual memory.
		 * This will be 0 for executables and 0x1000 for shared libraries
		 * (Note that the first load-segment is the text) */
		if(textOffset == -1 && pheader.p_type == PT_LOAD)
			textOffset = pheader.p_offset - pheader.p_vaddr;

		if(pheader.p_type == PT_DYNAMIC) {
			size_t strtblSize,i;
			/* TODO we don't have to read them from file; if we load data and text first, we
			 * already have them */
			/* read dynamic-entries */
			dst->dyn = (Elf32_Dyn*)malloc(pheader.p_filesz);
			if(!dst->dyn)
				load_error("Not enough mem!");
			load_read(binFd,pheader.p_offset,dst->dyn,pheader.p_filesz);

			/* read string-table */
			strtblSize = load_getDyn(dst->dyn,DT_STRSZ);
			dst->dynstrtbl = (char*)malloc(strtblSize);
			if(!dst->dynstrtbl)
				load_error("Not enough mem!");
			load_read(binFd,load_getDyn(dst->dyn,DT_STRTAB) + textOffset,dst->dynstrtbl,strtblSize);

			for(i = 0; i < (pheader.p_filesz / sizeof(Elf32_Dyn)); i++) {
				if(dst->dyn[i].d_tag == DT_NEEDED) {
					sSharedLib *nlib,*lib = (sSharedLib*)malloc(sizeof(sSharedLib));
					if(!lib)
						load_error("Not enough mem!");
					lib->relocated = false;
					lib->initialized = false;
					lib->dyn = NULL;
					lib->dynstrtbl = NULL;
					lib->isDSO = true;
					lib->name = dst->dynstrtbl + dst->dyn[i].d_un.d_val;
					lib->loadAddr = 0;
					lib->deps = sll_create();
					if(!lib->deps)
						load_error("Not enough mem!");
					nlib = load_addLib(lib);
					if(nlib == NULL) {
						load_library(lib);
						nlib = lib;
					}
					else
						free(lib);
					sll_append(dst->deps,nlib);
				}
			}
		}
	}
}

uintptr_t load_addSegments(uint *tlsStart,size_t *tlsSize) {
	sSLNode *n;
	uintptr_t entryPoint = 0;
	*tlsStart = 0;
	*tlsSize = 0;
	for(n = sll_begin(libs); n != NULL; n = n->next) {
		sSharedLib *l = (sSharedLib*)n->data;
		Elf32_Ehdr eheader;
		Elf32_Phdr pheader;
		uint8_t const *datPtr;
		size_t j,loadSeg;

		/* read header */
		load_read(l->fd,0,&eheader,sizeof(Elf32_Ehdr));

		if(!l->isDSO)
			entryPoint = eheader.e_entry;

		loadSeg = 0;
		datPtr = (uint8_t const*)(eheader.e_phoff);
		for(j = 0; j < eheader.e_phnum; datPtr += eheader.e_phentsize, j++) {
			/* read pheader */
			load_read(l->fd,(uint)datPtr,&pheader,sizeof(Elf32_Phdr));
			if(pheader.p_type == PT_LOAD || pheader.p_type == PT_TLS) {
				uintptr_t addr = load_addSeg(l->fd,&pheader,loadSeg,l->isDSO);
				if(addr == 0)
					load_error("Unable to add segment %d (type %d) of DSO %s",j,pheader.p_type,l->name);
				/* store load-address of text */
				if(loadSeg == 0) {
					if(l->isDSO)
						l->loadAddr = addr;
					else
						l->textAddr = addr;
					l->textSize = pheader.p_memsz;
				}
				else if(!l->isDSO && pheader.p_type == PT_TLS) {
					*tlsStart = addr;
					*tlsSize = pheader.p_memsz;
				}
				loadSeg++;
			}
		}

		/* store some shortcuts */
		/* TODO just temporary; later we should access the dynstrtbl always in .text */
		free(l->dynstrtbl);
		l->dynstrtbl = (char*)load_getDyn(l->dyn,DT_STRTAB);
		l->hashTbl = (Elf32_Word*)load_getDyn(l->dyn,DT_HASH);
		l->dynsyms = (Elf32_Sym*)load_getDyn(l->dyn,DT_SYMTAB);
		l->jmprel = (Elf32_Rel*)load_getDyn(l->dyn,DT_JMPREL);
		if(l->dynstrtbl)
			l->dynstrtbl = (char*)((uintptr_t)l->dynstrtbl + l->loadAddr);
		if(l->hashTbl)
			l->hashTbl = (Elf32_Word*)((uintptr_t)l->hashTbl + l->loadAddr);
		if(l->dynsyms)
			l->dynsyms = (Elf32_Sym*)((uintptr_t)l->dynsyms + l->loadAddr);
		if(l->jmprel)
			l->jmprel = (Elf32_Rel*)((uintptr_t)l->jmprel + l->loadAddr);
	}
	return entryPoint;
}

static void load_library(sSharedLib *dst) {
	char path[MAX_PATH_LEN];
	int fd;
	snprintf(path,sizeof(path),"%s%s",LIB_PATH,dst->name);
	fd = open(path,IO_READ);
	if(fd < 0)
		load_error("Unable to open '%s'",path);
	load_doLoad(fd,dst);
}

static sSharedLib *load_addLib(sSharedLib *lib) {
	sSLNode *n;
	for(n = sll_begin(libs); n != NULL; n = n->next) {
		sSharedLib *l = (sSharedLib*)n->data;
		if(strcmp(l->name,lib->name) == 0)
			return l;
	}
	if(!sll_append(libs,lib))
		load_error("Not enough mem!");
	return NULL;
}

static uintptr_t load_addSeg(int binFd,Elf32_Phdr *pheader,size_t loadSegNo,bool isLib) {
	int prot = 0,flags = isLib ? 0 : MAP_FIXED;
	int fd = binFd;
	void *addr;
	size_t memsz = pheader->p_memsz;

	/* check if the sizes are valid */
	if(pheader->p_filesz > pheader->p_memsz)
		return 0;

	/* determine protection flags */
	if(pheader->p_flags & PF_R)
		prot |= PROT_READ;
	if(pheader->p_flags & PF_W)
		prot |= PROT_WRITE;
	if(pheader->p_flags & PF_X)
		prot |= PROT_EXEC;

	/* determine type */
	if(loadSegNo == 0) {
		if(!isLib && (pheader->p_flags != (PF_X | PF_R)))
			return 0;
		/* text regions are shared */
		flags |= MAP_SHARED;
	}
	else if(pheader->p_type == PT_TLS) {
		/* TODO not supported atm */
		if(isLib)
			return 0;
		/* we need the thread-control-block at the end */
		memsz += sizeof(void*);
		/* tls needs no file */
		fd = -1;
		flags &= ~MAP_FIXED;
		flags |= MAP_TLS;
		/* the linker seems to think that readible is enough for TLS. so set the protection explicitly */
		prot = PROT_READ | PROT_WRITE;
	}
	else if(pheader->p_flags == (PF_R | PF_W))
		flags |= MAP_GROWABLE;
	else
		return 0;

	/* add the region */
	addr = mmap((void*)pheader->p_vaddr,pheader->p_memsz,pheader->p_filesz,
			prot,flags,fd,pheader->p_offset);
	if(addr == NULL)
		return 0;
	if(flags & MAP_TLS) {
		/* read tdata and clear tbss */
		load_read(binFd,(uint)pheader->p_offset,addr,pheader->p_filesz);
		memclear((void*)((uintptr_t)addr + pheader->p_filesz),pheader->p_memsz - pheader->p_filesz);
	}
	return (uintptr_t)addr;
}

static void load_read(int binFd,uint offset,void *buffer,size_t count) {
	if(seek(binFd,offset,SEEK_SET) < 0)
		load_error("Unable to seek to %x",offset);
	if(IGNSIGS(read(binFd,buffer,count)) != (ssize_t)count)
		load_error("Unable to read %d bytes @ %x",count,offset);
}
