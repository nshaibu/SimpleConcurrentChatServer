#ifndef LISTLIB_H
#define LISTLIB_H

#include "server.h"

/*extern threads_ptr head;*/
/*extern threads_ptr last;*/
/*extern int list_height;*/

threads_ptr create_thread_node(pthread_t tid, int user_id, struct user_inbox, short f_kill);
int insert_node(threads_ptr node);
void remove_node(threads_ptr node);
void destroy_thread_node(void);

#if !defined(SET_AND_GETTERS)
#define SET_AND_GETTERS
	
	void set_thread_socket(threads_ptr node, int socket);
	int get_thread_socket(threads_ptr node);
	
	
	int get_list_height();
#endif /*SET_AND_GETTERS*/

#endif
