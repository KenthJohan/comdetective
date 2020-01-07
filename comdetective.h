#pragma once

#define ADDRESS "ipc:///comdetective"
#define TOPIC "/writeserial/"

/*
void client ()
{
	nng_socket sub;
	nng_msg *msg;
	char * s;
	int r;

	r = nng_sub_open (&sub);
	ASSERTNNG (r);

	r = nng_dial (sub, ADDRESS, NULL, 0);
	ASSERTNNG (r);

	nng_msleep (200); // give time for connecting threads

	s = "/some/";
	r = nng_setopt (sub, NNG_OPT_SUB_SUBSCRIBE, s, strlen (s));
	ASSERTNNG (r);

	r = nng_setopt_ms (sub, NNG_OPT_RECVTIMEO, NNG_DURATION_INFINITE);
	ASSERTNNG (r);

	r = nng_msg_alloc(&msg, 0);
	ASSERTNNG (r);

	r = nng_recvmsg (sub, &msg, 0);
	ASSERTNNG (r);

	printf ("msg %s\n", nng_msg_body (msg));

	nng_msg_free (msg);
}
*/


