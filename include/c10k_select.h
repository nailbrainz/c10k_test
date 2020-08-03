#ifndef __C10K_SELECT_SERV__
#define __C10K_SELECT_SERV__
#include "c10k_common.h"

void select_work_forever(struct sockaddr_in* p_serv_addr, uint8_t buffer[BUF_SIZE]){
    struct sockaddr_in client_addr;
    int serv_fd;
    if( (serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        fprintf(stderr, "Server : Can't open stream socket\n");
        exit(-1);
    }
    
    printf("[C10K][SELECT][SERV] trying bind!\n");
	//binds the port and serv_fd
    if(bind(serv_fd, (struct sockaddr *)p_serv_addr, sizeof(struct sockaddr_in)) <0)
    {
        fprintf(stderr, "[C10K][SELECT][SERV] : Can't bind local address.\n");
        exit(-1);
    }
    

	printf("[C10K][SELECT][SERV] bind finished. Beginning to listen...\n");
    if(listen(serv_fd, 5) < 0)
    {
        fprintf(stderr, "[C10K][SELECT][SERV]: Can't listening connect.\n");
        exit(-1);
    }
	
	fd_set reads, cpy_reads;
	FD_ZERO(&reads);
	FD_SET(serv_fd, &reads);
	int fd_max = serv_fd; //for each connected clients, assign socket file descriptor
	int fd_num = 0;
    printf("[C10K][SELECT][SERV]: wating connection request.\n");
	struct timeval timeout;
	unsigned int cnt = 0;
    while(1)
    {
		cnt++;
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 50000;
		if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1){
			fprintf(stderr, "[C10K][SELECT][SERV] fd_num : %d select() error", fd_num);
			break;
		}
		if(fd_num == 0)
			continue;

		for(int i = 0; i<=fd_max; i++){
			if(FD_ISSET(i, &cpy_reads)){
				if(i == serv_fd){
					int sock_fd = 0, add_struct_size = 0;
					sock_fd = accept(serv_fd, (struct sockaddr *)&client_addr, &add_struct_size);
					FD_SET(sock_fd, &reads);
					if(fd_max < sock_fd){
						fd_max = sock_fd;
					}
					printf("[C10K][SELECT][SERV] accept!\n");
					if(sock_fd < 0)
					{
						fprintf(stderr, "[C10K][SELECT][SERV]: accept failed.\n");
						exit(-1);
					}
				}else{
					if(cnt % 1000 == 0)
						printf("[C10K][SELECT][SERV] processed socket %d\n", i);
					int msg_size = read(i, buffer, BUF_SIZE);
					if(msg_size == 0){
						FD_CLR(i, &reads);
				        close(i);
						printf("[C10K][SELECT][SERV] Server : %d client closed.\n", i);
					}
				}
			}
		}
    }
    close(serv_fd);
	printf("[C10K][SELECT][SERV], closed socket successfully (unreachable, perahps)\n");
}

#endif