#ifndef _MCGTK_H_
#define _MCGTK_H_
#include "mcv4l2.h"
#include "mcsocket.h"

#define WITH_GTK 1		/* gtk+ */

#ifdef WITH_GTK
#include <gtk/gtk.h>
#endif

  GtkWidget *drawing_area;
  short unsigned int SEND,RECEV;

  GtkWidget *txt_main;// 	= Create_text_view(frame,15,90,300,180);
  GtkWidget *txt_ip;// 	= Create_text_view(frame,80,30,260,30);
	
  /**************funtion****************************/
  void delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
  void key_event(GtkWidget *widget, GdkEvent *event, gpointer data);
  void frame_ready(gpointer data,gint source,GdkInputCondition condition);
  void set_ctrl(GtkWidget *widget, gpointer data);
  void toggle_ctrl(GtkWidget *widget, gpointer data);
  void set_qual(GtkWidget *widget, gpointer data);
  void set_parm(GtkWidget *widget, gpointer data);
  void reset_ctrl(GtkButton *b, gpointer data);
  void ctrl_create(GtkWidget *vbox);
  void main_frontend(int argc, char *argv[]);
  void display(gpointer data,gint source,GdkInputCondition condition);
  GtkWidget *create_button(GtkWidget * frame, char* name, int width, int height, int x, int y);
  GtkWidget* gtk_create_window_chat();
  GtkWidget* gtk_create_window_call(char* msg);
  /**************end funtion************************/

#endif