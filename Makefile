# -*-Makefile -*-

CC=gcc
LIBS=-lpthread
GTK_LIBS=`pkg-config --libs gtk+-3.0 --cflags gtk+-3.0`
OBJS=gtk_gui.o network.o func_pointer.o
HDR=./include/gtk_gui.h ./include/network.h ./include/func_pointer.h

chat1:main.c $(HDR) $(OBJS)
	$(CC) -g -o chat1 main.c $(OBJS) $(GTK_LIBS) $(LIBS) $(GTK_THREAD_LIBS)
	
gtk_gui.o:gtk_gui.c ./include/gtk_gui.h
	$(CC) -g -c gtk_gui.c $(GTK_LIBS) $(LIBS) $(GTK_THREAD_LIBS)
	
network.o:network.c ./include/network.h
	$(CC) -g -c network.c $(GTK_LIBS) $(LIBS) $(GTK_THREAD_LIBS)
	
func_pointer.o:func_pointer.c ./include/func_pointer.h
	$(CC) -g -c func_pointer.c $(GTK_LIBS) $(LIBS) $(GTK_THREAD_LIBS)
	
clean:
	rm -rf ./chat1 $(OBJS)
