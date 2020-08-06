#ifndef SHAREDIMAGE_HPP
#define SHAREDIMAGE_HPP

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


using namespace std;
using namespace cv;


class SharedImage
{
public:
cv::Mat image;
pthread_mutex_t imageLock;
cv::Mat blockingCloneImage();
void blockingWriteImage();
};

Mat SharedImage::blockingCloneImage()
{
    pthread_mutex_lock(&imageLock);

    cv::Mat clonedImage = image.clone();
    pthread_mutex_unlock(&imageLock); //release image lock
    return clonedImage;
}

void SharedImage::blockingWriteImage(void){

}

#endif // SHAREDIMAGE_HPP
