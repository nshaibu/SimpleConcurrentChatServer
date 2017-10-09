#if !defined(NETWORK_H)
#define NETWORK_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <unistd.h>
#include "gtk_gui.h"

struct sockaddr_in serv_addr;
struct sockaddr_in cli_addr;

int create_socket(void /*gui_info*/ *info);
int accept_connection(void /*gui_info*/ *info, int socket);

void close_socket(int socket);

#endif
