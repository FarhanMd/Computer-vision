#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0,cmd_to_send,alpha;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 
	char buff[10];
	
    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

     while (1) {
 	
	//printf("enter a value:");
	//scanf("%d",&cmd_to_send);
        //write via sockfd
       //alpha= write(sockfd, &ch, cmd_to_send);
	buff[0]='1';
	buff[1]=0;
	alpha = write(sockfd,buff,1);
	printf("alpha is %d",alpha);
        if (alpha== -1) break ;
         
    }
    close(sockfd);
 
    return 0;
}
