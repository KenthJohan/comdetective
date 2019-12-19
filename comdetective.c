#include <stdio.h>
#include <stdlib.h>

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


int main (int argc, char * argv[])
{
	setbuf (stdout, NULL);

	int opt_list = 0;
	char * opt_devname = NULL;

	//Configure the argparse (ap):
	//Define different program options that the user can input (argc, argv):
	struct argparse ap = {0};
	struct argparse_option ap_opt[] =
	{
		OPT_HELP (),
		OPT_GROUP ("Basic options"),
		OPT_BOOLEAN('l', "list", &opt_list, "list devices", NULL, 0, 0),
		OPT_STRING('D', "device", &opt_devname, "open device name", NULL, 0, 0),
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

	if (opt_list)
	{
		print_devices();
	}

	if (opt_devname)
	{
		enum sp_return r;
		struct sp_port * port;
		r = sp_get_port_by_name (opt_devname, &port);
		sp_exit_on_error (r);
		r = sp_open (port, SP_MODE_READ_WRITE);
		sp_exit_on_error (r);
		r = sp_set_baudrate (port, 115200);
		sp_exit_on_error (r);
		r = sp_set_parity (port, SP_PARITY_NONE);
		sp_exit_on_error (r);
		r = sp_set_stopbits (port, 1);
		sp_exit_on_error (r);
		r = sp_set_bits (port, 8);
		sp_exit_on_error (r);
		r = sp_set_flowcontrol (port, SP_FLOWCONTROL_NONE);
		sp_exit_on_error (r);
		while (1)
		{
			char buf [1000];
			size_t count = 1000;
			r = sp_blocking_read_next (port, buf, count, 0);
			sp_exit_on_error (r);
			if (r > 0)
			{
				fwrite (buf, sizeof (char), r, stdout);
			}
		}
	}

	return 0;
}
