#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#define NNG_STATIC_LIB
#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/supplemental/util/platform.h>

#include <csc_debug.h>

#include "comdetective.h"

#define USERINPUT_LENGTH_MAX 100


int main (int argc, char const * argv[])
{
	assert (argc);
	assert (argv);
	setbuf (stdout, NULL);

	nng_socket sock;
	int r;

	//https://nanomsg.org/gettingstarted/nng/reqrep.html
	//Request/Reply is used for synchronous communications where each question is responded with a single answer,
	//for example remote procedure calls (RPCs).
	r = nng_req0_open (&sock);
	if (r != 0)
	{
		perror (nng_strerror (r));
		exit (EXIT_FAILURE);
	}

	//Dialers initiate a remote connection to a listener.
	//Upon a successful connection being established, they create a pipe, add it to the socket, and then wait for that pipe to be closed.
	//When the pipe is closed, the dialer attempts to re-establish the connection.
	//Dialers will also periodically retry a connection automatically if an attempt to connect asynchronously fails.
	r = nng_dial (sock, ADDRESS, NULL, 0);
	if (r != 0)
	{
		perror (nng_strerror (r));
		exit (EXIT_FAILURE);
	}

	while (1)
	{
		char userinput [USERINPUT_LENGTH_MAX];

		fputs ("$ ", stdout);
		r = read (STDIN_FILENO, userinput, USERINPUT_LENGTH_MAX);
		if (r < 0)
		{
			perror ("Read failed\n");
			exit (EXIT_FAILURE);
		}
		if (r >= USERINPUT_LENGTH_MAX)
		{
			perror ("Too large input\n");
			exit (EXIT_FAILURE);
		}

		//Terminate the userinput string:
		assert (r >= 0 && r < USERINPUT_LENGTH_MAX);
		userinput [r] = '\0';

		//The userinputs contains the linefeed character due to pressing enter key.
		if (strcmp (userinput, "q\n") == 0)
		{
			break;
		}

		//https://nng.nanomsg.org/man/v1.2.2/nng_send.3.html
		//The semantics of what sending a message means vary from protocol to protocol,
		//so examination of the protocol documentation is encouraged.
		size_t n = (size_t)r + 1;
		r = nng_send (sock, userinput, n, 0);
		if (r != 0)
		{
			perror (nng_strerror (r));
			exit (EXIT_FAILURE);
		}
	}

	return 0;
}
