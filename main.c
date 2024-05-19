#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

typedef struct {
  int socket_fd;
  struct sockaddr_in client_addr;
} client_t;

#define PORT 8080
#define MAX_CLIENTS 100

void *handle_client(void *arg) {
    client_t *client = (client_t *)arg;
    char request[1024];
    int bytes_read = read(client->socket_fd, request, sizeof(request));
    if (bytes_read <= 0) {
        // The client disconnected.
        close(client->socket_fd);
        return NULL;
    }

    char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, world!";
    int bytes_written = write(client->socket_fd, response, sizeof(response));
    if (bytes_written <= 0) {
        // The client disconnected.
        close(client->socket_fd);
        return NULL;
    }

    // Close the client connection.
    close(client->socket_fd);

    return NULL;
}

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Server socket fd: %d\n", fd);

    struct sockaddr_in addr = {
        AF_INET,
        htons(PORT),
        0
    };
    
    int option_value = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &option_value, sizeof(int));

    if(bind(fd, &addr, sizeof(addr)) != 0)
    {
        perror("Could not bind\n");
        return -1;
    }

    if(listen(fd, 10) != 0)
    {
        perror("Could not listen\n");
        return -1;
    }

    pthread_t threads[MAX_CLIENTS];
    int i = 0;

    while(1)
    {
        client_t client;
        socklen_t client_addr_len = sizeof(client.client_addr);
        client.socket_fd = accept(fd, (struct sockaddr *)&client.client_addr, &client_addr_len);
        if(client.socket_fd == -1)
        {
            perror("Could not accept");
            continue;
        }
        printf("Client socket fd: %d\n", client.socket_fd);

        printf("Creating Thread: %d\n", i);
        pthread_create(&threads[i], NULL, handle_client, &client);
        i++;
    }
    close(fd);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
