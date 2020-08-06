#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <vector>
#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;
using namespace cv;


class SocketServer
{
public:
    SocketServer();
  static const int  PORT = 55555;
 static pthread_t serverListenerThread;
 static pthread_t serverReceivingThread;

  static void* connectionsListeningLoop(void* arg);
  static void* receivingLoop(void* arg);
static void sendImage(uchar* data, int length);
private:

 static int server_fd;
 static int new_socket;
 static struct sockaddr_in address;
 static int opt;

  void startAcceptingConnections();
 static void startReceivingThread();

};

#endif // SOCKETSERVER_H
