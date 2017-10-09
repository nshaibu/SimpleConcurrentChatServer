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


#define UNIX_OS
#define  VERIFCATION_FUNCTION_POINTERS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include <gdk/gdk.h>
#include <glib.h>
#include "./include/gtk_gui.h"
#include "./include/func_pointer.h"

SERVER server_info;

//callback for listening on ipc to the child process
static gboolean pipe_stdout_listener(GIOChannel *ch, GIOCondition cond, gpointer data_) {
	gchar *string;
	gsize  size;
	gui_info *data = (gui_info*)data_;
	
	if( cond == G_IO_HUP ) {
		g_io_channel_unref( ch );
		return( FALSE );
	}

	g_io_channel_read_line( ch, &string, &size, NULL, NULL );
	if (size >  0) gtk_text_buffer_insert_at_cursor( data->out, string, -1 );
	g_free( string );

	return( TRUE );
}

static gboolean pipe_stderr_listener(GIOChannel *ch, GIOCondition cond, gpointer data_) {
	gchar *string;
	gsize  size;
	gui_info *data = (gui_info*)data_;
	
	if( cond == G_IO_HUP ) {
		g_io_channel_unref( ch );
		return( FALSE );
	}

	g_io_channel_read_line( ch, &string, &size, NULL, NULL );
	if (size > 0) gtk_text_buffer_insert_at_cursor( data->err, string, -1 );
	g_free( string );

	return( TRUE );
}

static void clear_child_process(GPid pid, gint status, gpointer data) {
	g_spawn_close_pid( pid );
}

static void enter_key_cb(GtkEntry *entry, gpointer data) {
	char buff[50];
	gui_info *info = (gui_info*)data;
	
	strcpy(buff, gtk_entry_get_text(entry));
	gtk_entry_set_text(entry, "");
	//g_io_channel_write_chars(info->io_input, buff, sizeof(buff), NULL, NULL);
	if (server_info.child_pid != -1) write(server_info.child_pipe[CHILD_STDIN], buff, sizeof(buff));
}


int dialog_msg(gui_info *info, const char *errstr, GtkMessageType msgtype, GtkButtonsType btype) {
	guint resp;
	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(info->window),
															GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
															msgtype,
															btype,
															(char*)errstr );
															
	resp = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	
	return resp;
}


