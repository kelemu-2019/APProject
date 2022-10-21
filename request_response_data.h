typedef struct client_request_data
{
    unsigned int file_index;
    unsigned int key;
    char  buff[];
}client_request_d;

typedef struct server_response_data
{
   unsigned int errorCode;
   unsigned int file_size;
   char reslt[];
   

}server_response_d;
