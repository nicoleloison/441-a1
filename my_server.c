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
    int octoleg_count =0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    int numrv;

    
    
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
        int number_of_chunks = size_file / 8888;
        int last_chunk_size = size_file % 8888;
      
        if (last_chunk_size!=0){
            number_of_chunks ++;
            printf("Number of octoleg = %d\n",number_of_chunks);
        }
        
        
        /* Read data from file and send it */
        while(1)
        {
            /* First read file in chunks of 8888 bytes */
            unsigned char buff[8888]={0};
            int nread = fread(buff,1,8888,fp);
            printf("Bytes read %d \n", nread);
  
            
            /* If read was success, send data. */
            if(nread > 8888)
            {
                octoleg_count++;
                printf("Sending octoleg number %d\n", octoleg_count);
                
            }

            /*last chunk*/
            else if (nread < 8888 && nread > 0)
            {
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
            
            
            
            
 /*
            else if (nread>8888){
                int number_of_chunks = nread / 8888;
                int last_chunk_size = nread % 8888;
                printf("number of chunks = %d\n",number_of_chunks);
                printf("last chunk size of chunks = %d\n",last_chunk_size);
                char *chararray[number_of_chunks+1];
                
                while( fgets (chararray[i++], 8888, fp)!=NULL ) {
                   chararray[i] =
                }
                write(connfd, chararray[i], 8888);
        }        **/
            
            
            
        }
        
        close(connfd);
        sleep(1);
    }
    
    
    return 0;
}
