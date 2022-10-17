#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>
#include "request_response_data.h"
#include <arpa/inet.h>
#include <sys/time.h>


#define MAX_CLIENT_SUPPORTED 1000
#define SERVER_PORT 3000

client_request_d client_data;
server_response_d server_result;
char data_buffer[1024];

int controlfd_set[1000];

static void controlfd_setf()
{

    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        controlfd_set[i] = -1;
    }
}
static void add_controlf_set(int skt_fd)
{
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (controlfd_set[i] != -1)
            continue;

        controlfd_set[i] = skt_fd;
        break;
    }
}

static void remove_from_controlfd_set(int skt_fd)
{
    int i = 0;
    for (; i < MAX_CLIENT_SUPPORTED; i++)
    {

        if (controlfd_set[i] != skt_fd)
            continue;

        controlfd_set[i] = -1;
        break;
    }
}

static void
restart_init_readfds(fd_set *fd_set_ptr)
{

    FD_ZERO(fd_set_ptr);
    int i = 0;
    for (; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (controlfd_set[i] != -1)
        {
            FD_SET(controlfd_set[i], fd_set_ptr);
        }
    }
}

static int get_max_fd(){

    int i = 0;
    int max = -1;

    for (; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (controlfd_set[i] > max)
            max = controlfd_set[i];
    }

    return max;
}

void tcp_server_communication() {
    // vriable declartion
    int master_sock_fd = 0,
    sent_recive = 0,
    addr_len = 0,
    opt = 1;

    // client communication socket
    int comm_socket_fd = 0;
    fd_set readfdfile; 

    struct sockaddr_in server_addr, client_addr;

    // inctialize controle fdset

    controlfd_setf();

    // create master socket
    if ((master_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) == -1))
    {
        printf("socket creation failed\n");
        exit(1);
    }

    // Assigne the server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = SERVER_PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    addr_len = sizeof(struct sockaddr);

    // Bind the server

    if (bind(master_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("socket bind faild\n");
        return;
    }
    if (listen(master_sock_fd, 5) < 0)
    {
        printf("listen faild\n");
        return;
    }

    // add the master fd to the controle fd set
    add_controlf_set(master_sock_fd);

    // client conection

    while (1)
    {
        // incialized the fd set to read               

        restart_init_readfds(&readfdfile);
        printf("Sytem call \n"); 

        // Waiting the clint connection
        select(get_max_fd() + 1, &readfdfile, NULL, NULL, NULL);

        if (FD_ISSET(master_sock_fd, &readfdfile)){
            printf("New connection recived \n");

            comm_socket_fd = accept(master_sock_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (comm_socket_fd < 0)
            {
                printf("acceept error: error = %d\n", errno);
                exit(0);
            }
            add_controlf_set(comm_socket_fd);
            printf("Connection accepted from the client: %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }
        //To the other client
        else{
            int comm_socket_fd = -1;
            for(int i = 0; i <MAX_CLIENT_SUPPORTED; i++){
                
                if(FD_ISSET(controlfd_set[i], &readfdfile)){
                    comm_socket_fd = controlfd_set[i];

                    memset(data_buffer,0,sizeof(data_buffer));
                    sent_recive = recvfrom(comm_socket_fd,(char *)data_buffer,sizeof(data_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
                    printf("server recive data from client : %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    
                    if(sent_recive == 0){
                       // close(comm_socket_fd);
                        remove_from_controlfd_set(comm_socket_fd);
                         break;
                    }

                
                    client_request_d *client_data =  (client_request_d *)data_buffer;
                    if(client_data->file_index == 0 &&& client_data->key){
                       // close(comm_socket_fd);
                        remove_from_controlfd_set(comm_socket_fd);
                        printf("The server close \n");
                        break;
                    }
                    server_response_d server_result;
                    
                    //process and respond based on the file index and key
                    
                    /*server_result.file_size; */

                    sent_recive = sendto(comm_socket_fd, (char *)&server_result, sizeof(server_response_d), 0,
                                (struct sockaddr *)&client_addr, sizeof(struct sockaddr));

                    printf("Server replay to the clint \n");

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
