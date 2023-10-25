#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <ctime>
using namespace std;

#define TRUE 1
#define FALSE 0
#define PORT "65001"

void wait()
{
    int remaining_time = 60; // 60 seconds countdown
    time_t start_time = time(nullptr);

    while (remaining_time > 0)
    {
        cout << "\rTime remaining: " << remaining_time << " seconds" << flush;
        sleep(1);

        time_t current_time;
        do
        {
            current_time = time(nullptr);
        } while (current_time == start_time);

        remaining_time--;
    }
    cout << "\rYou are done!\n";
}

void replace(char s[])
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (s[i] == '\n')
        {
            s[i] = '_';
        }
        i++;
    }
}

int main(int argc, char *argv[])
{
    int counter = 0;
    if (argc < 2)
    {
        cerr << "Format: client hostname" << endl;
        exit(1);
    }

    char *host = argv[1];

    cout << "\nAttendance Tracker Server has started " << host << "...";
    struct addrinfo addressHints;
    memset(&addressHints, 0, sizeof(addressHints));
    addressHints.ai_family = AF_INET;
    addressHints.ai_socktype = SOCK_STREAM;

    struct addrinfo *server;
    int result = getaddrinfo(host, PORT, &addressHints, &server);
    if (result != 0)
    {
        perror("failed");
        exit(1);
    }
    cout << "\nClass Found!" << endl;
    cout << "\nEnter your Enrollment Number" << endl;

    int sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    if (sockfd == -1)
    {
        perror("failed");
        exit(1);
    }

    result = connect(sockfd, server->ai_addr, server->ai_addrlen);
    freeaddrinfo(server);
    if (result == -1)
    {
        perror("failed");
        exit(1);
    }

    int done = FALSE;
    fd_set read_fd;
    while (!done)
    {
        FD_ZERO(&read_fd);
        FD_SET(sockfd, &read_fd);
        FD_SET(0, &read_fd);

        result = select(sockfd + 1, &read_fd, nullptr, nullptr, nullptr);
        if (result == -1)
        {
            perror("failed");
            exit(1);
        }

        char buffer[BUFSIZ];
        if (FD_ISSET(sockfd, &read_fd))
        {
            sleep(1);
            result = recv(sockfd, buffer, BUFSIZ, 0);

            if (result < 1)
            {
                cout << "Connection closed by peer" << endl;
                break;
            }

            buffer[result] = '\0';
            cout << buffer;
        }

        if (FD_ISSET(0, &read_fd))
        {
            if (fgets(buffer, BUFSIZ, stdin) == nullptr)
            {
                putchar('\n');
            }
            else if (strcmp(buffer, "close\n") == 0)
            {
                done = TRUE;
            }
            else
            {
                if (counter == 0)
                {
                    replace(buffer);
                    send(sockfd, buffer, strlen(buffer), 0);
                    counter++;
                    cout << "\nYour timer has started!";
                    wait();
                    replace(buffer);
                    strcat(buffer, "Present\n");
                    send(sockfd, buffer, strlen(buffer), 0);
                    cout << "\nYour Attendance has been marked!" << endl;
                    break;
                }
            }
        }
    }

    cout << "\nDisconnected" << endl;
    close(sockfd);

    return 0;
}