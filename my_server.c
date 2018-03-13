/*GARANCE NIletterE LOISON
 ASSIGNMENT 2
 CPSC 441*/



#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>


static int octoleg_count=0, octoblock_count=1, octoleg_ex=0, end_of_block=0, end_of_file=0, number_of_legs=0, number_of_blocks=0;

static int octoblock_size= 8888;
static int octoleg_size =1111;
static char info [99]= {'\0'};
static char information [99]= {'\0'};
static char txt_to_send[1011]={'\0'};

/********************************************/
char value_to_letter (int a ){
    
    if (a==1)
    return 'A';
    
    else if (a ==2)
    return 'B';
    
    else if (a==3)
    return 'C';
    
    else if (a==4)
    return 'D';
    
    else if (a==5)
    return 'E';
    
    else if (a==6)
    return 'F';
    
    else if (a==7)
    return 'G';
    
    else if (a ==8)
    return 'H';
    
    else
    return 'o';
    
}

/********************************************/
int expected (int val) {
    if(val<8){
        end_of_block = 0;
        return val +1;
    }
    else{
        end_of_block = 6;
        return 1;
    }
}
/********************************************/
/*info is such
 0  1  2  3  4  5  6  7
 B  Lt L  Lx Be Fe \v \0 */

void update_info(){
    
    info[0]=value_to_letter(octoblock_count);
    info[1]=value_to_letter(number_of_legs);
    info[2]=value_to_letter(octoleg_count);
    info[3]=value_to_letter(octoleg_ex);
    info[4]=value_to_letter(end_of_block);
    info[5]=value_to_letter(end_of_file);
    info[6]='\v';
  
}

/********************************************/
/*info is such
 0  1  2  3  4  5  6  7
 B  Lt L  Lx Be Fe \v \0 */

void update_information(){

    FILE *in = fmemopen(information, sizeof(information), "w");
    fprintf(in,"%d", number_of_blocks);
    fprintf(in,"%d", octoblock_count);
    fprintf(in,"%d", octoleg_count);
    fprintf(in,"%d", end_of_block);
    fprintf(in,"%d", end_of_file);
    fprintf(in,"\v");
    fclose(in);
    printf("information is : %s\n", information);
}


/********************************************/
int main(void)
{
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    
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
        int size_f=size_file;
        /**sending file size **/
        char str_size[10];
        sprintf(str_size, "%d", size_f);
        printf("size is %s\n",str_size);
        
        /*write(connfd, str_size, sizeof(str_size));*/
        
        /*read all the file */
        unsigned char all_txt[size_file];
        int to_read = fread(all_txt,1,size_file,fp);
        number_of_blocks = size_file / octoblock_size;
        number_of_legs = size_file / octoleg_size;
        int last_block_size = size_file % octoblock_size;
        int last_leg_size = last_block_size % octoleg_size;
        
        if (last_block_size!=0){
            number_of_blocks ++;
            if (last_leg_size !=0){
                number_of_legs++;
            }
            
            printf("Total number of octolegs to send = %d\n",number_of_legs);
            printf("Total number of octoblock to send = %d\n",number_of_blocks);
            printf("Last leg size = %d\n",last_leg_size);

        }
        /*from txt to legs */
        char* temp_ptr;
        temp_ptr = &all_txt;
        
        char array_of_legs[number_of_legs][sizeof(txt_to_send)] ;
        char array_blocks[number_of_blocks][sizeof(txt_to_send)] ;
      
        for(int leg = 0; leg < number_of_legs; ++leg)
        {
            for(int letter = 0; letter < sizeof(txt_to_send); ++letter)
            {
                array_of_legs[leg][letter]=all_txt[leg*1011+letter];
            }
        }
        
      
        /*8 legs in blocks*/
        int counter =0;
        
        /* Read data from file and send it */
        do{
            if(octoleg_count<8){
                octoleg_count++;
            }
            /**SYSTEMATICALLY SKIPPING NUMBER 5 FOR ASSIGNMENT PURPOSES**/
            else if (octoleg_count==5){
                octoleg_count=7;
                counter ++;
            }
            else{
                octoleg_count=1;
                octoblock_count++;
            }
            
            counter ++;
            update_info();
            update_information();

    
            /*appending the info, block number and file size*/
            char to_send[1111] = { 0 };
            FILE *io = fmemopen(to_send, sizeof(to_send), "w");
            fprintf(io,information);
            fprintf(io,str_size);
            fprintf(io,"\v");
            fprintf(io,array_of_legs[counter]);
            fclose(io);
            
            
            /*Last leg to send*/
            if (counter == number_of_legs){
                end_of_block=6;
                end_of_file=6;
                update_info();
                update_information();
                write(connfd, to_send, sizeof(to_send));
                printf("sending leg : %d of block %d \n", octoleg_count, octoblock_count);
                printf("File sent \n");
                return 0;
            }
           
            
            printf("sending leg : %d of block %d \n", octoleg_count, octoblock_count);
            write(connfd, to_send, sizeof(to_send));
           
        }while(counter!= number_of_legs);
        
        close(connfd);
        return 0;
    }
    
    return 0;
}


