#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

#define MSG_SIZE 250

int main()
{
    int file,status;
    char msg[MSG_SIZE];

    // Open the process_list character device in READ ONLY mode.
    file = open("/dev/process_list",O_RDONLY);
    if(file<0)
    {
        perror("ERROR in opening device process_list");
        return 0;
    }

    // Looping till the EOF
    // Stop when read is 0
    status = read(file,msg,strlen(msg));
    while(status > 0)
    {
        printf("%s\n",msg);
        memset(msg, 0, sizeof(char)*MSG_SIZE);
        status = read(file, msg, strlen(msg));
    }
    if(status < 0)
    {
        perror("ERROR in reading from device process_list!");
        return 0;
    }
    
    printf("\n");
    
    // Close the Device
    close(file);
    return 0;
}
