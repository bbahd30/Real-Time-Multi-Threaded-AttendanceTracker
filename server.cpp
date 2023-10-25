#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fstream>
using namespace std;

#define yes 1
#define no 0

void split_string(const char *str, char *classs, char *name, char *roll)
{
    char *token;
    int i = 0;
    char tempStr[BUFSIZ];
    strcpy(tempStr, str);

    token = strtok(tempStr, "_");
    while (token != nullptr)
    {
        switch (i)
        {
        case 0:
            strcpy(classs, token);
            break;
        case 1:
            strcpy(name, token);
            break;
        case 2:
            strcpy(roll, token);
            break;
        }
        i++;
        token = strtok(nullptr, "_");
    }
}

void addStudentsToCSV(const char *classs, const char *name, const char *roll)
{
    ofstream file("students.csv", ios::app);
    if (!file.is_open())
    {
        cerr << "Error opening file" << endl;
        return;
    }

    file << name << "\t" << roll << "\t" << classs << "\t" << endl;
}

bool isPresent(const char *buffer)
{
    const char *presentText = "Present";
    const char *found = strstr(buffer, presentText);
    return found != nullptr;
}

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

    char classs[8], roll[8];

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
    bool present = false;
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

                            split_string(buffer, classs, name, roll);
                            if (isPresent(buffer))
                                addStudentsToCSV(classs, name, roll);
                            strcpy(classs, "");
                            strcpy(name, "");
                            strcpy(roll, "");

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
