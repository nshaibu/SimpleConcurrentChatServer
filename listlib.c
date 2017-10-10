#define TRY

#include "./include/listlib.h"

static threads_ptr head = NULL;
static threads_ptr last = NULL;
static int list_height = 0;

threads_ptr create_thread_node(pthread_t tid, int user_id /*struct user_inbox ub*/) {
	threads_ptr node = (threads_ptr)malloc(sizeof(threads_info));
	if (node == NULL) return NULL;
	
	node->data = (struct DATA*)malloc(sizeof(struct DATA));
	if (node->data == NULL) return NULL;
	
	node->data->tid = tid;
	node->data->user_id = user_id;
	//node->data->user_inbox = ub;
	node->data->f_kill = 1;
	
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
		printf("%ld\n", curr->data->tid);
		curr = curr->next;
		sleep(1);
	}while (curr != NULL);
}
#endif

void set_thread_socket(threads_ptr node, int socket) { node->data->threads_socket = socket; }
int get_thread_socket(threads_ptr node) { return node->data->threads_socket; }
int get_list_height() { return list_height; }

#ifdef TRY

int main() {
	threads_ptr node;
	for (int i=0; i<20; i++) {
		node = create_thread_node(i, 4);
		insert_thread_node(node);
	}
	
	printf("%d\n", get_list_height() );
	print_list();
	return 0;
}
#endif
