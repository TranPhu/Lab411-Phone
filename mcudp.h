#ifndef _MC_UDP_H_
#define _MC_UDP_H_
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define BUFLEN 65535  	//Max length of buffer
#define PORT 8000   	//The port on which to send data

unsigned char buf[BUFLEN];
struct sockaddr_in server_other;
struct sockaddr_in client_other;
char *SERVER;
int server_socket,client_socket;
int slen;

void die(char *s);
int creat_client_socket(char *add);
int creat_server_socket();
int send_datas(int socket,unsigned char *buffer,int len);
unsigned char *recev_datas(int socket);

#endif