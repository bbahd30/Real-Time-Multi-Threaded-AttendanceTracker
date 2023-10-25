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

    int max_connect = backlog;
    FD_ZERO(&main_fd);
    FD_SET(serverfd, &main_fd);
    done = no;

    socklen_t address_len = sizeof(struct sockaddr);
    while (!done)
    {
        read_fd = main_fd;
        result = select(max_connect + 1, &read_fd, nullptr, nullptr, nullptr);
        if (result == -1)
        {
            perror("failed");
            exit(1);
        }

        char buffer[BUFSIZ], sendstr[BUFSIZ];
        for (fd = 1; fd <= max_connect; fd++)
        {
            if (FD_ISSET(fd, &read_fd))
            {
                if (fd == serverfd)
                {
                    clientfd = accept(serverfd, (struct sockaddr *)&address, &address_len);
                    if (clientfd == -1)
                    {
                        perror("failed");
                        exit(1);
                    }
                    result = getnameinfo((struct sockaddr *)&address, address_len, clientname, clientname_size, nullptr, 0, NI_NUMERICHOST);

                    strcpy(connection[clientfd], clientname);

                    FD_SET(clientfd, &main_fd);

                    strcpy(buffer, "\nAttendance Tracker -> ");
                    strcat(buffer, " connected");
                    send(clientfd, buffer, strlen(buffer), 0);
                    strcpy(buffer, "\nAttendance Tracker -> ");
                    strcat(buffer, connection[clientfd]);
                    strcat(buffer, " has joined the server\n");

                    cout << buffer << endl;
                }
                else
                {
                    result = recv(fd, buffer, BUFSIZ, 0);
                    if (result < 1)
                    {
                        FD_CLR(fd, &main_fd);
                        close(fd);

                        strcpy(buffer, "\nAttendance Tracker ");
                        strcat(buffer, connection[fd]);
                        strcat(buffer, " disconnected\n");
                        for (x = serverfd + 1; x < max_connect; x++)
                        {
                            if (FD_ISSET(x, &main_fd))
                            {
                                send(x, buffer, strlen(buffer), 0);
                            }
                        }
                        cout << buffer << endl;
                    }
                    else
                    {
                        buffer[result] = '\0';
                        if (strcmp(buffer, "shutdown\n") == 0)
                        {
                            done = yes;
                        }
                        else
                        {
                            strcpy(sendstr, connection[fd]);
                            strcat(sendstr, "-> ");
                            strcat(sendstr, buffer);

                            split_string(buffer, classs, div, nam, roll, attendance);

                            strcpy(classs, "");
                            strcpy(div, "");
                            strcpy(nam, "");
                            strcpy(roll, "");
                            strcpy(attendance, "");

                            cout << sendstr << endl;
                        }
                    }
                }
            }
        }
    }


    cout << "\nClosing server." << endl;
    close(serverfd);
    freeaddrinfo(server);
    return 0;
}
