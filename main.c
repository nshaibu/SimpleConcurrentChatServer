#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

#include "./include/gtk_gui.h"

#include <stdlib.h>


int main(int argc, char **argv) {
	gui_info info;
	
	init_net_data(info.data_net);
	
	//gdk_threads_init();
	//gdk_threads_enter();
	gtk_init(&argc, &argv);
	make_window(&info, NULL);
	
	refresh_screen(&info);
	
	gtk_main();
	//gdk_threads_leave();
	return 0;
}
