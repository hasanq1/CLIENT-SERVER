#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <cstring>
using namespace std;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

struct Request
{
    char* hostname;
    char* portno;
    string message;
    void setValues( char* hostname, char* portno, string message )
    {
        this->hostname = hostname;
        this->portno = portno;
        this->message = message;
    }
};

void* pthread_funct( void* Request_pointer )
{
    struct Request* Request_ptr = (struct Request*)Request_pointer;
    int portno = atoi(Request_ptr->portno);

    char buffer[256];
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        cerr<<"ERROR opening socket";
    server = gethostbyname(Request_ptr->hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        cerr<<"ERROR connecting";
    // cout
    bzero(buffer,256);

    strcpy(buffer, (Request_ptr->message).c_str());
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         cerr<<"ERROR reading from socket";
    printf("%s\n",buffer);
    return NULL;
}

int main(int argc, char *argv[])
{
    int size = 3;

    static struct Request* request = new struct Request[size];

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    pthread_t tid[size];

    string message[] = {"First thread", "Second thread", "Third Thread"};

    for(int i = 0; i < size; i++)
    {
        request[i].setValues( argv[1], argv[2], message[i]);
        if(pthread_create(&tid[i], NULL, pthread_funct, &request[i] )){
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    for(int i = 0; i < size; i++)
    {
        pthread_join(tid[i], NULL);
    }

    return 0;

}