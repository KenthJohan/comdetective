#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

//https://sigrok.org/wiki/Libserialport
#include <libserialport.h>

#include "argparse.h"


static const char *const usage[] =
{
"comdetective [options] [[--] args]",
"comdetective [options]",
NULL,
};

void sp_exit_on_error (enum sp_return r)
{
	if (r < 0)
	{
		printf ("Error %i %s\n", r, sp_last_error_message ());
		exit (EXIT_FAILURE);
	}
}

void print_devices ()
{
	struct sp_port ** port;
	enum sp_return r;
	r = sp_list_ports (&port);
	sp_exit_on_error (r);
	for (struct sp_port ** p = port; (*p) != NULL; ++p)
	{
		//printf ("q %x\n", *p);
		printf ("       name:  %s\n", sp_get_port_name (*p));
		printf ("description:  %s\n", sp_get_port_description (*p));
	}
}


struct main_context
{
	struct sp_port * port;
	pthread_t thread_writer;
	pthread_t thread_reader;

	int showlist;
	char * devname;
	int baudrate;
};


void * reader (void * arg)
{
	struct main_context * ctx = arg;
	assert (ctx);
	enum sp_return r;
	while (1)
	{
		char buf [1000];
		size_t count = 1000;
		r = sp_blocking_read_next (ctx->port, buf, count, 0);
		sp_exit_on_error (r);
		if (r > 0)
		{
			fwrite (buf, sizeof (char), (size_t)r, stdout);
		}
	}
}

void * writer (void * arg)
{
	struct main_context * ctx = arg;
	assert (ctx);
	char buffer [100];
	int count = 100;
	while (1)
	{
		printf ("\nEnter: ");
		int n = read (STDIN_FILENO, buffer, (unsigned)count);
		if (n < 0)
		{
			perror ("Read failed\n");
			exit (EXIT_FAILURE);
		}
		if (n >= count)
		{
			perror ("Too large input\n");
			exit (EXIT_FAILURE);
		}
		assert (n >= 0 && n < count);
		buffer [n] = '\0';
		//Userinput ends with a linefeed character by pressing enter key.
		//Quit when user presses q + enter.
		if (strcmp (buffer, "q\n") == 0)
		{
			return NULL;
		}
	}
	return NULL;
}


int main (int argc, char const * argv[])
{
	struct main_context ctx = {0};
	ctx.baudrate = 115200;

	setbuf (stdout, NULL);

	//Configure the argparse (ap):
	//Define different program options that the user can input (argc, argv):
	struct argparse ap = {0};
	struct argparse_option ap_opt[] =
	{
		OPT_HELP (),
		OPT_GROUP ("Basic options"),
		OPT_BOOLEAN ('l', "list", &ctx.showlist, "list devices", NULL, 0, 0),
		OPT_STRING ('D', "device", &ctx.devname, "Specify the device, overriding the value given in the configuration file.", NULL, 0, 0),
		OPT_INTEGER ('b', "baudrate", &ctx.baudrate, "Specify the baud rate, overriding the value given in the configuration file.", NULL, 0, 0),
		OPT_END ()
	};
	argparse_init (&ap, ap_opt, usage, 0);
	argparse_describe (&ap, "COM Detetective", "");
	argc = argparse_parse (&ap, argc, argv);
	//Quit when there is an argparse error:
	//Quit when argparse help option is enabled:
	if ((ap_opt [0].flags & OPT_PRESENT) || (ap.flags & ARGPARSE_ERROR_OPT))
	{
		argparse_usage (&ap);
		exit (0);
	}

	if (ctx.showlist)
	{
		print_devices();
	}

	if (ctx.devname)
	{
		enum sp_return r;
		r = sp_get_port_by_name (ctx.devname, &ctx.port);
		sp_exit_on_error (r);
		r = sp_open (ctx.port, SP_MODE_READ_WRITE);
		sp_exit_on_error (r);
		r = sp_set_baudrate (ctx.port, ctx.baudrate);
		sp_exit_on_error (r);
		r = sp_set_parity (ctx.port, SP_PARITY_NONE);
		sp_exit_on_error (r);
		r = sp_set_stopbits (ctx.port, 1);
		sp_exit_on_error (r);
		r = sp_set_bits (ctx.port, 8);
		sp_exit_on_error (r);
		r = sp_set_flowcontrol (ctx.port, SP_FLOWCONTROL_NONE);
		sp_exit_on_error (r);
		pthread_create (&ctx.thread_reader, NULL, reader, &ctx);
	}


	pthread_create (&ctx.thread_writer, NULL, writer, &ctx);

	pthread_join (ctx.thread_writer, NULL);

	return 0;
}