/**
 * Copyright (C) 2012-2015 Yecheng Fu <cofyc.jackson at gmail dot com>
 * All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be found
 * in the LICENSE file.
 */
#pragma once

/* For c++ compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


struct argparse;
struct argparse_option;

typedef void argparse_callback (struct argparse *self, const struct argparse_option *option);


enum argparse_flag
{
	ARGPARSE_STOP_AT_NON_OPTION = 0x01,
	ARGPARSE_UNKNOWN_OPTION = 0x02,
	ARGPARSE_ERROR_OPT = 0x04
};


enum argparse_option_type
{
	/* special */
	ARGPARSE_OPT_END,
	ARGPARSE_OPT_GROUP,
	/* options with no arguments */
	ARGPARSE_OPT_BOOLEAN,
	ARGPARSE_OPT_BIT,
	/* options with arguments (optional or required) */
	ARGPARSE_OPT_INTEGER,
	ARGPARSE_OPT_FLOAT,
	ARGPARSE_OPT_STRING,
};


enum argparse_option_flags
{
	/* disable negation */
	OPT_NONEG = 0x01,
	/* If option is present */
	OPT_PRESENT = 0x02,
	OPT_ERROR = 0x04,
	OPT_LONG = 0x08,
	/* If option value starts with "no-" */
	OPT_UNSET = 0x10,
};

/**
 *  argparse option
 *
 *  `type`:
 *    holds the type of the option, you must have an ARGPARSE_OPT_END last in your
 *    array.
 *
 *  `short_name`:
 *    the character to use as a short option name, '\0' if none.
 *
 *  `long_name`:
 *    the long option name, without the leading dash, NULL if none.
 *
 *  `value`:
 *    stores pointer to the value to be filled.
 *
 *  `help`:
 *    the short help message associated to what the option does.
 *    Must never be NULL (except for ARGPARSE_OPT_END).
 *
 *  `callback`:
 *    function is called when corresponding argument is parsed.
 *
 *  `data`:
 *    associated data. Callbacks can use it like they want.
 *
 *  `flags`:
 *    option flags.
 */
struct argparse_option
{
	enum argparse_option_type type;
	const char short_name;
	const char *long_name;
	void * value;
	/*
	union
	{
		void * value;
		int vali;
		float valf;
		double vald;
		char * vals;
	};
	*/
	const char *help;
	argparse_callback *callback;
	intptr_t data;
	int flags;
};


struct argparse
{
	// user supplied
	struct argparse_option *options;
	int flags;
	// internal context
	int argc;
	const char **argv;
	const char **out;
	int cpidx;
	const char *optvalue;       // current option value
};


// built-in option macros
#define OPT_END()        { ARGPARSE_OPT_END, 0, NULL, NULL, 0, NULL, 0, 0 }
#define OPT_BOOLEAN(...) { ARGPARSE_OPT_BOOLEAN, __VA_ARGS__ }
#define OPT_BIT(...)     { ARGPARSE_OPT_BIT, __VA_ARGS__ }
#define OPT_INTEGER(...) { ARGPARSE_OPT_INTEGER, __VA_ARGS__ }
#define OPT_FLOAT(...)   { ARGPARSE_OPT_FLOAT, __VA_ARGS__ }
#define OPT_STRING(...)  { ARGPARSE_OPT_STRING, __VA_ARGS__ }
#define OPT_GROUP(h)     { ARGPARSE_OPT_GROUP, 0, NULL, NULL, h, NULL, 0, 0 }
#define OPT_HELP()       OPT_BOOLEAN('h', "help", NULL, "show this help message and exit", NULL, 0, OPT_NONEG)


int argparse_init (struct argparse *self, struct argparse_option *options, int flags);
int argparse_parse (struct argparse *self, int argc, const char *argv []);
void argparse_describe (struct argparse *self);
void argparse_usage (const char *const *usages);
void argparse_showvalues (struct argparse *self);

#ifdef __cplusplus
}
#endif
