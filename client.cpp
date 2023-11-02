#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <thread>
#include <mutex>

using namespace std;

#define TRUE 1
#define FALSE 0
#define PORT "65001"

mutex mtx;

void wait()
{
    int remaining_time = 10;
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
    cout << "\rYou are done!                                   \n";
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

void serverCommunication(int sockfd)
{
    char buffer[BUFSIZ];

    while (true)
    {
        int result = recv(sockfd, buffer, BUFSIZ, 0);

        if (result < 1)
        {
            lock_guard<mutex> lock(mtx);
            cout << "Connection closed by the server" << endl;
            break;
        }

        buffer[result] = '\0';
        cout << buffer;
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
    cout << "\nEnter your class, name, enrollment number and code separated by underscore." << endl;

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

    thread communicationThread(serverCommunication, sockfd);

    int done = FALSE;
    while (!done)
    {
        char buffer[BUFSIZ];
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

    {
        lock_guard<mutex> lock(mtx);
        cout << "\nDisconnected" << endl;
    }

    communicationThread.join();
    close(sockfd);

    return 0;
}
