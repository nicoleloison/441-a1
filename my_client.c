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

int main(void)
{
    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[8888];
    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;
    
    /* Create a socket first */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    
    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); /*port 5000*/
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    /* Attempt a connection */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
    
    
    /* Create file where data will be stored */
    FILE *fp;
    fp = fopen("transmit.txt", "w");

    if(NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }
    int octoleg_num =0;
    
    /* Receive data in chunks of 8888 bytes */
    while((bytesReceived = read(sockfd, recvBuff, 8888)) > 0)
    {
        octoleg_num++;
        printf("Bytes received %d\n",bytesReceived);
        printf("Octoleg number :  %d\n",octoleg_num);
        fwrite(recvBuff, 1,bytesReceived,fp);
    }
    
    if(bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }
   
    fseek(fp, 0, SEEK_END);
    int size_file = ftell(fp);
    printf("total file size transmitted :  %d\n",size_file);
    
    return 0;
}