void network_connect_handler(GtkWidget *win, gpointer data_) {
	gui_info *info=(gui_info*)data_;
	GError *error;
	char tmp[5];
	GIOChannel *io_output, *io_err;
	time_t tim = time(NULL); 
	
	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(win)) ) {
		char *ip = g_strdup_printf("%s", gtk_entry_get_text(GTK_ENTRY(info->entry_ip)) );
		
		/*FIXME if you do not give a correct ip address then use localhost*/
		if ( ip_addr_valid(ip) == -1 ) {
			int resp = dialog_msg(	info, 
							"IP address is wrong. Using IP address 127.0.0.1.", 
							GTK_MESSAGE_INFO,
							GTK_BUTTONS_CLOSE
						);
						
			if (resp == GTK_RESPONSE_CLOSE) gtk_entry_set_text(GTK_ENTRY(info->entry_ip), "127.0.0.1");
		}
		
		//setup argv for the process
		server_info.argv[0] = "./server";
		server_info.argv[1] = (char*)gtk_entry_get_text( GTK_ENTRY(info->entry_ip) );
		server_info.argv[2] = (char*)gtk_entry_get_text( GTK_ENTRY(info->entry_port) );
		server_info.argv[3] = NULL;
		
		if ( !g_spawn_async_with_pipes(	NULL,
													server_info.argv,
													NULL,
													G_SPAWN_DO_NOT_REAP_CHILD,
													NULL, 
													data_,
													&server_info.child_pid,
													&server_info.child_pipe[CHILD_STDIN],
													&server_info.child_pipe[CHILD_STDOUT],
													&server_info.child_pipe[CHILD_STDERR],
													&error
												)
			) {
				g_print("Error: %s %s %d\n", error->message, __FILE__, __LINE__);
				g_error_free(error);
				exit(EXIT_FAILURE);
			}
		
		char *string = g_strdup_printf("%sServer starting...\n", ctime( &tim ) );
		gtk_text_buffer_insert_at_cursor( info->out, string, -1 );
		g_free(string);
		g_free(ip);
		
		/* Add watch function to catch termination of the process. This function
		* will clean any remnants of process. */
		g_child_watch_add( server_info.child_pid, (GChildWatchFunc)clear_child_process, data_ );
			
		#ifdef UNIX_OS
			info->io_input = g_io_channel_unix_new(server_info.child_pipe[CHILD_STDIN]);
			io_output = g_io_channel_unix_new(server_info.child_pipe[CHILD_STDOUT]);
			io_err = g_io_channel_unix_new(server_info.child_pipe[CHILD_STDERR]);
		#else
			info->io_input = g_io_channel_win32_fd(server_info.child_pipe[CHILD_STDIN]);
			io_output = g_io_channel_win32_fd(server_info.child_pipe[CHILD_STDOUT]);
			io_err = g_io_channel_win32_fd(server_info.child_pipe[CHILD_STDERR]);
		#endif
		
		/* Add watches to channels */
		g_io_add_watch( io_output, G_IO_IN | G_IO_HUP, (GIOFunc)pipe_stdout_listener, data_ );
		g_io_add_watch( io_err, G_IO_IN | G_IO_HUP, (GIOFunc)pipe_stderr_listener, data_ );
		
				
		gtk_label_set_label(GTK_LABEL(info->label_stat), "Started");
		gtk_label_set_label(GTK_LABEL(info->label_ip), gtk_entry_get_text(GTK_ENTRY(info->entry_ip)));
		sprintf(tmp, "%g", gtk_spin_button_get_value(GTK_SPIN_BUTTON(info->entry_port)) );
		gtk_label_set_label(GTK_LABEL(info->label_port), tmp);
	}else {
		if ( !kill(server_info.child_pid, SIGTERM) ) {
			//Error handler
		}
		
		server_info.child_pid = -1;
		gtk_label_set_label(GTK_LABEL(info->label_stat), "Not Started");
		gtk_label_set_label(GTK_LABEL(info->label_ip), "000.000.000.000");
		gtk_label_set_label(GTK_LABEL(info->label_port), "0000");
	}
	
}

static GtkWidget *create_spin_button(GtkAdjustment *adj, gdouble rate, guint decimal_place, gchar *tooltip) {
	GtkWidget *spin = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1, 0);
	gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(spin), GTK_UPDATE_IF_VALID);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin), TRUE);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spin), TRUE);
	gtk_widget_set_tooltip_text(spin, tooltip);
	
	return spin;
}

static GtkWidget *create_text_view_widget(gui_info *info) {
	GtkWidget *text_view = gtk_text_view_new();
	info->out = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	info->err = info->out;
	return text_view;
}

static GdkPixbuf *create_pixbuf(const gchar *pixname) {
	GdkPixbuf *pix;
	GError *error = NULL;
	char *name = g_filename_from_utf8(pixname, sizeof(pixname), NULL, NULL, &error);
	if ( !name ) { g_print("%s %s %d\n", error->message, __FILE__, __LINE__); g_error_free(error); }
	
	pix = gdk_pixbuf_new_from_file(name, &error);
	if ( !pix ) { g_print("%s %s %d\n", error->message, __FILE__, __LINE__); g_error_free(error); }
	
	return pix;
}

void init_net_data(net_data_ptr data_) {
	net_data_ptr data = (net_data_ptr)malloc(sizeof(net_data));
	
	if (data == NULL) {
		perror("Data initialization failed");
		exit(EXIT_FAILURE);
	}
	data_ = data;
}

