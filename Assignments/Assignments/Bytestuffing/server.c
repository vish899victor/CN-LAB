#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXSIZE 50

char flag[] = "flag";
char esc[] = "esc";

int main() {
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0) {
        perror("socket creation failed");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8080);

    if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(2);
    }

    if(listen(listenfd, 5) < 0) {
        perror("listen failed");
        exit(3);
    }

    printf("Server listening on port 8080...\n");

    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
    if(connfd < 0) {
        perror("connection failed");
        exit(4);
    }

    int n;
    read(connfd, &n, sizeof(int));

    char frame[MAXSIZE][MAXSIZE];
    for(int i = 0; i < n+2; i++)
    {
        read(connfd, frame[i], MAXSIZE);
    }

    printf("\n------------------------------\n\n");
    printf("Byte stuffing at receiver side:\n\n");
    printf("------------------------------\n\n");

    for(int i = 1; i < n+1; i++)
    {
        if(strcmp(frame[i], flag) == 0 || strcmp(frame[i], esc) == 0)
        {
            i++;
        }

        printf("%s", frame[i]);
    }

    printf("\n");

    close(connfd);
    close(listenfd);

    return 0;
}
