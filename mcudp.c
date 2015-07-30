#ifndef _MC_UDP_C_
#define _MC_UDP_C_
#include "mcudp.h"
#include "mccodec.h"

void die(char* s)
{
    perror(s);
    exit(1);
}

int creat_client_socket(char* add)
{
    int s, i, slen;
 
    slen=sizeof(client_other);
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &client_other, 0, sizeof(client_other));
    client_other.sin_family = AF_INET;
    client_other.sin_port = htons(PORT);
     
    if (inet_aton(add , &client_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
    return s;
}

int creat_server_socket()
{
    struct sockaddr_in si_me;     
    int s;
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
    
    return s;
}

int send_datas(int socket, unsigned char* buffer, int len)
{
    int size;
    
    size = sendto(socket, buffer, len , 0 , (struct sockaddr *) &client_other, sizeof(client_other));
    if(size == -1)
    {
	die("sendto()");
    }
//     else printf("Send data ok\n");
    
    return size;
}

unsigned char* recev_datas(int socket)
{    
//     printf("waiting data...!\n");
    int size;
    fflush(stdout);  
    
    size = recvfrom(socket, buf, BUFLEN, 0, (struct sockaddr *) &server_other, &slen);
    
//     printf("Received packet from %s:%d   --    %i Bytes\n", inet_ntoa(server_other.sin_addr), ntohs(server_other.sin_port),size);
//     printf("Data: %s\n" , buf);
    avpkt.data = buf;
    avpkt.size = size;
    return buf;
}

#endif