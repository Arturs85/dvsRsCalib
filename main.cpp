#include <iostream>
#include "cameracalibration.h"
//#include "recievingthread.h"
#include <pthread.h>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <unistd.h>
#include <iomanip>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

// for valid images for calibration
#define DVS_CAL_OUT_FOLDER "/"
#define RS_CAL_OUT_FOLDER "/"
#define MAX_FRAME_TIME_DIFF 0.2

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
//returns mod time
double getModTime(std::string file){

    std::string command = "lsof "+file;

    std::string res = exec(command.c_str());
    //if output is emty than file is ready for opening
    if(res.size()>1)
        return 0;
    std::string timeModifiedCommand = "stat --format='%.Y' "+file;
    std::string resTime = exec(timeModifiedCommand.c_str());



    std::cout<<"open: "<<res<<"\n"<<std::flush;
    std::cout<<"mofified: "<<resTime<<"\n"<<std::flush;

    resTime = resTime.replace(resTime.find(","), 1, ".");
    double timeModifiedD = std::stod(resTime);
    return timeModifiedD;
}

int main(int argc, char *argv[])
{

    std::cout<<"OpenCV Version used:"<<CV_MAJOR_VERSION<<"."<<CV_MINOR_VERSION<<std::endl;
    //cv::Mat inputImage;
    //std::cout<<" empty:"<< inputImage.empty()<<"\n";
    // // cv::namedWindow( "Display window", CV_WINDOW_AUTOSIZE );
    //inputImage = cv::imread("/home/vnpc/Pictures/mill.png");

    //std::cout<<" cols:"<< inputImage.cols<<"\n";
    //std::cout<<" empty after read:"<< inputImage.empty()<<"\n";

    //if(!inputImage.empty()) {

    //   cv::namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
    //cv::imshow("Display Image", inputImage);
    //    cv::waitKey(0);
    //}
    //  //cv::Mat matInput = cv::imread("grafiks.png");


    // check if input file exists and is closed
    CameraCalibration dvsCalibration;

    std::string dvsFileName = "/home/vnpc/Pictures/mill.png";
    std::string rsFileName = "/home/vnpc/Pictures/mill.png";

    double dvsTime = getModTime(dvsFileName);
    double rsTime = getModTime(rsFileName);
    if(rsTime>1 && dvsTime>1){
        //check if rs and dvs times are close
        if(std::abs(rsTime-dvsTime)<0.2){

            cv::Mat dvsImage=cv::imread(dvsFileName);
            bool hasCornersDvs = dvsCalibration.checkCorners(dvsImage);
            cv::Mat rsImage = cv::imread(rsFileName);
            bool hasCornersRs = dvsCalibration.checkCorners(rsImage);
            // store both images for latter calibration


        }


    }

    //usleep(500000);
    //}
    char a;

    //CameraCalibration cameraCalibration;


    //pthread_t inc_x_thread;

    /* create a second thread which executes inc_x(&x) */
    //if(pthread_create(&inc_x_thread, NULL, RecievingThread::run,0)) {

    //fprintf(stderr, "Error creating thread\n");
    //return 1;

    //}else{
    //    std::cout<<" starting dvs thread"<<"\n";

    //}

    std::cin>>a;



    return 1;
}

