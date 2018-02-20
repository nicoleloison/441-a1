/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include<netinet/in.h>
#include<sys/types.h>
#include <string.h>
#include <errno.h>


int main(){
    
    int clientSocket, from_server,to_server;
    char buffer[2048];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    /*Create UDP socket*/
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
    
    /*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port =htons(8000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");/*IP adress at university: 136.159.195.213*/
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    
    /*Initialize size variable to be used later on*/
    addr_size = sizeof serverAddr;
    
    
    while(1){
        
        printf("Type the name of the file to transfer\n");
        scanf("%s",buffer);
        
        /*send  req of file to server*/
        sendto(clientSocket, buffer, strlen(buffer), 0,
               (struct sockaddr *)&serverAddr, sizeof(struct sockaddr));
        
        char file_buffer[2048];
        from_server=recvfrom(clientSocket, buffer, 2048, 0,(struct sockaddr *)&serverAddr,sizeof(struct sockaddr));
        while(from_server>0)
        {
            write(1, file_buffer, from_server);
        }
        
        if (from_server<0)
        {   printf("ERROR: %s\n", strerror(errno));
            break;
        }
        
        char new_file[]="copied";
        strcat(new_file,file_buffer);
       
        FILE *fp;
        fp=fopen(new_file,"w+");
        if(fwrite(file_buffer,1,sizeof(file_buffer),fp)<0)
        {
            printf("error writting file\n");
            printf("ERROR: %s\n", strerror(errno));
            exit(1);
        }
        
        
        close(clientSocket);
    }
    
    return 0;
}
