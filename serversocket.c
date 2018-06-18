#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0,rc=0,cmd_to_send,cmd_to_execute;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    //time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(2800); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        //ticks = time(NULL);
        //snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        //write(connfd, sendBuff, strlen(sendBuff)); 

    while(1)
    {
        char ch;
        printf("\nserver waiting\n");
 
        //Accept a connection
        //client_len = sizeof(client_address);
        //client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);
        //printf("after accept()... client_sockfd = %d\n", client_sockfd) ;
        //Read write to client on client_sockfd
 
       cmd_to_execute = read(connfd, &ch, cmd_to_send);
	//sleep(1);
	switch(cmd_to_execute)
	{
  		case 1:printf("you have raised the right hand");
			//sleep(1);
			break;
		case 2:printf("you have raised the lefthand");
			//sleep(1);

			break;
		case 3:printf("i could see left turn");
			//sleep(1);
			break;
		case 4:printf("i could see right turn");
			//sleep(1);
			break;
 		//default :printf("pose is not detected");
			//sleep(1);
			break;
	}
    }
 
    printf("server exiting\n");
 
    //close(client_sockfd);
        close(connfd);
        sleep(1);
return 0;
}
