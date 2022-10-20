#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include  "request_response_data.h"

#define DEST_PORT            2000
#define SERVER_IP_ADDRESS   "127.0.0.1"

client_request_d client_data;
server_response_d result;

void tcp_communication(){

    /*Initialization*/   
    int sockfd = 0, 
        sent_recv_bytes = 0;

    int addr_len = 0;

    addr_len = sizeof(struct sockaddr);

    struct sockaddr_in dest;

    /* server information*/   
    dest.sin_family = AF_INET;

    /*Client wants  send data to server process */
 
    dest.sin_port = DEST_PORT;
    struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP_ADDRESS);
    dest.sin_addr = *((struct in_addr *)host->h_addr);

    /*Create a TCP socket*/  
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    connect(sockfd, (struct sockaddr *)&dest,sizeof(struct sockaddr));

    /*Step 4 : recive data to be sent to server*/    

PROMPT_USER:
    

    // Add the code
    printf("Enter index : ?\n");
    scanf("%u", &client_data.file_index);
    printf("Enter key : ?\n");
    scanf("%u", &client_data.key);

    
    /*send the data to server*/
    sent_recv_bytes = sendto(sockfd, 
           &client_data,
           sizeof(client_request_d), 
           0, 
           (struct sockaddr *)&dest, 
           sizeof(struct sockaddr));
    
    printf("No of bytes sent = %d\n", sent_recv_bytes);  
  
    
    /*recvfrom data from server*/
    sent_recv_bytes =  recvfrom(sockfd, (char *)&result, sizeof(server_response_d), 0,(struct sockaddr *)&dest, &addr_len);

    printf("No of bytes recvd = %d\n", sent_recv_bytes);
    
    printf("Result recvd = %u\n", result.file_size);
    /*If client to send data agin*/
    goto PROMPT_USER;
}
    

int
main(int argc, char **argv){

    tcp_communication();
    printf("application quits\n");
    return 0;
}