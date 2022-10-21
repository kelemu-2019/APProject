#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include  "request_response_data.h"

#define DEST_PORT            2000
#define MAX 80
#define SERVER_IP_ADDRESS   "127.0.0.1"

client_request_d client_data;
server_response_d server_result;

void tcp_communication(){

    /*Initialization*/   
    int sockfd = 0, 
        sent_recv_data= 0;

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

    /*recive data to be sent to server*/    

PROMPT_USER:
    


    // Add the code
   
   /*
    bzero(client_data.buff, sizeof(client_data.buff[MAX]));
    printf("Enter the string : ");
    int n = 0;
    while ((client_data.buff[n++] = getchar()) != '\n');
    write(sockfd,client_data.buff, sizeof(client_data.buff[MAX]));
    */

    
    
    printf("Enter request file index, key : \n");    
    scanf("%[^\n]%*c", client_data.buff);    
    
    /*
    printf("Enter index : ?\n");
    scanf("%u", &client_data.file_index);
    
    printf("Enter key : ?\n");
    scanf("%u", &client_data.key);

    printf("Data %s\n",server_result.reslt);
    
*/     

 

    /*send the data to server*/
    
    sent_recv_data = sendto(sockfd,&client_data,sizeof(client_request_d),0, (struct sockaddr *)&dest,sizeof(struct sockaddr));
        
   
    //printf("No of bytes sent = %d\n", sizeof(client_data));  
  
    
    /*recvfrom data from server*/
    sent_recv_data =  recvfrom(sockfd, (char *)&server_result, sizeof(server_response_d), 0,(struct sockaddr *)&dest, &addr_len);

    //printf("No of bytes recived = %d\n", sizeof(server_response_d));
    int x =strlen(server_result.reslt);
    printf("Recived file size = %d\n",x);
    printf("Recived encrypted file  = %s\n", server_result.reslt);

    /*If client to send data agin*/
    goto PROMPT_USER;
}
    

int
main(int argc, char **argv){

    tcp_communication();
    printf("application quits\n");
    return 0;
}