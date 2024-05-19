#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080); // Port 80 is the default port for HTTP
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 127.0.0.1 is the localhost address

    if (connect(fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        exit(1);
    }

    char request[] = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";

    if (send(fd, request, strlen(request), 0) < 0) {
        perror("send");
        exit(1);
    }

    char response[1024];
    int n = recv(fd, response, sizeof(response), 0);
    if (n < 0) {
        perror("recv");
        exit(1);
    }

    // Print the response to the console
    printf("%s", response);

    // Close the socket
    close(fd);

    return 0;

}
