/************* UDP SERVER CODE *******************/

#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<string.h>
#include <errno.h>

int main(){
    
    int udpSocket, from_client, to_client;
    char buffer[2048];
    char file_buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size, cl_addr_size;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (UDP in this case) */
    udpSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(8000);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");/*IP address at university: 136.159.195.213*/
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    
     /*---- Bind the address struct to the socket ----*/
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    
    /*Initialize size variable to be used later on*/
    addr_size = sizeof serverStorage;
    cl_addr_size = sizeof clientAddr;
    
    while(1){
        
        from_client = recvfrom(udpSocket,buffer,2048,0,(struct sockaddr *)&serverStorage, &addr_size);
        printf("connected to client: file to send %s \n",buffer);
        
        /* SENDING FILE*/
        FILE *fp;
        fp=fopen(buffer,"r");
        if(fp==NULL)
        {
            printf("file does not exist\n");
            printf("ERROR: %s\n", strerror(errno));
        }
        
        fseek(fp,0,SEEK_END);
        size_t file_size=ftell(fp);
        fseek(fp,0,SEEK_SET);
   
        
        if(fread(file_buffer,file_size,1,fp)<=0)
        {
            printf("file could not be copied.\n");
            printf("ERROR: %s\n", strerror(errno));
            break;
        }
        
        char copy[1024];
        /* make sure strlen(name) >= 3 */
        strncpy(copy, file_buffer, 1024);
    
        
        to_client = sendto(udpSocket,copy,1024,0,(struct sockaddr *)&clientAddr, cl_addr_size);
       
        if(to_client <0){
            printf("file could not be sent.\n");
            printf("ERROR: %s\n", strerror(errno));
            printf("file length: %s\n",file_size);
            break;
        }
        
        bzero(file_buffer,sizeof(file_buffer));
        close(udpSocket);
    }
    
    return 0;
}
