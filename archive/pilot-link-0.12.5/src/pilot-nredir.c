/*
 * $Id: pilot-nredir.c,v 1.54 2006/10/12 14:21:21 desrod Exp $ 
 *
 * pilot-nredir.c: Redirect a connection over the network
 *
 * Copyright (C) 1997, Kenneth Albanowski
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <stdio.h>

#include "pi-dlp.h"
#include "pi-header.h"
#include "pi-userland.h"

int main(int argc, const char *argv[])
{
	int 	c,		/* switch */
		len,
		sd 		= -1,
		netsd 		= -1, /* This is the network socket */
		state;

	size_t	size;

	const char
                *progname = "pi-nredir";

	char
		port2[255] = "net:";

	pi_buffer_t *buffer;

	struct 	NetSyncInfo 	Net;

	poptContext po;

	enum { mode_none=0, mode_net=257 }
		run_mode = mode_none;

	struct poptOption options[] = {
		USERLAND_RESERVED_OPTIONS
		{"net", 'n', POPT_ARG_NONE, NULL, mode_net, "Redirect to net:", NULL},
	    POPT_AUTOHELP    
		POPT_TABLEEND
	};

	po = poptGetContext(progname, argc, argv, options, 0);
	poptSetOtherOptionHelp(po,"\n\n"
	"   Accept connection and redirect via Network Hotsync Protocol.\n"
	"   This will bind your locally connected device to a network port, and\n"
	"   redirect them through the network device to a listening server as\n"
	"   specified in the LANSync Preferences panel on your Palm.\n\n"
	"   Example arguments:\n"
	"      -n -p /dev/pilot\n\n");

	if (argc < 2) {
		poptPrintUsage(po,stderr,1);
		return 1;
	}
	while ((c = poptGetNextOpt(po)) >= 0) {
		switch(c) {
		case mode_net :
			if (mode_none == run_mode) {
				run_mode = c;
			} else {
				if (c != run_mode) {
					fprintf(stderr,"   ERROR: Specify only one mode (-n)\n");
					return 1;
				}
			}
			break;
		default:
			fprintf(stderr,"   ERROR: Unhandled option %d.\n",c);
			return 1;
		}
	}
	if (c < -1) {
		plu_badoption(po,c);
	}
	sd = plu_connect();
	if (sd < 0)
		goto error;

	if (dlp_ReadNetSyncInfo(sd, &Net) < 0) {
		fprintf(stderr,
			"   ERROR: Unable to read network information, cancelling sync.\n");
		goto error_close;
	}

	if (!Net.lanSync) {
		fprintf(stderr,
			"   ERROR: LANSync not enabled on your Palm, cancelling sync.\n");
		goto error_close;
	}

	netsd = pi_socket(PI_AF_PILOT, PI_SOCK_STREAM, PI_PF_NET);
	if (netsd < 0)
		goto error_close;

	strncat(port2, Net.hostAddress, 251 - strlen(Net.hostAddress));

	if (!plu_quiet) {
		printf("\tTrying %s... ", Net.hostAddress);
		fflush(stdout);
	}
	if (pi_connect(netsd, port2) < 0) {
		fprintf(stderr,"   ERROR: Failed to connect to network.\n");
		goto error_close;
	}

	if (!plu_quiet) {
		printf("Connected\n");
		fflush(stdout);
	}

	buffer = pi_buffer_new (0xffff);

	while ((len = pi_read(netsd, buffer, 0xffff)) > 0) {
		pi_write(sd, buffer->data, len);
		buffer->used = 0;
		len = pi_read(sd, buffer, 0xffff);
		if (len < 0)
			break;
		pi_write(netsd, buffer->data, len);
		buffer->used = 0;
	}

	pi_buffer_free (buffer);

	state = PI_SOCK_CONN_END;
	size = sizeof (state);
	pi_setsockopt (sd, PI_LEVEL_SOCK, PI_SOCK_STATE, &state, &size);
	pi_setsockopt (netsd, PI_LEVEL_SOCK, PI_SOCK_STATE, &state, &size);

	pi_close(sd);
	pi_close(netsd);

	return 0;

 error_close:
 	if (netsd >=0) {
		pi_close(netsd);
	}
	pi_close(sd);

 error:
	return -1;
}

/* vi: set ts=8 sw=4 sts=4 noexpandtab: cin */
/* ex: set tabstop=4 expandtab: */
/* Local Variables: */
/* indent-tabs-mode: t */
/* c-basic-offset: 8 */
/* End: */
