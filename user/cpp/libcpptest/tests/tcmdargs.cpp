/**
 * $Id: tcmdargs.c 696 2010-07-09 13:22:02Z nasmussen $
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

#include <esc/common.h>
#include <cmdargs.h>
#include <esc/test.h>
#include <errors.h>
#include <stdlib.h>
#include <iostream>
#include "tcmdargs.h"

/* forward declarations */
static void test_cmdargs(void);
static void test_flags(void);
static void test_vals(void);
static void test_reqNFree(void);

/* our test-module */
sTestModule tModCmdArgs = {
	"Command arguments",
	&test_cmdargs
};

static void test_cmdargs(void) {
	test_flags();
	test_vals();
	test_reqNFree();
}

static void test_flags(void) {
	u32 before;
	test_caseStart("Testing flags");

	before = heapspace();
	{
		const char * const argv1[] = {"progname",NULL};
		const char * const argv2[] = {"progname","-flag",NULL};
		const char * const argv3[] = {"progname","--flag1x","--flag2",NULL};
		const char * const argv4[] = {"progname","--flag1","--flag2",NULL};
		const char *format = "flag1 flag2";
		bool flag1,flag2;
		cmdargs *a;

		flag1 = flag2 = false;
		a = new cmdargs(ARRAY_SIZE(argv1) - 1,(char* const*)argv1,0);
		a->parse(format,&flag1,&flag2);
		test_assertFalse(flag1);
		test_assertFalse(flag2);
		delete a;

		flag1 = flag2 = false;
		a = new cmdargs(ARRAY_SIZE(argv2) - 1,(char* const*)argv2,0);
		a->parse(format,&flag1,&flag2);
		test_assertFalse(flag1);
		test_assertFalse(flag2);
		delete a;

		flag1 = flag2 = false;
		a = new cmdargs(ARRAY_SIZE(argv3) - 1,(char* const*)argv3,0);
		a->parse(format,&flag1,&flag2);
		test_assertFalse(flag1);
		test_assertTrue(flag2);
		delete a;

		flag1 = flag2 = false;
		a = new cmdargs(ARRAY_SIZE(argv4) - 1,(char* const*)argv4,0);
		a->parse(format,&flag1,&flag2);
		test_assertTrue(flag1);
		test_assertTrue(flag2);
		delete a;
	}
	test_assertUInt(heapspace(),before);

	test_caseSucceded();
}

static void test_vals(void) {
	u32 before;
	test_caseStart("Testing values");

	before = heapspace();
	{
		const char * const argv1[] = {"progname",NULL};
		const char * const argv2[] = {"progname","-a","test","-b=4","-c","-12","-d","0xabc","--long=4",NULL};
		const char * const argv3[] = {"progname","-a=test","--long","444",NULL};
		const char *format = "a=s b=d c=i d=x long=d";
		string a;
		s32 b,c,_long;
		u32 d;
		cmdargs *args;

		a = "";
		b = c = _long = d = 0;
		args = new cmdargs(ARRAY_SIZE(argv1) - 1,(char* const*)argv1,0);
		args->parse(format,&a,&b,&c,&d,&_long);
		test_assertTrue(a == "");
		test_assertInt(b,0);
		test_assertInt(c,0);
		test_assertUInt(d,0);
		test_assertInt(_long,0);
		delete args;

		a = "";
		b = c = _long = d = 0;
		args = new cmdargs(ARRAY_SIZE(argv2) - 1,(char* const*)argv2,0);
		args->parse(format,&a,&b,&c,&d,&_long);
		test_assertStr(a.c_str(),"test");
		test_assertInt(b,4);
		test_assertInt(c,-12);
		test_assertUInt(d,0xabc);
		test_assertInt(_long,4);
		delete args;

		a = "";
		b = c = _long = d = 0;
		args = new cmdargs(ARRAY_SIZE(argv3) - 1,(char* const*)argv3,0);
		args->parse(format,&a,&b,&c,&d,&_long);
		test_assertStr(a.c_str(),"test");
		test_assertInt(b,0);
		test_assertInt(c,0);
		test_assertUInt(d,0);
		test_assertInt(_long,444);
		delete args;
	}
	test_assertUInt(heapspace(),before);

	test_caseSucceded();
}

static void test_reqNFree(void) {
	u32 before;
	test_caseStart("Testing required and free args");

	// one exception before to allocate mem for exception-stuff
	// this way we can test wether everything that we've under control is free'd
	try {
		throw cmdargs_error("");
	}
	catch(...) {
	}

	before = heapspace();
	{
		const char * const argv1[] = {"progname",NULL};
		const char * const argv2[] = {"progname","-b","12",NULL};
		const char * const argv3[] = {"progname","-b=123","--req","val",NULL};
		const char * const argv4[] = {"progname","-b=12","--nreq=f","--req=test","free1","-free2","--req",NULL};
		const char *format = "b=d* req=s* nreq=X";
		bool ex;
		s32 b;
		string req;
		u32 nreq;
		cmdargs *a;

		try {
			b = nreq = ex = 0;
			req = "";
			a = new cmdargs(ARRAY_SIZE(argv1) - 1,(char* const*)argv1,0);
			a->parse(format,&b,&req,&nreq);
		}
		catch(const cmdargs_error& e) {
			ex = true;
		}
		test_assertUInt(a->get_free().size(),0);
		test_assertTrue(ex);
		test_assertTrue(req == "");
		test_assertInt(b,0);
		test_assertUInt(nreq,0);
		delete a;

		/* --- */

		try {
			b = nreq = ex = 0;
			req = "";
			a = new cmdargs(ARRAY_SIZE(argv2) - 1,(char* const*)argv2,0);
			a->parse(format,&b,&req,&nreq);
		}
		catch(const cmdargs_error& e) {
			ex = true;
		}
		test_assertUInt(a->get_free().size(),0);
		test_assertTrue(ex);
		test_assertTrue(req == "");
		test_assertInt(b,12);
		test_assertUInt(nreq,0);
		delete a;

		/* --- */

		try {
			b = nreq = ex = 0;
			req = "";
			a = new cmdargs(ARRAY_SIZE(argv3) - 1,(char* const*)argv3,0);
			a->parse(format,&b,&req,&nreq);
		}
		catch(const cmdargs_error& e) {
			ex = true;
		}
		test_assertUInt(a->get_free().size(),0);
		test_assertFalse(ex);
		test_assertStr(req.c_str(),"val");
		test_assertInt(b,123);
		test_assertUInt(nreq,0);
		delete a;

		/* --- */

		try {
			b = nreq = ex = 0;
			req = "";
			a = new cmdargs(ARRAY_SIZE(argv4) - 1,(char* const*)argv4,0);
			a->parse(format,&b,&req,&nreq);
		}
		catch(const cmdargs_error& e) {
			ex = true;
		}
		const vector<string*>& fargs = a->get_free();
		test_assertUInt(fargs.size(),3);
		test_assertStr(fargs[0]->c_str(),"free1");
		test_assertStr(fargs[1]->c_str(),"-free2");
		test_assertStr(fargs[2]->c_str(),"--req");
		test_assertFalse(ex);
		test_assertStr(req.c_str(),"test");
		test_assertInt(b,12);
		test_assertUInt(nreq,0xf);
		delete a;
	}
	test_assertUInt(heapspace(),before);

	test_caseSucceded();
}