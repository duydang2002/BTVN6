#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
void signalHandler(int signo)
{
    int pid = wait(NULL);
    printf("Child %d terminated.\n", pid);
}

int main()
{
   int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9090);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) 
    {
        perror("listen() failed");
        return 1;
    }

    signal(SIGCHLD, signalHandler);
    char *format1 = "dd/mm/yyyy";
    char *format2 = "dd/mm/yy";
    char *format3 = "mm/dd/yyyy";
    char *format4 = "mm/dd/yy";
    while (1){

        printf("Wating for new client.\n");
        int client = accept(listener, NULL,NULL);
        printf("New client connected : %d\n",client);

        if (fork() == 0)
        {   
            time_t t = time(NULL);
            struct tm tm= *localtime(&t);
            while(1)
            {
                char buf[512];
                int ret = recv(client, buf, sizeof(buf),0);

                if (ret <=0)
                {
                    close(client);
                    exit(0);
                    break;
                }
                buf[ret] = 0;
                // Kiem tra xau dau vao.
                char cmd[32],format[32],tmp[256];
                int count = sscanf(buf, "%s%s%s",cmd,format,tmp);
                if (count == 2  ){
                    if (strcmp(cmd,"GET_TIME") == 0 )
                    {
                        if (strcmp(format,format1) == 0)
                        {
                            strftime(buf, sizeof(buf), "%d/%m/%Y", &tm);
                            sprintf(buf,"%s\n",buf);
                            send(client, buf, strlen(buf), 0);  
                        }
                        else if (strcmp(format,format2) == 0)
                        {
                            strftime(buf, sizeof(buf), "%d/%m/%y", &tm);
                            sprintf(buf,"%s\n",buf);
                            send(client, buf, strlen(buf), 0);  
                        }
                         else if (strcmp(format,format3) == 0)
                         {
                            strftime(buf, sizeof(buf), "%m/%d/%Y", &tm);
                            sprintf(buf,"%s\n",buf);
                            send(client, buf, strlen(buf), 0);  
                        } 
                        else if (strcmp(format,format4) == 0)
                        {
                            strftime(buf, sizeof(buf), "%m/%d/%y", &tm);
                            sprintf(buf,"%s\n",buf);
                            send(client, buf, strlen(buf), 0);  
                        }
                        else{
                            char *msg = "Nhap sai format yeu cau nhap dung format\n";
                            send(client, msg, strlen(msg), 0);  
                        }
                    }
                }
                else
                {
                    char *msg = "Nhap sai format yeu cau nhap dung format\n";
                    send(client, msg, strlen(msg), 0);    
                }
                printf("Received from client %d:  %s\n",client,buf);
               
            }
        }
        
    }
}
