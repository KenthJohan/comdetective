/*
MIT License

Copyright (c) 2019 CSC Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "csc_tcol.h"

//https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#ifdef _WIN32
#define __RELATIVE_FILE__ (strrchr("\\" __FILE__, '\\') + 1)
#else
#define __RELATIVE_FILE__ (strrchr("/" __FILE__, '/') + 1)
#endif

//https://gcc.gnu.org/onlinedocs/gcc/Variadic-Macros.html
#define ASSERT(A)              do{if(!(A)){assert_format(__COUNTER__, __RELATIVE_FILE__, __LINE__, __func__, #A, (0), NULL, NULL                );}}while(0)
#define ASSERTF(A, F, ...)     do{if(!(A)){assert_format(__COUNTER__, __RELATIVE_FILE__, __LINE__, __func__, #A, (0), NULL,  (F), ## __VA_ARGS__);}}while(0)
#define ASSERTC(A, C)          do{if(!(A)){assert_format(__COUNTER__, __RELATIVE_FILE__, __LINE__, __func__, #A, (C),   #C, NULL                );}}while(0)
#define ASSERTCF(A, C, F, ...) do{if(!(A)){assert_format(__COUNTER__, __RELATIVE_FILE__, __LINE__, __func__, #A, (C),   #C,  (F), ## __VA_ARGS__);}}while(0)



#define TRACE(F)            trace_format (__COUNTER__, __RELATIVE_FILE__, __LINE__, __func__, (0), NULL,  (F)                )
#define TRACEF(F, ...)     trace_format (__COUNTER__, __RELATIVE_FILE__, __LINE__, __func__, (0), NULL,  (F), ## __VA_ARGS__)
#define TRACECF(C, F, ...) trace_format (__COUNTER__, __RELATIVE_FILE__, __LINE__, __func__, (C),   #C,  (F), ## __VA_ARGS__)

#define TRACE_TCOL_INFO0 TCOL (TCOL_NORMAL, TCOL_YELLOW, TCOL_DEFAULT)
#define TRACE_TCOL_INFO1 TCOL (TCOL_NORMAL, TCOL_WHITE, TCOL_DEFAULT)
#define TRACE_TCOL_INFO2 TCOL (TCOL_NORMAL, TCOL_WHITE, TCOL_DEFAULT)
#define TRACE_TCOL_INFO3 TCOL (TCOL_NORMAL, TCOL_RED, TCOL_DEFAULT)
#define TRACE_TCOL_INFO4 TCOL (TCOL_NORMAL, TCOL_WHITE, TCOL_DEFAULT)
#define TRACE_TCOL_INFO5 TCOL (TCOL_NORMAL, TCOL_WHITE, TCOL_DEFAULT)
#define TRACE_TCOL_INFO6 TCOL (TCOL_NORMAL, TCOL_BLUE, TCOL_DEFAULT)


#define ASSERT_TCOL0 TCOL (TCOL_NORMAL, TCOL_RED, TCOL_DEFAULT)
#define ASSERT_TCOL1 TCOL (TCOL_NORMAL, TCOL_WHITE, TCOL_DEFAULT)
#define ASSERT_TCOL2 TCOL (TCOL_NORMAL, TCOL_WHITE, TCOL_DEFAULT)
#define ASSERT_TCOL3 TCOL (TCOL_BOLD, TCOL_RED, TCOL_DEFAULT)
#define ASSERT_TCOL4 TCOL (TCOL_NORMAL, TCOL_WHITE, TCOL_DEFAULT)
#define ASSERT_TCOL5 TCOL (TCOL_BOLD, TCOL_RED, TCOL_DEFAULT)
#define ASSERT_TCOL6 TCOL (TCOL_BOLD, TCOL_RED, TCOL_DEFAULT)


#define ASSERTNNG(x) ASSERTF(x == 0,"%s",nng_strerror(x))

__attribute__ ((__unused__))
static void assert_format_va
(
	int id,
	char const * file,
	int line,
	char const * fn,
	char const * exp,
	int code,
	char const * scode,
	char const * fmt,
	va_list va
)
{
	fprintf (stderr, ASSERT_TCOL0 "ASSERT " ASSERT_TCOL1 "[%04i]" TCOL_RST " ", id);
	fprintf (stderr, ASSERT_TCOL2 "%s" ASSERT_TCOL3 ":" TCOL_RST, file);
	fprintf (stderr, ASSERT_TCOL4 "%04i" TCOL_RST " in ", line);
	fprintf (stderr, ASSERT_TCOL5 "%s" TCOL_RST " () ", fn);
	fprintf (stderr, ASSERT_TCOL6 "[%s]" TCOL_RST " ", exp);
	if (scode)
	{
		fprintf (stderr, TCOL (TCOL_BOLD, TCOL_BLACK, TCOL_YELLOW) "[%i %s]" TCOL_RST " ", code, scode);
	}
	fprintf (stderr, "[%04i:" TCOL (TCOL_BOLD, TCOL_RED , TCOL_DEFAULT) "%s" TCOL_RST "]: ", errno, strerror (errno));
	vfprintf (stderr, fmt, va);
	fprintf (stderr, "\n");
	fflush (stderr);
}


__attribute__ ((__unused__))
static void assert_format 
(
	int id, 
	char const * file, 
	int line, 
	char const * fn, 
	char const * exp, 
	int code,
	char const * scode,
	char const * fmt, 
	...
)
{
	va_list va;
	va_start (va, fmt);
	assert_format_va (id, file, line, fn, exp, code, scode, fmt, va);
	va_end (va);
	exit (1);
}



__attribute__ ((__unused__))
static void trace_format 
(
	int id, 
	char const * file, 
	int line, 
	char const * fn, 
	int code,
	char const * scode,
	char const * fmt, 
	...
)
{
	va_list list;
	va_start (list, fmt);
	
	fprintf (stderr, TRACE_TCOL_INFO0 "TRACE " TRACE_TCOL_INFO1 "[%04i]" TCOL_RST " ", id);
	fprintf (stderr, TRACE_TCOL_INFO2 "%s" TRACE_TCOL_INFO3 ":" TCOL_RST, file);
	fprintf (stderr, TRACE_TCOL_INFO4 "%04i" TCOL_RST " in ", line);
	fprintf (stderr, TRACE_TCOL_INFO5 "%s" TCOL_RST " ()", fn);
	if (scode)
	{
		fprintf (stderr, TRACE_TCOL_INFO6 " [%i %s]" TCOL_RST " ", code, scode);
	}
	fprintf (stderr, ": ");
	vfprintf (stderr, fmt, list);
	fprintf (stderr, "\n");
	fflush (stderr);
	va_end (list);
}
