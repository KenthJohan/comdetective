/**
 * Copyright (C) 2012-2015 Yecheng Fu <cofyc.jackson at gmail dot com>
 * All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be found
 * in the LICENSE file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "argparse.h"


static const char * prefix_skip (const char *str, const char *prefix)
{
	size_t len = strlen (prefix);
	int diff = strncmp (str, prefix, len);
	char const * r = diff ? NULL : str + len;
	return r;
}


static int prefix_cmp (const char *str, const char *prefix)
{
	while (1)
	{
		if (!*prefix) {return 0;}
		else if (*str != *prefix)
		{
			return (unsigned char)*prefix - (unsigned char)*str;
		}
		str++;
		prefix++;
	}
}


static void argparse_error (struct argparse *self, struct argparse_option *opt, const char *reason)
{
	(void)self;
	if (opt->flags & OPT_LONG)
	{
		fprintf (stderr, "error: option `--%s` %s\n", opt->long_name, reason);
	}
	else
	{
		fprintf (stderr, "error: option `-%c` %s\n", opt->short_name, reason);
	}
	opt->flags |= OPT_ERROR;
	self->flags |= ARGPARSE_ERROR_OPT;
}


static void argparse_getvalue (struct argparse *self, struct argparse_option *opt)
{
	char *s = NULL;
	if (!opt->value) {goto skipped;}
	switch (opt->type)
	{
	case ARGPARSE_OPT_BOOLEAN:
		if (opt->flags & OPT_UNSET)
		{
			*(int *)opt->value = *(int *)opt->value - 1;
		}
		else
		{
			*(int *)opt->value = *(int *)opt->value + 1;
		}
		if (*(int *)opt->value < 0)
		{
			*(int *)opt->value = 0;
		}
		break;
	case ARGPARSE_OPT_BIT:
		if (opt->flags & OPT_UNSET)
		{
			*(int *)opt->value &= ~opt->data;
		}
		else
		{
			*(int *)opt->value |= opt->data;
		}
		break;
	case ARGPARSE_OPT_STRING:
		if (self->optvalue)
		{
			*(const char **)opt->value = self->optvalue;
			self->optvalue = NULL;
		}
		else if (self->argc > 1)
		{
			self->argc--;
			*(const char **)opt->value = *++self->argv;
		}
		else
		{
			//*(const char **)opt->value = NULL;
			argparse_error (self, opt, "requires a value");
		}
		break;
	case ARGPARSE_OPT_INTEGER:
		errno = 0;
		if (self->optvalue == NULL && self->argc > 1)
		{
			self->optvalue = *++self->argv;
			self->argc--;
		}
		if (self->optvalue == NULL)
		{
			argparse_error (self, opt, "requires a integer value");
			break;
		}
		if (self->optvalue[0] == '\0')
		{
			argparse_error (self, opt, "requires a integer value");
			break;
		}

		*(int *)opt->value = strtol (self->optvalue, &s, 0);
		self->optvalue = NULL;

		if (errno)
		{
			argparse_error (self, opt, strerror (errno));
			break;
		}
		assert (s);
		if (s[0] != '\0')
		{
			argparse_error (self, opt, "expects an integer value");
			break;
		}
		break;
	case ARGPARSE_OPT_FLOAT:
		errno = 0;
		if (self->optvalue == NULL && self->argc > 1)
		{
			self->optvalue = *++self->argv;
			self->argc--;
		}
		if (self->optvalue == NULL)
		{
			argparse_error (self, opt, "requires a numerical value");
			break;
		}
		if (self->optvalue[0] == '\0')
		{
			argparse_error (self, opt, "requires a numerical value");
			break;
		}

		*(float *)opt->value = strtof(self->optvalue, (char **)&s);
		self->optvalue = NULL;

		if (errno)
		{
			argparse_error (self, opt, strerror(errno));
			break;
		}
		assert (s);
		if (s[0] != '\0')
		{
			argparse_error (self, opt, "expects a numerical value");
			break;
		}
		break;
	default:
		assert(0);
	}

skipped:
	if (opt->callback)
	{
		opt->callback (self, opt);
	}
}


static void argparse_options_check (struct argparse *self, const struct argparse_option options [])
{
	assert (self);
	while (1)
	{
		if (options == NULL) {break;}
		if (options->type == ARGPARSE_OPT_END) {break;}
		switch (options->type)
		{
		case ARGPARSE_OPT_END:
		case ARGPARSE_OPT_BOOLEAN:
		case ARGPARSE_OPT_BIT:
		case ARGPARSE_OPT_INTEGER:
		case ARGPARSE_OPT_FLOAT:
		case ARGPARSE_OPT_STRING:
		case ARGPARSE_OPT_GROUP:
			break;
		default:
			fprintf (stderr, "wrong option type: %d", options->type);
			break;
		}
		options++;
	}
}


static void argparse_short_opt (struct argparse *self, struct argparse_option options [])
{
	assert (self);
	assert ((self->flags & ARGPARSE_UNKNOWN_OPTION) == 0);
	while (1)
	{
		if (options == NULL) {break;}
		if (options->type == ARGPARSE_OPT_END) {break;}
		assert (self->optvalue);
		if (options->short_name == *self->optvalue)
		{
			options->flags |= OPT_PRESENT;
			self->optvalue = self->optvalue[1] ? self->optvalue + 1 : NULL;
			argparse_getvalue (self, options);
			return;
		}
		options++;
	}
	self->flags |= ARGPARSE_UNKNOWN_OPTION;
}


static void argparse_long_opt (struct argparse *self, struct argparse_option options [])
{
	assert (self);
	assert ((self->flags & ARGPARSE_UNKNOWN_OPTION) == 0);
	for (;;options++)
	{
		if (options == NULL) {break;}
		if (options->type == ARGPARSE_OPT_END) {break;}
		if (!options->long_name) {continue;}
		//--attribute=value
		//--(name)(=value)
		//--(name)(rest)
		//The (name) is after two dashes ("--") thus add by 2 characters:
		char const * name = self->argv[0] + 2;
		//The (prefix_skip) will return pointer directly after (name) i.e. (rest):
		char const * rest = prefix_skip (name, options->long_name);
		if (rest == NULL)
		{
			// negation disabled?
			if (options->flags & OPT_NONEG)
			{
				continue;
			}
			// only OPT_BOOLEAN/OPT_BIT supports negation
			if (options->type != ARGPARSE_OPT_BOOLEAN && options->type != ARGPARSE_OPT_BIT)
			{
				continue;
			}

			//If option value does not starts with "no-"
			if (prefix_cmp (name, "no-"))
			{
				continue;
			}
			//The name is after "no-" thus add by 3 characters:
			rest = prefix_skip (name + 3, options->long_name);
			if (!rest) {continue;}
			options->flags |= OPT_UNSET;
		}
		if (*rest)
		{
			if (*rest != '=') {continue;}
			self->optvalue = rest + 1;
		}
		options->flags |= OPT_LONG;
		options->flags |= OPT_PRESENT;
		argparse_getvalue (self, options);
		return;
	}
	self->flags |= ARGPARSE_UNKNOWN_OPTION;
}


int argparse_init (struct argparse *self, struct argparse_option *options, int flags)
{
	assert (self);
	memset (self, 0, sizeof(*self));
	self->options = options;
	self->flags = flags;
	return 0;
}


int argparse_parse (struct argparse *self, int argc, const char *argv [])
{
	assert (self);
	self->argc = argc - 1;
	self->argv = argv + 1;
	self->out = argv;

	argparse_options_check (self, self->options);

	for (; self->argc; self->argc--, self->argv++)
	{
		const char *arg = self->argv[0];
		if (arg[0] != '-' || !arg[1])
		{
			if (self->flags & ARGPARSE_STOP_AT_NON_OPTION)
			{
				goto end;
			}
			// if it's not option or is a single char '-', copy verbatim
			self->out[self->cpidx++] = self->argv[0];
			continue;
		}
		// short option
		if (arg[1] != '-')
		{
			self->optvalue = arg + 1;
			argparse_short_opt (self, self->options);
			if (self->flags & ARGPARSE_UNKNOWN_OPTION) {goto unknown;}
			while (self->optvalue)
			{
				argparse_short_opt (self, self->options);
				if (self->flags & ARGPARSE_UNKNOWN_OPTION) {goto unknown;}
			}
			continue;
		}
		// if '--' presents
		if (!arg[2])
		{
			self->argc--;
			self->argv++;
			break;
		}
		// long option
		argparse_long_opt (self, self->options);
		if (self->flags & ARGPARSE_UNKNOWN_OPTION) {goto unknown;}
		continue;
unknown:
		fprintf (stdout, "error: unknown option `%s`\n", self->argv[0]);
		argparse_describe (self);
		exit (1);
	}

end:
	memmove (self->out + self->cpidx, self->argv, (size_t)self->argc * sizeof(*self->out));
	self->out[self->cpidx + self->argc] = NULL;
	return self->cpidx + self->argc;
}


void argparse_usage (const char *const *usages)
{
	if (usages == NULL)
	{
		fprintf (stdout, "Usage:\n");
		return;
	}
	fprintf (stdout, "Usage: %s\n", *usages);
	usages++;
	while (*usages && **usages)
	{
		fprintf (stdout, "   or: %s\n", *usages);
		usages++;
	}
}


void argparse_describe (struct argparse *self)
{
	assert (self);
	fputc ('\n', stdout);
	for (const struct argparse_option *options = self->options; options->type != ARGPARSE_OPT_END; options++)
	{
		switch (options->type)
		{
		case ARGPARSE_OPT_FLOAT:
			fprintf (stdout, " -%c, --%-20.20s %s\n", options->short_name, options->long_name, options->help);
			break;
		case ARGPARSE_OPT_INTEGER:
			fprintf (stdout, " -%c, --%-20.20s %s\n", options->short_name, options->long_name, options->help);
			break;
		case ARGPARSE_OPT_STRING:
			fprintf (stdout, " -%c, --%-20.20s %s\n", options->short_name, options->long_name, options->help);
			break;
		case ARGPARSE_OPT_GROUP:
			fprintf (stdout, "\n%s\n", options->help);
			break;
		case ARGPARSE_OPT_BOOLEAN:
			if (options->value)
			{
				fprintf (stdout, " -%c, --%-20.20s %s\n", options->short_name, options->long_name, options->help);
			}
			else
			{
				fprintf (stdout, " -%c, --%-20.20s\n", options->short_name, options->long_name);
			}
			break;
		case ARGPARSE_OPT_BIT:
			break;
		case ARGPARSE_OPT_END:
			break;
		}
	}
}


void argparse_showvalues (struct argparse *self)
{
	assert (self);
	fputc ('\n', stdout);
	for (const struct argparse_option *options = self->options; options->type != ARGPARSE_OPT_END; options++)
	{
		switch (options->type)
		{
		case ARGPARSE_OPT_FLOAT:
			fprintf (stdout, " -%c, --%-20.20s = %-30f\n", options->short_name, options->long_name, *(double*)options->value);
			break;
		case ARGPARSE_OPT_INTEGER:
			fprintf (stdout, " -%c, --%-20.20s = %-30i\n", options->short_name, options->long_name, *(int*)options->value);
			break;
		case ARGPARSE_OPT_STRING:
			fprintf (stdout, " -%c, --%-20.20s = %-30s\n", options->short_name, options->long_name, *(char**)options->value);
			break;
		case ARGPARSE_OPT_GROUP:
			fprintf (stdout, "\n%s\n", options->help);
			break;
		case ARGPARSE_OPT_BOOLEAN:
			if (options->value)
			{
				fprintf (stdout, " -%c, --%-20.20s = %-30i\n", options->short_name, options->long_name, *(int*)options->value);
			}
			else
			{
				fprintf (stdout, " -%c, --%-20.20s\n", options->short_name, options->long_name);
			}
			break;
		case ARGPARSE_OPT_BIT:
			break;
		case ARGPARSE_OPT_END:
			break;
		}
	}
}
