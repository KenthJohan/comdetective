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
	enum sp_mode mode;
	pthread_t thread_writer;
	pthread_t thread_reader;

	char * devname;
	int baudrate;
	int bits;
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
	enum sp_return r;
	while (1)
	{
		printf ("\n$ ");
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
		//The userinputs contains the linefeed character due to pressing enter key.
		//Destroy this thread when user presses q + enter.
		if (strcmp (buffer, "q\n") == 0)
		{
			return NULL;
		}

		r = sp_blocking_write (ctx->port, buffer, (size_t)n, 0);
		sp_exit_on_error (r);
	}
	return NULL;
}

#define DESCRIPTION_DEVICE \
"Specify the device, overriding the value given in the configuration file."

#define DESCRIPTION_BAUDRATE \
"Specify the baud rate, overriding the value given in the configuration file."

#define DESCRIPTION_BITS \
"Set the data bits for the specified serial port."

#define DESCRIPTION_5BIT \
"5bit mode for terminals which aren't 8bit capable. 8bit is default if the environment " \
"is configured for this via LANG or LC_ALL, 7bit otherwise."

#define DESCRIPTION_6BIT \
"6bit mode for terminals which aren't 8bit capable. 8bit is default if the environment " \
"is configured for this via LANG or LC_ALL, 7bit otherwise."

#define DESCRIPTION_7BIT \
"7bit mode for terminals which aren't 8bit capable. 8bit is default if the environment " \
"is configured for this via LANG or LC_ALL, 7bit otherwise."

#define DESCRIPTION_8BIT \
"8bit characters pass through without any modification. 'Continuous' means no" \
"locate/attribute control sequences are inserted without real change of" \
"locate/attribute. This mode is to display 8bit multi-byte characters such as" \
"Japanese. Not needed in every language with 8bit characters. (For example  displaying" \
"Finnish text doesn't need this.)"




int main (int argc, char const * argv[])
{
	struct main_context ctx = {0};
	ctx.baudrate = 115200;
	ctx.bits = 8;
	ctx.mode = 0;

	setbuf (stdout, NULL);

	//Configure the argparse (ap):
	//Define different program options that the user can input (argc, argv):
	int bit5 = 0;
	int bit6 = 0;
	int bit7 = 0;
	int bit8 = 1;
	int mode_read = 0;
	int mode_write = 0;
	int showlist = 0;
	struct argparse ap = {0};
	struct argparse_option ap_opt[] =
	{
		OPT_HELP (),
		OPT_GROUP ("Basic options"),
		OPT_BOOLEAN ('l', "list", &showlist, "list devices", NULL, 0, 0),
		OPT_STRING ('D', "device", &ctx.devname, DESCRIPTION_DEVICE, NULL, 0, 0),
		OPT_INTEGER ('b', "baudrate", &ctx.baudrate, DESCRIPTION_BAUDRATE, NULL, 0, 0),
		OPT_INTEGER ('B', "bits", &ctx.bits, DESCRIPTION_BITS, NULL, 0, 0),
		OPT_BOOLEAN ('5', "7bit", &bit5, DESCRIPTION_5BIT, NULL, 0, 0),
		OPT_BOOLEAN ('6', "6bit", &bit6, DESCRIPTION_6BIT, NULL, 0, 0),
		OPT_BOOLEAN ('7', "7bit", &bit7, DESCRIPTION_7BIT, NULL, 0, 0),
		OPT_BOOLEAN ('8', "8bit", &bit8, DESCRIPTION_8BIT, NULL, 0, 0),
		OPT_BOOLEAN ('r', "read", &mode_read, "Read mode", NULL, 0, 0),
		OPT_BOOLEAN ('w', "write", &mode_write, "Write mode", NULL, 0, 0),
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

	if (showlist)
	{
		print_devices();
	}

	if (0) {}
	else if (bit5) {ctx.bits = 5;}
	else if (bit6) {ctx.bits = 6;}
	else if (bit7) {ctx.bits = 7;}
	else if (bit8) {ctx.bits = 8;}

	if (0) {}
	else if (mode_write && mode_read) {ctx.mode = SP_MODE_READ_WRITE;}
	else if (mode_read) {ctx.mode = SP_MODE_READ;}
	else if (mode_write) {ctx.mode = SP_MODE_WRITE;}
	else
	{
		printf ("No write or read mode is enabled.\n");
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
	}

	if (ctx.mode & SP_MODE_READ)
	{
		printf ("thread_reader\n");
		pthread_create (&ctx.thread_reader, NULL, reader, &ctx);
	}

	if (ctx.mode & SP_MODE_WRITE)
	{
		printf ("thread_writer\n");
		pthread_create (&ctx.thread_writer, NULL, writer, &ctx);
	}

	if (ctx.mode & SP_MODE_READ)
	{
		pthread_join (ctx.thread_reader, NULL);
	}

	if (ctx.mode & SP_MODE_WRITE)
	{
		pthread_join (ctx.thread_writer, NULL);
	}



	return 0;
}
