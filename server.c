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
#include "./include/listlib.h"

struct sockaddr_in serv_addr; //server address struct
struct sockaddr_in cli_addr; //client address struct

static void *connection_handler(void *data) {
	threads_ptr thread_node = (threads_ptr)data;
	int i = 0, client;
	char buff[50];
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	while (1) {
		sprintf(buff, "client on server are %d", get_list_height());
		send(thread_node->data->threads_socket, buff, sizeof(buff), MSG_DONTWAIT);
		sleep(4);
		
		if (i == 5) break;
		++i;
	}
	shutdown(thread_node->data->threads_socket, 2);
	close(thread_node->data->threads_socket);
	pthread_cancel(pthread_self());
}

net_info_ptr setup_net_data(char *ip, int port) {
	net_info_ptr net_data = (net_info_ptr)malloc( sizeof(net_info) );
	net_data->port = port;
	strcpy(net_data->ip_addr, ip);
	
	return net_data;
}

void make_server(net_info_ptr info, threads_ptr th) {
	threads_ptr thr_node; //Thread info nodes
	
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
	int connection_socket, ret;
	
	while(1) {
		int size = sizeof(cli_addr);
		connection_socket = accept(info->socket, (struct sockaddr*)&cli_addr, (socklen_t*)&size);
		
		pthread_t tid;
		thr_node = create_thread_node();
		
		if ( thr_node != NULL) {
			ret = pthread_create(&tid, NULL, connection_handler, (void*)thr_node); 
			if (ret != 0) {
				fprintf(stderr, "%s %s %d\n", "Failed to start handler", __FILE__, __LINE__);
				free(thr_node->data);
				free(thr_node);
			}else {
				set_thread_id(thr_node, tid);
				set_thread_socket(thr_node, connection_socket);
				
				insert_thread_node(thr_node);
				/*TODO cannot set user_inbox inbox now because I have to know the user identification number
				so that i can retreive the user inbox from the SQL lite server. The userinbox and user id will be 
				set in the connection_handler function*/
			}
		}
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

