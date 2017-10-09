#if !defined(SERVER_H)
#define SERVER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>



struct user_inbox {
	//inbox struct
};

typedef struct data_ {
		pthread_t tid; //thread id
		unsigned int user_id; //user identification number
		struct user_inbox inbox;
		short f_kill; //determine whether to kill thread 
} *DATA_PTR, DATA;

typedef struct threads_node {
	DATA_PTR data;
	struct threads_node *next; //points to next node
} *threads_ptr, threads_info;

typedef struct net_info_ {
	char ip_addr[16];
	int port;
	int socket;
} *net_info_ptr, net_info;

//extern net_info_ptr network_data;

struct sockaddr_in serv_addr;
struct sockaddr_in cli_addr;

static void token_ring_thread(threads_ptr info);
static void *connection_socket(void *);

net_info_ptr setup_net_data(char *ip, int port);
void make_server(net_info_ptr, threads_ptr th);
void close_socket(net_info_ptr);


#endif
