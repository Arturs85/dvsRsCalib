#include "socketserver.h"
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//#include <pthread.h>

int SocketServer::server_fd;
int SocketServer::new_socket;
struct sockaddr_in SocketServer::address;
int SocketServer::opt =1;
pthread_t SocketServer::serverListenerThread;
pthread_t SocketServer::serverReceivingThread;

SocketServer::SocketServer()
{




    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    startAcceptingConnections();

}

void SocketServer::startAcceptingConnections()
{
    int iret1 = pthread_create( &serverListenerThread, NULL,connectionsListeningLoop , 0);

    if(iret1)
    {
        fprintf(stderr,"Error creating socket listening thread return code: %d\n",iret1);
        return;//exit(-1);
    }
    cout<< "started socket connection listening thread\n";
}

void SocketServer::startReceivingThread()
{
    int iret1 = pthread_create( &serverReceivingThread, NULL,receivingLoop , 0);

    if(iret1)
    {
        fprintf(stderr,"Error creating receiving thread return code: %d\n",iret1);
        return;//exit(-1);
    }
    cout<< "started socket receiving thread\n";
}

void *SocketServer::connectionsListeningLoop(void *arg)
{
    int addrlen = sizeof(address);
while(true){
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    cout<<"socket connected"<<new_socket <<endl;
    startReceivingThread();

}
}

void *SocketServer::receivingLoop(void *arg)
{
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    while(true){
        int  valread = read( new_socket , buffer, 1024);
        if(valread>=1){
            printf("%s\n",buffer );
           // send(new_socket , hello , strlen(hello) , 0 );
            printf("Hello message sent\n");

        }else{
            cout<<"receiving error"<<valread<<endl;
break;
        }
    }
}

void SocketServer::sendImage(uchar *data, int length)
{

    uchar* len = (uchar*)(&length);
   uchar lenInverted[4] = {((uchar*)(&length))[3],((uchar*)(&length))[2],((uchar*)(&length))[1],((uchar*)(&length))[0]};
    cout<<"len int"<<length<<"bytes : "<<(int)len[0]<<" "<<(int)len[1]<<" "<<(int)len[2]<<" "<<(int)len[3]<<endl;
    send(new_socket,lenInverted,4,0);
    send(new_socket,data,length,0);

}
