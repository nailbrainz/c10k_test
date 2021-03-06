#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "c10k_epoll.h"
#include "c10k_select.h"
#include "c10k_pthread.h"

struct AppObj{
    void (*server_engine)(int serv_fd, uint8_t buffer[BUF_SIZE], uint32_t buf_size);
};

uint8_t buffer[BUF_SIZE];

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
    int serv_fd;

    if( (serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        fprintf(stderr, "[SERV] : Can't open stream socket\n");
        exit(-1);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);
    printf("[SERV] port = %u, trying bind!\n", SERV_PORT);
    //binds the port and serv_fd
    if(bind(serv_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) <0)
    {
        fprintf(stderr, "[SERV] : Can't bind local address.\n");
        exit(-1);
    }
    

    printf("[SERV] bind finished. Beginning to listen...\n");
    if(listen(serv_fd, 5) < 0) //5 : max # of accpeted sockets (not processed yet)
    {
        fprintf(stderr, "[SERV]: Can't listening connect.\n");
        exit(-1);
    }
    


    struct AppObj appObj = {
        //.server_engine = select_work_forever
        //.server_engine = epoll_work_forever
        .server_engine = pthread_work_forever
    };
    printf("[SERV] listen done. Beginning to run the main c10k procedure...\n");
    appObj.server_engine(serv_fd, buffer, BUF_SIZE);

    return 0;
}