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
    
    char users[64];
    int num_users = 0;


    while (1)
    {
        printf("Waiting for new client.\n");
        int client = accept(listener, NULL, NULL);
        printf("New client accepted: %d.\n", client);
        if (fork() == 0)
        {   
            char *msg = "Nhap tai khoan va Mat khau theo cu phap: \nTK MK\n";
            send(client, msg, strlen(msg), 0);
            close(listener);
            char buf[256];
            while (1)
            {
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0){
                    close(client);
                    exit(0);
                    for (int i = 0; i < num_users; i++){
                        if (users[i] == client) users[i] = -1;
                    }
                    break;
                    
                }
                buf[ret] = 0;
                printf("Received from %d: %s\n", client, buf);

                int i =0;
                for (;i<num_users;i++){
                    if (users[i]==client) break;
                }

                // Chua dang nhap
                if (i==num_users){
                    
                    char user_name[32], password[32], temp[66], line[66];
                    int count = sscanf(buf, "%s%s%s", user_name, password,temp);

                    if (count ==2 ){
                        
                        sprintf(temp, "%s %s\n", user_name  , password);
                        FILE *f = fopen("users.txt", "r");
                        int found = 0;
                        while (fgets(line, sizeof(line), f) != NULL)
                        {
                            if (strcmp(line, temp) == 0)
                            {
                                found = 1;
                                break;
                            }
                        }
                        fclose(f);

                        if (found == 1){
                            char *msg = "Dang nhap thanh cong. Hay nhap lenh de thuc hien.\n";
                            send(client, msg, strlen(msg), 0);
                            users[num_users] = client;
                            num_users++;
                        }
                        else{
                            send(client, "Dang nhap that bai. Hay thu lai\n", strlen("Dang nhap that bai. Hay thu lai\n"), 0);
                        }
                    }
                    else {
                        char *msg = "Khong dung cu phap\n";
                        send(client, msg, strlen(msg), 0);
                    }
                }
                else {
                    // Da dang Nhap
                    char tmp[256];

                    // Xoa dau xuong dong neu co
                    if (buf[strlen(buf) - 1] == '\n')
                        buf[strlen(buf) - 1] = '\0';

                    sprintf(tmp, "%s > out.txt", buf);
                    int ret = system(tmp);

                    if (ret == 0)
                    {
                        FILE *f = fopen("out.txt", "rb");
                        while (!feof(f))
                        {
                            ret = fread(tmp, 1, sizeof(tmp), f);
                            if (ret <= 0)
                                break;
                            send(client, tmp, ret, 0);
                        }
                        fclose(f);
                    }
                    else
                    {
                        char *msg = "Lenh khong thuc hien duoc.\n";
                        send(client, msg, strlen(msg), 0);
                    }
                }
            }
        }

        close(client);
    }
    
    close(listener);    
    return 0;
}