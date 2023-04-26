#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXSIZE 50

char flag[] = "flag";
char esc[] = "esc";

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2) {
        printf("Usage: %s <Server IP Address>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("socket creation failed");
        exit(2);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);

    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("Invalid address or address not supported\n");
        exit(3);
    }

    if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connection failed");
        exit(4);
    }

    char str[MAXSIZE][MAXSIZE];
    int n;

    printf("Enter length of String : ");
    scanf("%d", &n);
    getchar();

    printf("Enter the String: \n");
    for (int i = 0; i < n; i++)
    {
        fgets(str[i], MAXSIZE, stdin);
        str[i][strcspn(str[i], "\n")] = 0; // remove newline character
    }

    printf("\nYou entered :\n");
    for (int i = 0; i < n; i++)
    {
        printf("%s\n", str[i]);
    }
    printf("\n");

    char frame[MAXSIZE][MAXSIZE];
    int k = 0;

    strcpy(frame[k++], flag);
    for (int i = 0; i < n; i++)
    {
        int len = strlen(str[i]);
        for (int j = 0; j < len; j++)
        {
            if (str[i][j] == flag[0])
            {
                strcpy(frame[k++], esc);
                strcpy(frame[k++], flag);
            }
            else if (str[i][j] == esc[0])
            {
                strcpy(frame[k++], esc);
                strcpy(frame[k++], esc);
            }
            else
            {
                char ch[2] = {str[i][j], '\0'};
                strcpy(frame[k++], ch);
            }
        }
    }
    strcpy(frame[k++], flag);

    printf("------------------------------\n\n");
    printf("Byte stuffing at sender side:\n\n");
    printf("------------------------------\n\n");
    for (int i = 0; i < k; i++)
    {
        printf("%s\t", frame[i]);
    }

    printf("\n\n");

    write(sockfd, &n, sizeof(int));

    for(int i = 0; i < k; i++)
    {
        write(sockfd, frame[i], MAXSIZE);
    }

    printf("Message sent successfully\n");

    close(sockfd);

    return 0;
}
