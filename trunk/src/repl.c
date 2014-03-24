#include "repl.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libguile.h>
#include <zmq.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

void *NETWORK_CONTEXT;
void *NETWORK_SOCKET;

void initialize_repl()
{
	NETWORK_CONTEXT = zmq_ctx_new();
	NETWORK_SOCKET = zmq_socket(NETWORK_CONTEXT, ZMQ_REP);
	if (zmq_bind(NETWORK_SOCKET, "tcp://*:5555") != 0) {
		log_err("Failed to bind socket 5555");
		exit(1);
	}
}

void update_repl()
{
	char buffer[256] = {0x0};
	if (zmq_recv(NETWORK_SOCKET, buffer, 256, ZMQ_NOBLOCK) != -1) {
		char *s = scm_to_locale_string(scm_object_to_string(scm_c_eval_string(buffer), scm_variable_ref(scm_c_lookup("display"))));
		zmq_send(NETWORK_SOCKET, s, strlen(s), 0);
		free(s);
	}
}
