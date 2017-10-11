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
	/*
	This inbox location is used to deposit messages if the user is online. It should 
	either be empty or filled with data.i.e if data is read from it the reading thread 
	must set it to zero length to notified token_ring_thread that it can placed new
	messages in it or else token_ring_thread will put it in the offline_inbox_file.
	*/
	char online_inbox[1024];
	
	/*
	Message read from this file must be remove or designated sent.Message must always appended.
	file_content_format:TIME(in unix epoch secs)|THE MESSAGE|remove or designated set
	*/
	char offline_inbox_file[256];
};

struct user_sendbox {
	//messages should be kept at temporary location before it is read by token_ring_thread
}

struct DATA {
		pthread_t tid; //thread id
		
		//user identification number
		unsigned int user_id; 
		
		//user specific inbox
		struct user_inbox *inbox;
		
		/*
		This is used to determine whether to kill thread or not. If is set 
		to 1 it means that the thread is still running. However, if it is zero
		it means the thread has been cancelled and therefore remove the node from
		the list.
		*/
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
