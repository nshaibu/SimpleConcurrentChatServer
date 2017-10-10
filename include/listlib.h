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

#ifndef LISTLIB_H
#define LISTLIB_H

#include <stdio.h>
#include <stdlib.h>

#include "server.h"


threads_ptr create_thread_node(pthread_t tid, int user_id /*struct user_inbox,*/);
int insert_thread_node(threads_ptr node);
void remove_thread_node(threads_ptr node);
void destroy_thread_node(void);

#if !defined(SET_AND_GETTERS)
#define SET_AND_GETTERS
	
	void set_thread_socket(threads_ptr node, int socket);
	int get_thread_socket(threads_ptr node);
	
	
	int get_list_height();
#endif /*SET_AND_GETTERS*/

#endif
