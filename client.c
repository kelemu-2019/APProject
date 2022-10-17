#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include<netdb.h>
#include<memory.h>
#include "request_response_data.h"


#define DEST_PORT  3000
#define SRV_IP_ADDS "192.168.0.1"

client_request_d client_data;
server_response_d  server_result;

void tcp_client_communication(){

    int sockfd = 0, sent_recv = 0;
    unsigned int addr_len = 0;

    addr_len = sizeof(struct sockaddr);

    //Socket addr, ip and port 
    struct sockaddr_in dest;

    //Srver Info
    dest.sin_family = AF_INET;
    dest.sin_port = DEST_PORT;

    //Create TCP socket
    struct hostent *host = (struct hostent *)gethostbyname(SRV_IP_ADDS);
    dest.sin_addr = *((struct in_addr *)host->h_addr_list);

    //connect client to server 

    connect(sockfd,(struct sockaddr *)&dest, sizeof(struct sockaddr));

    //send file index and key to the server

    REPEAT_REQUEST:

    printf("Enter File index: \n");
    scanf("%d", &client_data.file_index);
    printf("Enter key: \n");
    scanf("%d", &client_data.key);

    //send to the server

    sent_recv = sendto(sockfd, &client_data, sizeof(struct sockaddr),0,(struct sockaddr *)&dest,sizeof(struct sockaddr));

    printf("File is sent \n");

    //Respons  from server 

    sent_recv = recvfrom(sockfd, (char *)&server_result, sizeof(server_response_d), 0,(struct sockaddr *)&dest, &addr_len);
    
    //Respons from server expected----
    printf("Respons from server ----\n");

    //To reapeat the clint the request

    goto REPEAT_REQUEST;

   }
   
   int main(int argc, char **argv){

    tcp_client_communication();
    printf("communication is terminated");
    return 0;

   } 


