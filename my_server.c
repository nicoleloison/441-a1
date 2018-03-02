#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main(void)
{
    int listenfd = 0;
    int connfd = 0;
    int octoleg_count =1;
    int octoblock_count =1;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    int numrv;
    int octoblock_size= 8888;
    int octoleg =1111;

    
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);
    
    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
    
    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }
    
    /*sending to server*/
    printf("Type the name of the file to transfer\n");
    const char* file_name [1024];
    scanf("%s",file_name);
    strcat(file_name,".txt");
    
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);
        
        /* Open the file that we wish to transfer */
        FILE *fp = fopen(file_name,"rb");
        if(fp==NULL)
        {
            printf("ERROR: %s\n", strerror(errno));
            return -1;
        }
        
        fseek(fp, 0, SEEK_END);
        int size_file = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        printf("Total file size = %d\n",size_file);
        int number_of_blocks = size_file / octoblock_size;
        int last_chunk_size = size_file % octoblock_size;
        
        if (last_chunk_size!=0){
            number_of_blocks ++;
            printf("Number of octoblocks = %d\n",number_of_blocks);
        }
        
        
        
        /* Read data from file and send it */
        while(1)
        {
            if(octoleg_count>8){
                octoleg_count=0;
                octoblock_count++;
            }
            /* First read file in chunks of 1111 bytes */
            unsigned char buff[1111]={0};
            int nread = fread(buff,1,1111,fp);
            
            /* If read was success, send data with octoleg number. */
            if(nread > octoleg)
            {
                octoleg_count++;
                printf("Sending octoleg number %d", octoleg_count);
                printf(" of octoblock number %d", octoblock_count);
                
            }
            
            /*last chunk*/
            else if (nread < 1111 && nread > 0)
            {
                octoblock_count++;
                octoleg_count++;
                write(connfd, buff, nread);
                
                if (feof(fp)){
                    printf("End of file\n");
                    return 0;
                }
                
                if (ferror(fp))
                printf("Error reading\n");
                break;
            }
            
            write(connfd, buff, nread);
            
            
        }
        
        close(connfd);
        sleep(1);
    }
    
    
    return 0;
}
