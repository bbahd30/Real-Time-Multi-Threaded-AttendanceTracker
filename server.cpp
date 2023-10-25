#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fstream>
using namespace std;

int main()
{
    const int clientname_size = 32;
    char clientname[clientname_size];

    const int backlog = 10;
    char connection[backlog][clientname_size];

    struct sockaddr address;
    int result, fd, x, done;
    fd_set main_fd, read_fd;
    int serverfd, clientfd;

    char classs[8], div[8], nam[20], roll[5], attendance[10];

    int details_counter = 0;
    char name[60];
    char namee[60];

    struct addrinfo addressHints, *server;
    memset(&addressHints, 0, sizeof(struct addrinfo));
    addressHints.ai_family = AF_INET;
    addressHints.ai_socktype = SOCK_STREAM;
    addressHints.ai_flags = AI_PASSIVE;

    const char *port = "65001";
    result = getaddrinfo(nullptr, port, &addressHints, &server);
    if (result != 0)
    {
        perror("failed");
        exit(1);
    }

    serverfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    if (serverfd == -1)
    {
        perror("failed");
        exit(1);
    }

    result = bind(serverfd, server->ai_addr, server->ai_addrlen);
    if (result == -1)
    {
        perror("failed");
        exit(1);
    }

    cout << "Attendance Tracker is tracking students now..." << endl;
    result = listen(serverfd, backlog);
    if (result == -1)
    {
        perror("failed");
        exit(1);
    }


    cout << "\nClosing server." << endl;
    close(serverfd);
    freeaddrinfo(server);
    return 0;
}
