/*===========================================================================================
# Copyright (C) 2017 Nafiu Shaibu.
# Purpose: 
#-------------------------------------------------------------------------------------------
# This is a free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your option) 
# any later version.

# This is distributed in the hopes that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
# Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

#===========================================================================================*/

#define _TRY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#include "./include/server.h"


static void *connection_handler(void *data) {
	int *fd = (int*)data;
	int i = 0;
	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	while (1) {
		send(*fd, "hello world\n", 11, MSG_DONTWAIT);
		sleep(4);
		
		if (i == 5) break;
		++i;
	}
	shutdown(*fd, 2);
	close(*fd);
	pthread_cancel(pthread_self());
}

net_info_ptr setup_net_data(char *ip, int port) {
	net_info_ptr net_data = (net_info_ptr)malloc( sizeof(net_info) );
	net_data->port = port;
	strcpy(net_data->ip_addr, ip);
	
	return net_data;
}

void make_server(net_info_ptr info, threads_ptr th) {
	if ( (info->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) { 
		fprintf(stderr, "%s\n", "[x]Server failed to start");
		exit(EXIT_FAILURE);
	}
	else fprintf(stdout, "%d:%s\n", getpid(), "Creating socket..." );
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(info->port);
	serv_addr.sin_addr.s_addr = inet_addr(info->ip_addr);
	
	
	if ( bind(info->socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 ) {
		fprintf(stderr, "%s\n", "[X]Binding to socket failed...");
		exit(EXIT_FAILURE);
	}
	else fprintf(stdout, "%d:%s\n", getpid(), "Given socket a name...");
	
	listen(info->socket, SOMAXCONN);
	int i=0, connection_socket[SOMAXCONN];
	
	while(1) {
		int size = sizeof(cli_addr);
		connection_socket[i] = accept(info->socket, (struct sockaddr*)&cli_addr, (socklen_t*)&size);
		pthread_t tid;
		
		if ( pthread_create(&tid, NULL, connection_handler, (void*)&connection_socket[i]) ) 
			fprintf(stderr, "%s %s %d\n", "Failed to start handler", __FILE__, __LINE__);
		i++;
	}
}


#ifdef _TRY
	threads_info th;
	
	int main(int argc, char **argv) {
	
		net_info_ptr net = setup_net_data(argv[1], atoi(argv[2]));
		make_server(net, &th);
		
		return 0;
	}
#endif

