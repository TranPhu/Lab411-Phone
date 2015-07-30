#ifndef _MCSOCKET_H_
#define _MCSOCKET_H_
#include <sys/types.h>   /* for type definitions */
#include <sys/socket.h>  /* for socket API function calls */
#include <netinet/in.h>  /* for address structs */
#include <arpa/inet.h>   /* for sockaddr_in */
#include <stdio.h>       /* for printf() */
#include <stdlib.h>      /* for atoi() */
#include <string.h>      /* for strlen() */
#include <unistd.h>      /* for close() */

#define MAX_LEN  102400    /* maximum string size to send */
#define MIN_PORT 1024    /* minimum port allowed */
#define MAX_PORT 65535   /* maximum port allowed */

    int _sock;
    struct sockaddr_in mc_addr_send; /* socket address structure */
    struct sockaddr_in mc_addr_recev; /* socket address structure */
    
  /**************************function**********************/
  int create_socket_send(char* mc_addr_str,unsigned short mc_port);
  int create_socket_recev(char* mc_addr_str,unsigned short mc_port);
  unsigned int send_data(int sock, unsigned char* send_str,unsigned int len);
  unsigned char* recev_data(int sock);
  void drop_socket(int sock,struct ip_mreq mc_req);
  /**************************end function******************/
#endif