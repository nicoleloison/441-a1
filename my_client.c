/*GARANCE NICOLE LOISON
 CPSC 441
 ASSIGMENT 2
 ID 10083186*/



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
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>


/**For socket file descriptor identification*/
#define S1READY 0x01
#define SERVERIP " 127.0.0.0"
#define SERVERPORT 5000
#define DEVICE_SEND_PORT 5000
#define DEVICE_RECEIVE_PORT 5000

int msgcount;

int sockfd = 0, sockResult;
int octoleg_num =1;
int octoblock_num =1;
int bytesReceived = 0;
char recvBuff[1111];
char buf[1111];
struct sockaddr_in serv_addr;

static int  octoleg_rcv=0 ,octoblock_rcv=0, octoleg_ex=0,octoblock_ex=0, size_f=0, end_of_block=0, number_of_legs=0, end_of_file=0;
static int octoblock_size= 8888;
static int octoleg_size =1111;
static char info [100]= "";
static char txt_to_write [1111]="";

/********************************************/
int letter_to_value (char a ){
    
    if (a=='A')
    return 1;
    
    else if (a=='B')
    return 2;
    
    else if( a=='C')
    return 3;
    
    else if (a=='D')
    return 4;
    
    else if (a=='E')
    return 5;
    
    else if (a=='F')
    return 6;
    
    else if (a=='G')
    return 7;
    
    else if (a=='H')
    return 8;
    
    else
    return '\0';
    
}
/********************************************/
int waitToRead(int socket)
{
    fd_set fds;
    struct timeval timeout;
    /* Set time limit*/
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    
    int recv_result;
    int count =0 ;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO,&timeout,sizeof(timeout));
 
    while(1)
    {
        recv_result = recvfrom(socket, &buf, sizeof(buf), 0,(struct sockaddr*)&serv_addr, &serv_addr);
    
        if(recv_result > 0 && count<number_of_legs){
            printf(buf);
            return 1;
        }
        else{
            /**if timeout sendinfo**/
            sendto(socket, &info, sizeof(info), 0,(struct sockaddr*)&serv_addr, &serv_addr);
            return 1;
        }
    }
    
    
}
/********************************************/
void populate_array(char * val, char arr [] ) {
    for(int i = 0; i < sizeof(val); ++i){
        arr[i] = val[i];
    }
}
/********************************************/
void decode_info(){
    octoblock_rcv = letter_to_value(info[0]);
    octoleg_rcv = letter_to_value(info[2]);
    end_of_block = letter_to_value(info[5]);
    end_of_file = letter_to_value(info[6]);
}
/********************************************/
void update_expected(){
    if (octoleg_rcv <8 && end_of_block != 6){
        octoleg_ex = octoleg_rcv+1;
    }
    else if (octoleg_rcv ==8 ){
        octoleg_ex = 1;
    }
    if (octoblock_rcv<8){
        octoblock_ex = octoblock_rcv;
    }
    else {
        octoblock_ex = 1;
    }
}
/********************************************/
void check_expected(){
    if (octoleg_rcv!=octoleg_ex){
        printf("LEG MISSING:  %d \n", octoleg_ex);
        update_expected();
    }
    
    else if (octoblock_rcv!=octoblock_ex){
        printf("Rcvd unexpected bock %d \n",octoleg_ex );
    }
}


/****************read_the info from file****************************/
void read_info(char array []){
    char * separator = "\v";
    char * a = strtok(array, separator);
    populate_array(a, info);
    char * c = strtok(NULL, "");
    strcat(txt_to_write, c);
   
}
/********************************************/
bool receiveMessage()
{
    memset(recvBuff, '0', sizeof(recvBuff));
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
        printf("\n Error : No Server Present \n");
        return 1;
    }
    
    // Loop and listen for incoming message
    int counter = 0;
   
    while (1){
        /*wait at select to, read*/
        sockResult = waitToRead(sockfd);
        
        if(sockResult == 1)
        {
            bytesReceived = read(sockfd,recvBuff,sizeof(recvBuff));
            recvBuff[bytesReceived] = '\0';
            
            if(bytesReceived <0)
            {
                close(sockfd);
                return false;
            }
            /*size msg*/
            else if(bytesReceived <11 && bytesReceived>0 )
            {
                char* temp;
                temp = &recvBuff;
                size_f= atoi(recvBuff);
                printf("size of file : %d\n", size_f);
                if(size_f<octoleg_size){
                    number_of_legs=1;
                }
                else{
                    number_of_legs = (size_f / octoleg_size) +1;
                }
                printf("number of legs expected of file : %d\n", number_of_legs);
            }
            
            /*if we receive proper info*/
            else if(bytesReceived > 0)
            {
                /*empty txt to write every time*/
                txt_to_write[0]= '\0';
                info[0]='\0';
                char* ptr;
                ptr = &recvBuff;
                read_info(ptr);
              
                txt_to_write[bytesReceived -7] = '\0';/**clean up of info **/
                /** dealing with the legs and blocks **/
                decode_info();
                if(octoleg_rcv<8){
                    octoleg_ex++;
                }
                /**SYSTEMATICALLY SKIPPING NUMBER 5 FOR ASSIGNMENT PURPOSES**/
                else{
                    octoleg_ex=1;
                    octoblock_ex++;
                }
                if (info[6] =='F' ){
                    end_of_block=6;
                    end_of_file=6;
                }
                
                printf("info says leg is: %d at block was: %d  end of file: %d \n", letter_to_value(info[2]), letter_to_value(info[0]),letter_to_value(info[6]) );
                
                
                
                printf("expected leg is: %d and received was : %d \n", octoleg_ex, octoleg_rcv);
                check_expected();
        
                
                /*write into transmit file*/
                counter ++;
                FILE *fp;
                fp = fopen("transmit.txt", "a");
                if(NULL == fp)
                {
                    printf("Error opening file");
                    return 1;
                }
                fprintf(fp, "%s", txt_to_write);
                fclose(fp);
            }
            
            if (end_of_file!=0){
                printf("\n-finish writing-\n");
                break;
            }
        }
        
        memset(recvBuff, 0, sizeof(recvBuff));
        fflush(stdout);
    }
    /*closing socket*/
    close(sockfd);
    printf("[%s:%d#%s] Recieve socket closed:%s\n",
           __FILE__, __LINE__,__func__, strerror(errno));
    return true;
}

/********************************************/
int main(void)
{
    char str[10];
    msgcount = 1;
    pthread_t receiveThread;
    
    /*if a thread is */
    if(pthread_create(&receiveThread, NULL,(void *)&receiveMessage, NULL) != 0)
    {
        printf("[%s:%d#%s] thread create Failed(%s)\n",
               __FILE__, __LINE__,__func__, strerror(errno));
        return false;
    }
    pthread_join(receiveThread, NULL);


    return 0;
}


