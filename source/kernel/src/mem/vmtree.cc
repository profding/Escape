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

#include <sys/common.h>
#include <sys/mem/vmtree.h>
#include <sys/mem/region.h>
#include <sys/mem/cache.h>
#include <sys/task/proc.h>
#include <sys/vfs/vfs.h>
#include <sys/video.h>
#include <sys/mutex.h>
#include <assert.h>

/**
 * We use a treap (combination of binary tree and heap) to be able to find vm-regions by an address
 * very quickly. To be able to walk through all vm-regions quickly as well, we maintain a linked
 * list of this vm-regions as well.
 */

mutex_t VMTree::regMutex;
VMTree *VMTree::regList;
VMTree *VMTree::regListEnd;

void VMTree::addTree(VirtMem *vm,VMTree *tree) {
	mutex_aquire(&regMutex);
	if(regListEnd)
		regListEnd->next = tree;
	else
		regList = tree;
	regListEnd = tree;
	tree->virtmem = vm;
	tree->next = NULL;
	tree->begin = NULL;
	tree->end = NULL;
	tree->root = NULL;
	tree->priority = 314159265;
	mutex_release(&regMutex);
}

void VMTree::remTree(VMTree *tree) {
	VMTree *t,*p;
	p = NULL;
	mutex_aquire(&regMutex);
	for(t = regList; t != NULL; p = t, t = t->next) {
		if(t == tree) {
			if(p)
				p->next = t->next;
			else
				regList = t->next;
			if(t == regListEnd)
				regListEnd = p;
			break;
		}
	}
	mutex_release(&regMutex);
}

bool VMTree::available(uintptr_t addr,size_t size) const {
	VMRegion *vm;
	for(vm = begin; vm != NULL; vm = vm->next) {
		uintptr_t endaddr = vm->virt + ROUND_PAGE_UP(vm->reg->getByteCount());
		if(OVERLAPS(addr,addr + size,vm->virt,endaddr))
			return false;
	}
	return true;
}

VMRegion *VMTree::getByAddr(uintptr_t addr) const {
	VMRegion *vm;
	for(vm = root; vm != NULL; ) {
		if(addr >= vm->virt && addr < vm->virt + ROUND_PAGE_UP(vm->reg->getByteCount()))
			return vm;
		if(addr < vm->virt)
			vm = vm->left;
		else
			vm = vm->right;
	}
	return NULL;
}

VMRegion *VMTree::getByReg(Region *reg) const {
	VMRegion *vm;
	for(vm = begin; vm != NULL; vm = vm->next) {
		if(vm->reg == reg)
			return vm;
	}
	return NULL;
}

VMRegion *VMTree::add(Region *reg,uintptr_t addr) {
	/* find a place for a new node. we want to insert it by priority, so find the first
	 * node that has <= priority */
	VMRegion *p,**q,**l,**r;
	for(p = root, q = &root; p && p->priority < priority; p = *q) {
		if(addr < p->virt)
			q = &p->left;
		else
			q = &p->right;
	}
	/* create new node */
	*q = (VMRegion*)Cache::alloc(sizeof(VMRegion));
	if(!*q)
		return NULL;
	/* we have a reference to that file now. we'll release it on unmap */
	if(reg->getFile())
		vfs_incRefs(reg->getFile());
	/* fibonacci hashing to spread the priorities very even in the 32-bit room */
	(*q)->priority = priority;
	priority += 0x9e3779b9;	/* floor(2^32 / phi), with phi = golden ratio */
	(*q)->reg = reg;
	(*q)->virt = addr;
	/* At this point we want to split the binary search tree p into two parts based on the
	 * given key, forming the left and right subtrees of the new node q. The effect will be
	 * as if key had been inserted before all of p’s nodes. */
	l = &(*q)->left;
	r = &(*q)->right;
	while(p) {
		if(addr < p->virt) {
			*r = p;
			r = &p->left;
			p = *r;
		}
		else {
			*l = p;
			l = &p->right;
			p = *l;
		}
	}
	*l = *r = NULL;
	p = *q;
	/* insert at the end of the linked list */
	if(end)
		end->next = p;
	else
		begin = p;
	end = p;
	p->next = NULL;
	return p;
}

void VMTree::remove(VMRegion *reg) {
	VMRegion **p,*r,*prev;
	/* find the position where reg is stored */
	for(p = &root; *p && *p != reg; ) {
		if(reg->virt < (*p)->virt)
			p = &(*p)->left;
		else
			p = &(*p)->right;
	}
	assert(*p);
	/* remove from tree */
	doRemove(p,reg);

	/* remove from linked list */
	prev = NULL;
	for(r = begin; r != NULL; prev = r, r = r->next) {
		if(r == reg) {
			if(prev)
				prev->next = r->next;
			else
				begin = r->next;
			if(!r->next)
				end = prev;
			break;
		}
	}
	/* close file */
	if(reg->reg->getFile())
		vfs_closeFile(virtmem->getPid(),reg->reg->getFile());
	Cache::free(reg);
}

void VMTree::doRemove(VMRegion **p,VMRegion *reg) {
	/* two childs */
	if(reg->left && reg->right) {
		/* rotate with left */
		if(reg->left->priority < reg->right->priority) {
			VMRegion *t = reg->left;
			reg->left = t->right;
			t->right = reg;
			*p = t;
			doRemove(&t->right,reg);
		}
		/* rotate with right */
		else {
			VMRegion *t = reg->right;
			reg->right = t->left;
			t->left = reg;
			*p = t;
			doRemove(&t->left,reg);
		}
	}
	/* one child: replace us with our child */
	else if(reg->left)
		*p = reg->left;
	else if(reg->right)
		*p = reg->right;
	/* no child: simply remove us from parent */
	else
		*p = NULL;
}

void VMTree::print() const {
	doPrint(root,0);
	vid_printf("\n");
}

void VMTree::doPrint(const VMRegion *n,int layer) {
	if(n) {
		vid_printf("prio=%08x, addr=%p\n",n->priority,n->virt);
		vid_printf("%*s\\-(l) ",layer * 2,"");
		doPrint(n->left,layer + 1);
		vid_printf("\n");
		vid_printf("%*s\\-(r) ",layer * 2,"");
		doPrint(n->right,layer + 1);
	}
}
