#ifndef _MCOSIP_C_
#define _MCOSIP_C_
#include <ortp/ortp.h>
#include <ortp/port.h>
#include "mcosip.h"
#include "common.h"
#include "mcgtk.h"

bool osip_initial(osip_core* oc)
{
      char *sip_addr;
      
      struct ifaddrs * ifAddrStruct=NULL;
      struct ifaddrs * ifa=NULL;
      void * tmpAddrPtr=NULL;
      char addressBuffer[INET_ADDRSTRLEN];
      
      getifaddrs(&ifAddrStruct);
      for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
	    if(ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name,"lo")) {
		  tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;	      
		  inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
// 		  printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
		  sip_addr = addressBuffer;
		  break;
	    } 
      }      
      if (ifAddrStruct!=NULL)
	    freeifaddrs(ifAddrStruct);
      
      oc->port = 5060;
      oc->dest_call = sip_addr;
      oc->identity = sip_addr;
      
      if (eXosip_init()){
	    printf ("Can't initialize eXosip! \n");
	    return false;
      }
      
      if(eXosip_listen_addr(IPPROTO_UDP, NULL, oc->port, AF_INET, 0)){
	    eXosip_quit();
	    fprintf(stderr, "eXosip_listen_addr error!\n");
	    return false;
      }
      
      return true;
}

void osip_uninit(osip_core* oc)
{
      eXosip_quit();
      return;
}

eXosip_event_t* osip_wait_event(int seconds, int mseconds)
{
      eXosip_event_t * je = NULL;
      
      je = eXosip_event_wait(seconds,mseconds);
      // Protocol stack with this statement, the specific role of the unknown
      eXosip_lock();
      eXosip_default_action(je);
      eXosip_automatic_refresh();
      eXosip_unlock ();
      
      return je;
}

char* osip_get_ipaddr()
{
      struct ifaddrs * ifAddrStruct=NULL;
      struct ifaddrs * ifa=NULL;
      void * tmpAddrPtr=NULL;
      char addressBuffer[INET_ADDRSTRLEN];
      
      getifaddrs(&ifAddrStruct);
      for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
	    if(ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name,"lo")) {
		  tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;	      
		  inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
		  printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
		  break;
	    } 
      }      
      if (ifAddrStruct!=NULL)
	    freeifaddrs(ifAddrStruct);
      
      if(!strcmp(addressBuffer,""))
// 	printf("%s IP Address1 %s\n", ifa->ifa_name, addressBuffer);
	    return NULL;
      else
// 	printf("%s IP Address2 %s\n", ifa->ifa_name, addressBuffer);
	    return addressBuffer;
}

void osip_accept_call(GtkWidget*widget,gpointer data){
//       g_print("Call incomming:\n");      
      osip_message_t *answer = NULL;
      char tmp [4096];
      
      gtk_widget_destroy(GTK_WIDGET(data));
      oc->status_call = ACCEPTT;
      eXosip_lock();
      if(eXosip_call_build_answer (oc->je-> tid, 200, &answer)){
	    printf ("This request msg is invalid! Cann't response!\n");
	    eXosip_call_send_answer (oc->je-> tid, 400, NULL);
      }else{
	    snprintf(tmp, 4096,
	    "V = 0\r\n"
	    "O = anonymous 0 0 IN IP4 0.0.0.0\r\n"
	    "T = 1 10\r\n"
	    "A = username: Lab411\r\n"
	    "A = password: phutran\r\n");
	    // Set the response SDP message body, the next step plan to analyze the body of the message
	    // No analysis of the message body, a direct reply to the original message, to do this one bad.
	    osip_message_set_body (answer, tmp, strlen (tmp));
	    osip_message_set_content_type (answer, "application/sdp");	    
	    eXosip_call_send_answer (oc->je-> tid, 200, answer);
	    osip_free(answer);
      }
      eXosip_unlock();      
}

void osip_decline_call(GtkWidget* widget, gpointer data)
{
      gtk_widget_destroy(GTK_WIDGET(data));
      oc->status_call = DECLINE;
      
      eXosip_lock();
      eXosip_call_send_answer (oc->je-> tid,486,NULL);
      eXosip_unlock();
}

void* osip_threads(void* arg)
{     
      osip_from_t *from_url;
      char *tmp;
      
      osip_initial(oc);	
      while(true){
	    oc->je = osip_wait_event(60,0);
	    eXosip_lock ();
	    eXosip_default_action (oc->je);
	    eXosip_automatic_refresh ();
	    eXosip_unlock ();

	    if(oc->je == NULL) // does not receive the message
		continue;
	    switch(oc->je->type)
	    {
		case EXOSIP_MESSAGE_NEW :   // new message arrives
		      printf("EXOSIP_MESSAGE_NEW! \n");
		      if (MSG_IS_MESSAGE (oc->je->request)) // if the received message type is Message
		      {
			  osip_body_t * body;
			  osip_message_get_body (oc->je->request, 0, &body);
			  printf("I get the msg is: %s \n", body->body);
		      }
		      break;
		case EXOSIP_CALL_INVITE:
		      from_url=oc->je->request->from;
		      if (osip_convert_addr(from_url,&tmp)!=0){
			     strcpy(tmp,"Unknown user");
		      }

		      eXosip_lock();
		      eXosip_call_send_answer(oc->je->tid, 100, NULL);
		      eXosip_call_send_answer(oc->je->tid, 180, NULL);     
		      eXosip_unlock ();
// 		      printf("Received a INVITE msg from %s: %s, UserName is %s, password is %s \n",
// 			       tmp,oc->je->request->req_uri->port,
// 				oc->je->request->req_uri->username, oc->je->request->req_uri->password);
		      gtk_widget_show_all(gtk_create_window_call(tmp));	
		      osip_free(tmp);
		      osip_free(from_url);
		      break;
		case EXOSIP_CALL_ACK:
		case EXOSIP_CALL_CLOSED:
		case EXOSIP_CALL_MESSAGE_NEW:
		      break;
		default:
		      break;
	    }
      }
}

int osip_convert_addr(osip_from_t *from,char **str)
{
	osip_from_t *tmpfrom=NULL;
	osip_from_clone(from,&tmpfrom);
	if (tmpfrom!=NULL){
		while(!osip_list_eol(&tmpfrom->gen_params,0)){
			osip_generic_param_t *param=(osip_generic_param_t*)osip_list_get(&tmpfrom->gen_params,0);
			osip_generic_param_free(param);
			osip_list_remove(&tmpfrom->gen_params,0);
		}
	}else return -1;
	osip_from_to_str(tmpfrom,str);
	osip_from_free(tmpfrom);
	return 0;
}

#endif