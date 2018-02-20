/************* UDP SERVER CODE *******************/

#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<string.h>

int main(){
    
    
    int udpSocket, nBytes;
    char buffer[1024];
    char file_buffer[2000];
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

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
    
    while(1){
        
        nBytes = recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&serverStorage, &addr_size);
        printf("connected to client: file to send %s \n",buffer);
        
        /* SENDING FILE*/
        FILE *fp;
        fp=fopen(buffer,"r");
        if(fp==NULL)
        {
            printf("file does not exist\n");
        }
        
        fseek(fp,0,SEEK_END);
        size_t file_size=ftell(fp);
        fseek(fp,0,SEEK_SET);
        
        if(fread(file_buffer,file_size,1,fp)<=0)
        {
            printf("unable to copy file into buffer\n");
            return 0;
        }
        
        if(sendto(udpSocket,file_buffer,strlen(file_buffer),0,  (struct sockaddr *)&clientAddr, &addr_size)<0){
            printf("error in sending the file\n");
            break;
        }
        
        bzero(file_buffer,sizeof(file_buffer));
        close(udpSocket);
    }
    
    return 0;
}
