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

struct DATA {
		pthread_t tid; //thread id
		
		//user identification number
		unsigned int user_id; 
		
		//user specific inbox
		struct user_inbox inbox;
		
		/*This is used to determine whether to kill thread or not. If is set 
		to 1 it means that the thread is still running. However, if it is zero
		it means the thread has been cancelled and therefore remove the node from
		the list.*/
		short f_kill; 
		
		/*The threads private socket*/
		int threads_socket; 
};

typedef struct threads_node {
	struct DATA *data;
	struct threads_node *next; //points to next node
} *threads_ptr, threads_info;

typedef struct net_info_ {
	char ip_addr[16];
	int port;
	int socket;
} *net_info_ptr, net_info;

//extern net_info_ptr network_data;

static void token_ring_thread(threads_ptr info);
static void *connection_socket(void *);

net_info_ptr setup_net_data(char *ip, int port);
void make_server(net_info_ptr, threads_ptr th);
void close_socket(net_info_ptr);


#endif
