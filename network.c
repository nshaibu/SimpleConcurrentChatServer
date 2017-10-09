#include "./include/gtk_gui.h"
#include "./include/network.h"

#include <string.h>
#include <errno.h>
#include <stdlib.h>

extern int errno;

int create_socket(void /*gui_info*/ *info_) {
	int fd;
	gui_info *info = (gui_info*)info_;
	if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) { return  -1; }
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(info->data_net->port);
	serv_addr.sin_addr.s_addr = inet_addr(info->data_net->ip_addr);
	
	if ( bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 ) { return -1; }
	
	return fd;
}

int accept_connection(void /*gui_info*/ *info_, int socket) {
	gui_info *info = (gui_info*)info_;
	if (listen(socket, SOMAXCONN) < 0) { return -1; }
	
	int size = sizeof(cli_addr);
	return accept(socket, (struct sockaddr*)&cli_addr, (socklen_t*)&size);
}

void close_socket(int socket) { close(socket); }
