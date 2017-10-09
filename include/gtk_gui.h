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

#if !defined(GTK_GUI_H)
#define GTK_GUI_H

#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>

#define _NETWORK_DATA

#ifdef _NETWORK_DATA
typedef struct _net_data {
	int port;
	char ip_addr[15];
	int sockfd;
	char out_buffer[1024];
} *net_data_ptr, net_data;
#endif

/*Data structure for gui informations. It also contains the 
 *network datastructure which contains general output buffer
*/
typedef struct _gui_info {
	GtkWidget *window;
	GtkWidget *button;
	
	//Data entry fields
	GtkWidget *entry_ip;
	GtkWidget *entry_port;
	GtkWidget *entry_input;
	GtkWidget *entry_output;
	GtkWidget *entry_error;
	GtkTextBuffer *out;
	GtkTextBuffer *err;
	GIOChannel *io_input; //childs input channel
	//Labels
	GtkWidget *label_ip;
	GtkWidget *label_port;
	GtkWidget *label_stat;
	
	//network data 
	net_data_ptr data_net;
} gui_info;

//childs process  infos
enum { CHILD_STDIN, CHILD_STDOUT, CHILD_STDERR };
typedef struct child_info {
	int child_pipe[3];
	int child_pid;
	char *argv[2];
} SERVER;

extern SERVER server_info;

#define destroy_net_data(x) ( free((x)) )

void init_net_data(net_data_ptr data);
int dialog_msg(gui_info *info, const char *errstr, GtkMessageType msgtype, GtkButtonsType btype);

//static functions for widget creation
static GtkEntryBuffer *create_entry_buffer(guint value, gchar *placeholder);
static GtkWidget *create_entry_widget(GtkEntryBuffer *buff, gchar *);
static GtkWidget *create_window_widget(guint width, guint height);
static GtkWidget *create_button_widget(gchar *btype, gchar *, gchar *, void (*callbackfunc)(GtkWidget *, void *), gpointer );
static GtkWidget *create_label_widget(gchar *label, GtkJustification jusfiy);
static GtkWidget *create_text_view_widget(gui_info *info);
static GdkPixbuf *create_pixbuf(const gchar *pixname);
static GtkWidget *create_spin_button(GtkAdjustment *adj, gdouble rate, guint decimal_place, gchar*);

void make_window(gui_info *info, void *);
void refresh_screen(gui_info *info);

static void enter_key_cb(GtkEntry *entry, gpointer data);

#ifdef _PIPE_LISTENERS
	static gboolean pipe_stdout_listener(GIOChannel *src, GIOCondition cond, gpointer data);
	static gboolean pipe_stderr_listener(GIOChannel *src, GIOCondition cond, gpointer data);
	static void clear_child_process(GPid, gint, gpointer);
	
	void write_to_pipe_stdin();
#endif

#endif
