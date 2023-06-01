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
    while (1){

        printf("Wating for new client.\n");
        int client = accept(listener, NULL,NULL);
        printf("New client connected : %d\n",client);

        if (fork() == 0)
        {   
            while(1)
            {
                char buf[256];
                int ret = recv(client, buf, sizeof(buf),0);

                if (ret <=0)
                {
                    break;
                }
                buf[ret] = 0;

                printf("Received from client %d:  %s\n",client,buf);
                char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac ban!</h1></body></html>";
                send(client, msg, strlen(msg), 0);
                close(client);
                exit(0);
            }
        }
        
    }
}
