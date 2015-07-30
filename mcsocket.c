#ifndef _MCSOCKET_C_
#define _MCSOCKET_C_
#include "mcsocket.h"
#include "mccodec.h"

int create_socket_send(char* mc_addr_str, short unsigned mc_port)
{
    int sock;                   /* socket descriptor */
    char send_str[MAX_LEN];     /* string to send */
    unsigned char mc_ttl=1;     /* time to live (hop count) */
    
    if ((mc_port < MIN_PORT) || (mc_port > MAX_PORT)) 
    {
	fprintf(stderr, "Invalid port number argument %d.\n",
		mc_port);
	fprintf(stderr, "Valid range is between %d and %d.\n",
		MIN_PORT, MAX_PORT);
	exit(1);
    }
    
    /* create a socket for sending to the multicast address */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
    {
	perror("socket() failed");
	exit(1);
    }
    
     /* set the TTL (time to live/hop count) for the send */
    if ((setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, 
	(void*) &mc_ttl, sizeof(mc_ttl))) < 0) {
      perror("setsockopt() failed");
      exit(1);
    } 
  
    /* construct a multicast address structure */
    memset(&mc_addr_send, 0, sizeof(mc_addr_send));
    mc_addr_send.sin_family      = AF_INET;
    mc_addr_send.sin_addr.s_addr = inet_addr(mc_addr_str);
    mc_addr_send.sin_port        = htons(mc_port);
    
     /* clear send buffer */
    memset(send_str, 0, sizeof(send_str));
    
    return sock;
}

int create_socket_recev(char* mc_addr_str, short unsigned int mc_port)
{
    int sock;                     /* socket descriptor */
    int flag_on = 1;              /* socket option flag */
    struct ip_mreq mc_req;        /* multicast request structure */
    
    /* validate the port range */
    if ((mc_port < MIN_PORT) || (mc_port > MAX_PORT)) 
    {
	fprintf(stderr, "Invalid port number argument %d.\n",
		mc_port);
	fprintf(stderr, "Valid range is between %d and %d.\n",
		MIN_PORT, MAX_PORT);
	exit(1);
    }

    /* create socket to join multicast group on */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
	perror("socket() failed");
	exit(1);
    }
    
    /* set reuse port to on to allow multiple binds per host */
    if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag_on,
	sizeof(flag_on))) < 0) {
      perror("setsockopt() failed chu!");
      exit(1);
    }

    /* construct a multicast address structure */
    memset(&mc_addr_recev, 0, sizeof(mc_addr_recev));
    mc_addr_recev.sin_family      = AF_INET;
    mc_addr_recev.sin_addr.s_addr = htonl(INADDR_ANY);
    mc_addr_recev.sin_port        = htons(mc_port);

    /* bind to multicast address to socket */
    if ((bind(sock, (struct sockaddr *) &mc_addr_recev, 
	sizeof(mc_addr_recev))) < 0) {
      perror("bind() failed");
      exit(1);
    }

    /* construct an IGMP join request structure */
    mc_req.imr_multiaddr.s_addr = inet_addr(mc_addr_str);
    mc_req.imr_interface.s_addr = htonl(INADDR_ANY);

    /* send an ADD MEMBERSHIP message via setsockopt */
    if ((setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
	(void*) &mc_req, sizeof(mc_req))) < 0) {
      perror("setsockopt() failed roi");
      exit(1);
    }
    
    return sock;
}

unsigned int send_data(int sock, unsigned char* send_str,unsigned int len)
{
    unsigned int lenx;
    
    /* send string to multicast address */
    if ((lenx = sendto(sock, send_str, len, 0, 
         (struct sockaddr *) &mc_addr_send, 
         sizeof(mc_addr_send))) != len) {
	    printf("size of data = %i byte, size of sended data = %i bytes\n",len,lenx);
	//exit(1);
    }
//     printf("sened data: %i  -- %i\n",len,lenx);
    /* clear send buffer */
    memset(send_str, 0, sizeof(send_str));
    
    return lenx;
}

unsigned char* recev_data(int sock)
{
    unsigned char *data;
    unsigned char recv_str[28800];     /* buffer to receive string */
    int recv_len;                 /* length of string received */
    unsigned int from_len;        /* source addr length */
    struct sockaddr_in from_addr; /* packet source */
    
    /* clear the receive buffers & structs */
    memset(recv_str, 0, sizeof(recv_str));
    from_len = sizeof(from_addr);
    memset(&from_addr, 0, from_len);
//     data = malloc(sizeof(unsigned char *)*MAX_LEN);

    /* block waiting to receive a packet */
    if ((recv_len = recvfrom(sock, recv_str, 28800, 0, 
         (struct sockaddr*)&from_addr, &from_len)) < 0) {
      perror("recvfrom() failed");
      exit(1);
    }
    avpkt.data = recv_str;
    avpkt.size = recv_len;
    data = recv_str;
    
    /* output received string */
    
    return data;
}

void drop_socket(int sock, struct ip_mreq mc_req)
{
    if ((setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, 
       (void*) &mc_req, sizeof(mc_req))) < 0) {
	  perror("setsockopt() failed");
	  exit(1);
    }
}

#endif