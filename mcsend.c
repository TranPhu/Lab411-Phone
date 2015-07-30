#include <sys/types.h>   /* for type definitions */
#include <sys/socket.h>  /* for socket API function calls */
#include <netinet/in.h>  /* for address structs */
#include <arpa/inet.h>   /* for sockaddr_in */
#include <stdio.h>       /* for printf() */
#include <stdlib.h>      /* for atoi() */
#include <string.h>      /* for strlen() */
#include <unistd.h>      /* for close() */
#include <assert.h>
#include <pthread.h>

#include "mcgtk.h"
#include "mcv4l2.h"
#include "mcsocket.h"
#include "mccodec.h"
#include "mcudp.h"
#include "mcosip.h"

char* address;          	/* multicast IP address */
unsigned short mc_port;     	/* multicast port */
int width, height;
pthread_t osip_thread;

int main(int argc, char **argv)
{
      io = V4L2_MEMORY_MMAP;
      dev_name = "/dev/video0";
      width = 352; height = 288;
    /*****************************/    
      open_device();
      init_device(width, height);
      start_capturing();
      
      oc = malloc(sizeof(osip_core*));
      if(pthread_create(&osip_thread, NULL, &osip_threads, NULL))
	    printf("Error when create thread\n");
	    
    
#ifdef WITH_GTK
	main_frontend(argc, argv);
#endif
// 	pthread_join(osip_thread, NULL);
	uninit_device();
	close_device();
	
	return 0;
}
