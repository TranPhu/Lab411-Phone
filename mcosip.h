#ifndef _MCOSIP_H_
#define _MCOSIP_H_
#include <eXosip2/eXosip.h>
#include <osip2/osip_mt.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <string.h> 
#include <arpa/inet.h>
#include "stdint.h"
#include "unistd.h"
#include "common.h"
#include <gtk/gtk.h>


enum _osip_status_call{
      ACCEPTT,
      DECLINE,
      NON
};
typedef enum _osip_status_call osip_status_call;
struct _osip_core{
      int call_id;
      int dialog_id;
      char *identity;  
      char *registerer;  
      char *source_call;  
      char *dest_call;  
      int port;
      eXosip_event_t * je;
      osip_status_call status_call;
};
typedef struct _osip_core osip_core;
osip_core *oc;

bool osip_initial(osip_core *oc);
void osip_uninit(osip_core *oc);
eXosip_event_t* osip_wait_event(int seconds,int mseconds);
int osip_send_request();
int osip_recev_request();
char* osip_get_ipaddr();
void* osip_threads(void* arg);
void osip_accept_call(GtkWidget* widget,gpointer data);
void osip_decline_call(GtkWidget* widget,gpointer data);
int osip_convert_addr(osip_from_t *from,char **str);

#endif