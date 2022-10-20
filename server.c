#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "request_response_data.h"


#define MAX_CLIENT_SUPPORTED   700
#define SERVER_PORT     2000 

client_request_d client_data;
server_response_d server_result;
char data_buffer[1024];

int controlfd_set[64];

static void
intitiaze_monitor_fd_set(){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++)
        controlfd_set[i] = -1;
}

static void 
add_to_controlfd_set(int skt_fd){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(controlfd_set[i] != -1)
            continue;   
        controlfd_set[i] = skt_fd;
        break;
    }
}

static void
remove_from_controlfd_set(int skt_fd){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(controlfd_set[i] != skt_fd)
            continue;

        controlfd_set[i] = -1;
        break;
    }
}

static void re_init_readfds(fd_set *fd_set_ptr){

    FD_ZERO(fd_set_ptr);    
   
    for(int i = 0; i < MAX_CLIENT_SUPPORTED; i++){
        if(controlfd_set[i] != -1){
            FD_SET(controlfd_set[i], fd_set_ptr);
        }
    }
}

static int  get_max_fd(){

    int i = 0;
    int max = -1;

    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(controlfd_set[i] > max)
            max = controlfd_set[i];
    }

    return max;
}

void tcp_server_communication(){

    /*Initialization*/
    /*Master socket file descriptor */
    int master_sock_tcp_fd = 0, 
        sent_recv_bytes = 0, 
        addr_len = 0, 
        opt = 1;

    int comm_socket_fd = 0;     
    fd_set readfds;             
    
    /*store the server and client info*/
    struct sockaddr_in server_addr, 
                       client_addr;
    
    intitiaze_monitor_fd_set();

    /*tcp master socket creation*/
    if ((master_sock_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP )) == -1)
    {
        printf("socket creation failed\n");
        exit(1);
    }

    /*server Information*/
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = SERVER_PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    addr_len = sizeof(struct sockaddr);

    /* Bind the server*/

    if (bind(master_sock_tcp_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("socket bind failed\n");
        return;
    }

    /*S To maintain the queue of max length incoming client -----?*/
  
    if (listen(master_sock_tcp_fd, 5)<0)  
    {
        printf("listen failed\n");
        return;
    }


    /*Add master socket to controlfds*/
    add_to_controlfd_set(master_sock_tcp_fd);
  

    while(1){
       
        /* Initialize the file descriptor set*/
        re_init_readfds(&readfds);               
        printf("blocked on  and select System call...\n");

        /* Wait for client connection*/        
        select(get_max_fd() + 1, &readfds, NULL, NULL, NULL); 

        /*If Data arrives on master socket FD*/
        if (FD_ISSET(master_sock_tcp_fd, &readfds))
        { 
           printf("New connection recieved recvd, accept the connection.\n");

            /* accept() returns a new temporary file desriptor(fd) */
            comm_socket_fd = accept(master_sock_tcp_fd, (struct sockaddr *)&client_addr, &addr_len);
            if(comm_socket_fd < 0){

                printf("accept error : errno = %d\n", errno);
                exit(0);
            }

            add_to_controlfd_set(comm_socket_fd); 
            printf("Connection accepted from client : %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }
        /* Data srrives on from other client FD*/
        else 
        {

            int i = 0, comm_socket_fd = -1;
            for(; i < MAX_CLIENT_SUPPORTED; i++){

                /*Find the clinet FD on which Data has arrived*/
                if(FD_ISSET(controlfd_set[i], &readfds)){

                    comm_socket_fd = controlfd_set[i];

                    memset(data_buffer, 0, sizeof(data_buffer));
                    sent_recv_bytes = recvfrom(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);

                    printf("Server recvd %d bytes from client %s:%u\n", sent_recv_bytes,
                            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                    if(sent_recv_bytes == 0){
                        /*If server recvs empty msg from client, server may close the connection and wait */                        
                        close(comm_socket_fd);
                        remove_from_controlfd_set(comm_socket_fd);
                        break; 

                    }

                        // The code  merg
                    client_request_d*client_data = (client_request_d*)data_buffer;

                    /* If the client sends a special msg to server, then server close the client connection */                    
                    if(client_data->file_index == 0 && client_data->key ==0){

                        close(comm_socket_fd);
                        remove_from_controlfd_set(comm_socket_fd);
                        printf("Server closes connection with client : %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));                       
                        break;
                    }

                    server_response_d result;
                    server_result.file_size = client_data->file_index + client_data->key;

                    /* Server replying back to client */
                    sent_recv_bytes = sendto(comm_socket_fd, (char *)&server_result, sizeof(server_response_d), 0,
                            (struct sockaddr *)&client_addr, sizeof(struct sockaddr));

                    printf("Server sent %d bytes in reply to client\n", sent_recv_bytes);
                }
            }
        }

    }
}

int
main(int argc, char **argv){

    tcp_server_communication();
    return 0;
}