static GtkEntryBuffer *create_entry_buffer(guint value, gchar *placeholder) {
	GtkEntryBuffer *buffer = gtk_entry_buffer_new(placeholder, value);
	gtk_entry_buffer_set_max_length(buffer, value);
	return buffer;
}

static GtkWidget *create_entry_widget(GtkEntryBuffer *buff, gchar *tooltip) {
	GtkWidget *entry = gtk_entry_new_with_buffer(buff);
	gtk_widget_set_tooltip_text(entry, tooltip);
	gtk_entry_set_overwrite_mode(GTK_ENTRY(entry), TRUE);
	return entry;
}

static GtkWidget *create_window_widget(guint width, guint height) {
	GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), "theyChat");
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(win), width, height);
	gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
	
	g_signal_connect(GTK_WINDOW(win), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	return win;
}

static GtkWidget *create_button_widget(gchar *btype, gchar *label, gchar *event, void (*callbackfunc)(GtkWidget *, void*), gpointer data) {
	GtkWidget *widget;
	
	if (btype == "normal") {
		widget = gtk_button_new_with_label(label);
		g_signal_connect(GTK_BUTTON(widget), event, G_CALLBACK(callbackfunc), data);
	} else {
		widget = gtk_toggle_button_new_with_label(label);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);
		g_signal_connect(GTK_TOGGLE_BUTTON(widget), event, G_CALLBACK(callbackfunc), data);
	}
	
	return widget;
}

static GtkWidget *create_label_widget(gchar *label, GtkJustification justify) {
	GtkWidget *widget = gtk_label_new(label);
	gtk_label_set_justify(GTK_LABEL(widget), justify);
	return widget;
}


