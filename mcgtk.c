#ifndef _MCGTK_C_
#define _MCGTK_C_
#include <linux/videodev2.h>
#include <gdk/gdkkeysyms.h>
#include "mcgtk.h"
#include "mcv4l2.h"
#include "mcsocket.h"
#include "mccodec.h"
#include "mcudp.h"
#include "mcosip.h"

void delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
}

void key_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	switch (event->key.keyval) {		
	case 'q':
	      if (event->key.state == GDK_CONTROL_MASK){
		    gtk_main_quit();
	      } 
	      break;
	default:
	      break;
	}
}

void frame_ready(gpointer data,gint source,GdkInputCondition condition)
{
      if (condition != GDK_INPUT_READ)
	      errno_exit("poll");
      read_frame();
}

void signal_refresh(GtkWidget*widget,gpointer data){
  g_print("refresh ok:\n");
}
void signal_sent_file(GtkWidget*widget,gpointer data){
      g_print("send ok:\n");
}
void signal_chat(GtkWidget*widget,gpointer data){
  g_print("chat ok:\n");
}
void signal_call(GtkWidget*widget,gpointer data){
      gchar *text;
      
      text = gtk_entry_get_text(txt_ip);
      g_print("IP: %s\n",text);
}
void signal_end_call(GtkWidget*widget,gpointer data){
  g_print("ended ok:\n");
}

GtkWidget* create_button(GtkWidget* frame, char* name, int width, int height, int x, int y)
{
      GtkWidget *button;
  
      button=gtk_button_new_with_label(name);
      gtk_widget_set_size_request(button ,width,height);
      gtk_fixed_put(GTK_FIXED(frame), button, x, y);
      
      return button;
}

GtkWidget *create_text_view(GtkWidget *frame,int kt1,int kt2,int x,int y){
  GtkWidget* text_view;
  
  text_view=gtk_text_view_new();
  gtk_fixed_put(GTK_FIXED(frame), text_view, kt1, kt2);
  gtk_widget_set_size_request(text_view, x,y);
  
  return (text_view);
  
}
GtkWidget *create_children_menubar(GtkWidget* menubar,char* name)
{
  GtkWidget * menu_item;
  
  menu_item=gtk_menu_item_new_with_label(name);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menu_item);
  
  return menu_item;  
}

GtkWidget *menubar_create()
{
	GtkWidget *vbox;
	GtkWidget *fileMenu;
	GtkWidget *menubar;
	
	menubar = gtk_menu_bar_new();
	fileMenu = gtk_menu_new();	  
	vbox = gtk_vbox_new(FALSE, 0);
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(create_children_menubar
				  (menubar,"File")), fileMenu);
	
	g_signal_connect(G_OBJECT(create_children_menubar(fileMenu,"Exit")), 
			 "activate",G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(create_children_menubar(fileMenu,"OK")), 
			 "activate",G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(create_children_menubar(fileMenu,"YESs")), 
			 "activate",G_CALLBACK(gtk_main_quit), NULL);
	
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	
	// delete pointers anh return
	return vbox;
}

void ctrl_create(GtkWidget *window)
{
	GtkWidget *frame;
	gtk_signal_connect (GTK_OBJECT (window), "delete_event",
			      GTK_SIGNAL_FUNC (gtk_exit), NULL);
	
	frame = gtk_fixed_new(); 
	gtk_container_add(GTK_CONTAINER(window), frame);
	
	//create buttons and get press event for its. 	
	g_signal_connect(GTK_OBJECT(create_button(frame,"Exit",100,35,320,220)), 
			 "clicked", G_CALLBACK(delete_event), NULL);	
	g_signal_connect(GTK_OBJECT(create_button(frame,"Refresh",100,35,320,180)), 
			 "clicked", G_CALLBACK(signal_refresh), NULL);	
	g_signal_connect(GTK_OBJECT(create_button(frame,"Send File",100,35,320,140)), 
			 "clicked", G_CALLBACK(signal_sent_file), NULL);	
	g_signal_connect(GTK_OBJECT(create_button(frame,"Chat",100,35,320,100)), 
			 "clicked", G_CALLBACK(signal_chat), NULL);
	g_signal_connect(GTK_OBJECT(create_button(frame,"Call",70,60,2,2)), 
			 "clicked", G_CALLBACK(signal_call), NULL);	
	g_signal_connect(GTK_OBJECT(create_button(frame,"Ended",70,60,348,2)), 
			 "clicked", G_CALLBACK(signal_end_call), NULL);
	//create textviews
	txt_main 	= create_text_view(frame,15,90,300,180);
// 	txt_ip 	= Create_text_view(frame,80,30,260,30);
	txt_ip = gtk_entry_new();
	gtk_fixed_put(GTK_FIXED(frame), txt_ip, 80, 30);
	gtk_widget_set_size_request(txt_ip, 260,30);
	gtk_entry_set_text(txt_ip,"sip:PhuTran@192.168.123.100:5060");
	
	//create labels
	gtk_fixed_put(GTK_FIXED(frame), 
		      gtk_label_new("Address IP:..."),175,10);	
	gtk_fixed_put(GTK_FIXED(frame),
		      gtk_label_new("Contact List"),10,70);	
	gtk_fixed_put(GTK_FIXED(frame), 
		      gtk_label_new("Function"),340,75);
		
	gtk_container_add(GTK_CONTAINER(frame), menubar_create());
	
}

GtkWidget* gtk_create_window_call(char* msg)
{
	GtkWidget *window;
	GtkWidget *frame;
	
	frame = gtk_fixed_new(); 	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 350, 100);
	gtk_window_set_title(GTK_WINDOW(window), "Lab411: Call incoming");
// 	gtk_widget_show_all(window);
	gtk_container_add(GTK_CONTAINER(window), frame);
	g_signal_connect(GTK_OBJECT(create_button(frame,"Accept",120,30,30,40)), 
			 "clicked", G_CALLBACK(osip_accept_call),G_OBJECT(window));
	g_signal_connect(GTK_OBJECT(create_button(frame,"Decline",120,30,180,40)), 
			 "clicked", G_CALLBACK(osip_decline_call),G_OBJECT(window));
	gtk_fixed_put(GTK_FIXED(frame), 
		      gtk_label_new("Incoming: "),5,5);	
	gtk_fixed_put(GTK_FIXED(frame), 
		      gtk_label_new(msg),100,5);	
	
	gtk_widget_show_all(window);
	return window;
}

void main_frontend(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *video;

	gtk_init(&argc, &argv);
	gdk_rgb_init();

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 350, 300);
	gtk_window_set_title(GTK_WINDOW(window), "Lab411:Video-Streaming " SVV_VERSION);
	video = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(video), "Lab411:Video-camera");

	gtk_signal_connect(GTK_OBJECT(window), "delete_event",
			   GTK_SIGNAL_FUNC(delete_event), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "destroy",
			   GTK_SIGNAL_FUNC(delete_event), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "key_release_event",
			   GTK_SIGNAL_FUNC(key_event), NULL);

	gtk_container_set_border_width(GTK_CONTAINER(window), 2);

	/* image */
	drawing_area = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_area),
			      fmt.fmt.pix.width, fmt.fmt.pix.height);
	gtk_container_add(GTK_CONTAINER(video), drawing_area);

 	ctrl_create(window);

	gtk_widget_show_all(window);
	gtk_widget_show_all(video);
	
	gdk_input_add(fd,GDK_INPUT_READ,frame_ready,NULL);

	gtk_main();
}
// #endif

#endif