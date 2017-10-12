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

//#define TRY

#include "./include/listlib.h"

static threads_ptr head = NULL;
static threads_ptr last = NULL;
static int list_height = 0;

threads_ptr create_thread_node(void) {
	threads_ptr node = (threads_ptr)malloc(sizeof(threads_info));
	if (node == NULL) return NULL;
	
	node->data = (struct DATA*)malloc(sizeof(struct DATA));
	if (node->data == NULL) {
		free(node);
		return NULL;
	}
	
	return node;
}

int insert_thread_node(threads_ptr node) {
	if (list_height == 0) {
		head = last = node;
		node->next = head;
		
		++list_height;
		return 0;
	}
	
	last->next = node;
	node->next = head;
	last = node;
	++list_height;
	return 0;
}

void remove_thread_node(threads_ptr node) {
	threads_ptr prev = NULL;
	
	if (node == head) {
		--list_height;
		head = node->next;
		last->next = head;
		
		free(node->data);
		free(node);
	} else {
		prev = node;
		while (prev->next != node) prev = prev->next;
		prev->next = node->next;
		
		if (node == last) last = prev;
		free(node->data);
		free(node);
		--list_height;
	}
}

void destroy_thread_node(void) {
	for (threads_ptr curr = head;;) {
		threads_ptr tmp = curr->next;
		free(curr->data);
		free(curr);
		curr = tmp;
		
		--list_height;
		if (list_height <= 0) break;
	}
}

#ifdef TRY
void print_list() {
	threads_ptr curr = head;
	do{
		printf("%ld ", curr->data->tid);
		curr = curr->next;
		sleep(0.5);
	}while (curr != head);
}
#endif

/*set and get socket for connection*/
void set_thread_socket(threads_ptr node, int socket) { node->data->threads_socket = socket; }
const int get_thread_socket(threads_ptr node) { return node->data->threads_socket; }
/*Get list height*/
int get_list_height() { return list_height; }
/* set and get thread id */
void set_thread_id(threads_ptr node, pthread_t id) { 	node->data->tid = id; }
const int get_thread_id(threads_ptr node) { return node->data->tid; }
/*set and get user id */
const int get_thread_uid(threads_ptr node) { return node->data->user_id; }
void set_thread_uid(threads_ptr node, int uid) { node->data->user_id = uid; }
/*set and get user in box*/
struct user_inbox* get_thread_userinbox(threads_ptr node) { return node->data->inbox; }
void set_thread_userinbox(threads_ptr node, struct user_inbox *inbox) { node->data->inbox = inbox; }
/*get new head and tail of the list*/
threads_ptr get_thread_list_head() { return head; }
threads_ptr get_thread_list_last() { return last; }
/*set and get f_kill*/
void set_thread_fkill(threads_ptr node, short flag) { node->data->f_kill = flag; }
const int get_thread_fkill(threads_ptr node) { return node->data->f_kill; }

#ifdef TRY

int main() {
	threads_ptr node, me, me1;
	for (int i=0; i<10; i++) {
		node = create_thread_node(i, 4);
		insert_thread_node(node);
		if (i == 5) me = node;
		if (i == 6) me1 = node;
	}
	
	printf("%d\n", get_list_height() );
	print_list();
	remove_thread_node(me);
	remove_thread_node(me1);
	printf("\n");
	print_list();
	printf("%d\n", get_list_height() );
	destroy_thread_node();
	return 0;
}
#endif