void make_window(gui_info *info, void *combined) {
	server_info.child_pid = -1;
	
	info->window = create_window_widget(450, 260);
	gtk_window_set_icon(GTK_WINDOW(info->window), create_pixbuf("QBox.png"));
	
	info->label_ip = create_label_widget("000.000.000.000", GTK_JUSTIFY_LEFT);
	info->label_port = create_label_widget("0000", GTK_JUSTIFY_LEFT);
	info->label_stat = create_label_widget("Not started", GTK_JUSTIFY_LEFT);
	
	//first box create top box
	GtkWidget *box_top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	GtkWidget *frame_left = gtk_frame_new("Status");
	gtk_frame_set_shadow_type(GTK_FRAME(frame_left), GTK_SHADOW_ETCHED_OUT);
	
	GtkWidget *top_left_grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(top_left_grid), 3);
	gtk_grid_set_row_spacing(GTK_GRID(top_left_grid), 3);
	
	GtkWidget *ip_label = create_label_widget("IP Address:  ", GTK_JUSTIFY_LEFT);
	gtk_grid_attach(GTK_GRID(top_left_grid), ip_label, 0, 0, 16, 7);
	gtk_grid_attach(GTK_GRID(top_left_grid), info->label_ip, 16, 0, 16, 7);
	
	GtkWidget *port_label = create_label_widget(" Port Number:  ", GTK_JUSTIFY_LEFT);
	gtk_grid_attach(GTK_GRID(top_left_grid), port_label, 0, 7, 16, 7);
	gtk_grid_attach(GTK_GRID(top_left_grid), info->label_port, 16, 7, 16, 7);
	
	GtkWidget *stat_label = create_label_widget("Status:       ", GTK_JUSTIFY_LEFT);
	gtk_grid_attach(GTK_GRID(top_left_grid), stat_label, 0, 14, 16, 7);
	gtk_grid_attach(GTK_GRID(top_left_grid), info->label_stat, 16, 14, 16, 7);
	
	gtk_container_add(GTK_CONTAINER(frame_left), top_left_grid);
	gtk_widget_set_hexpand(GTK_WIDGET(frame_left), TRUE);
	gtk_container_add(GTK_CONTAINER(box_top), frame_left);
	
	//second box create top box
	GtkWidget *frame_right = gtk_frame_new("Network Information");
	GtkEntryBuffer *buffer = create_entry_buffer(15, "000.000.000.000");
	gtk_frame_set_shadow_type(GTK_FRAME(frame_right), GTK_SHADOW_ETCHED_OUT);
	
	GtkWidget *top_right_grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(top_right_grid), 3);
	gtk_grid_set_row_spacing(GTK_GRID(top_right_grid), 3);
		
	ip_label = create_label_widget("IP Address:  ", GTK_JUSTIFY_LEFT);
	info->entry_ip = create_entry_widget(buffer, "Enter the ip address here");
	gtk_grid_attach(GTK_GRID(top_right_grid), ip_label, 0, 0, 16, 7);
	gtk_grid_attach(GTK_GRID(top_right_grid), info->entry_ip, 16, 0, 16, 7);
	
	port_label = create_label_widget("Port Number:  ", GTK_JUSTIFY_LEFT);
	buffer = create_entry_buffer(4, "0000");
	info->entry_port = create_spin_button(gtk_adjustment_new(4020, 1024, 4096, 1, 10, 20), 1, 0, "select the port number to listen on");
	gtk_grid_attach(GTK_GRID(top_right_grid), port_label, 0, 7, 16, 7);
	gtk_grid_attach(GTK_GRID(top_right_grid), info->entry_port, 16, 7, 16, 7);
	
	info->button = create_button_widget("toggle", "Start", "toggled", network_connect_handler, info);
	gtk_grid_attach(GTK_GRID(top_right_grid), info->button, 8, 14, 16, 5);
	
	gtk_container_add(GTK_CONTAINER(frame_right), top_right_grid);
	gtk_container_add(GTK_CONTAINER(box_top), frame_right);
	
	//vertical box packing
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(box), box_top);//add top box to main box
	
	//bottom box 
	GtkWidget *frame_bottom = gtk_frame_new("Messages");//bottom frame
	gtk_frame_set_shadow_type(GTK_FRAME(frame_right), GTK_SHADOW_ETCHED_OUT);
	
	GtkWidget *box_bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	GtkWidget *bottom_grid = gtk_grid_new();
	
	//Out text view is define here
	info->entry_output = create_text_view_widget(info);	//text view for ouput
	GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	gtk_widget_set_size_request(GTK_WIDGET(scroll), 450, 100);
	gtk_container_add(GTK_CONTAINER(scroll), info->entry_output);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(info->entry_output), FALSE);
	//gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(info->entry_input), GTK_WRAP_NONE);
	gtk_grid_attach(GTK_GRID(bottom_grid), scroll, 18, 18, 30, 30);
	//lower input bar 
	GtkWidget *label_in = create_label_widget("INPUT: ", GTK_JUSTIFY_LEFT);
	info->entry_input = gtk_entry_new();//create_entry_widget(create_entry_buffer(500, ""), "Inputs here");
	gtk_widget_set_tooltip_text(GTK_WIDGET(info->entry_input), "Inputs here");
	gtk_entry_set_max_length(GTK_ENTRY(info->entry_input), 50);
	g_signal_connect(info->entry_input, "activate", G_CALLBACK(enter_key_cb), info);
	gtk_grid_attach(GTK_GRID(bottom_grid), label_in, 18, 50, 10, 7);
	gtk_grid_attach(GTK_GRID(bottom_grid), info->entry_input, 28, 50, 10, 7);
	
	gtk_container_add(GTK_CONTAINER(frame_bottom), bottom_grid);
	gtk_container_add(GTK_CONTAINER(box_bottom), frame_bottom);
	gtk_container_add(GTK_CONTAINER(box), box_bottom);//add bottom box to main box
	
	gtk_container_add(GTK_CONTAINER(info->window), box);//add all boxes to main box
}

void refresh_screen(gui_info *info) { gtk_widget_show_all(info->window); }
