#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#define NNG_STATIC_LIB
#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include <nng/protocol/reqrep0/rep.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/supplemental/util/platform.h>

#include <csc_debug.h>

#include "comdetective.h"



int main (int argc, char const * argv[])
{
	assert (argc);
	assert (argv);
	setbuf (stdout, NULL);

	nng_socket pub;
	int r;

	r = nng_req0_open (&pub);
	ASSERTNNG (r);

	r = nng_dial (pub, ADDRESS, NULL, 0);
	ASSERTNNG (r);

	nng_msleep (200); // give time for connecting threads

	while (1)
	{
		char buffer [100];
		int count = 100;
		fputs ("$ ", stdout);
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
			break;
		}

		r = nng_send (pub, buffer, (size_t)n+1, 0);
		ASSERTNNG (r);
	}

	return 0;
}
