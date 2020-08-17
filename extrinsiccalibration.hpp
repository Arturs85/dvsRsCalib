#ifndef EXTRINSICCALIBRATION_HPP
#define EXTRINSICCALIBRATION_HPP

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <vector>
#include <iostream>

#include "sharedimage.hpp"

using namespace std;
using namespace cv;


class ExtrinsicCalibration
{
public:
    ExtrinsicCalibration(std::string dvsFileNameYAML,std::string rsFileNameYAML,std::string outFileNameYAML);
    static const int  arrayWidth = 240;
    static const int  arrayHeight = 180;
    constexpr static const double  dtMax = 0.1;//sec

    SharedImage* currentDvsImage=0;
    SharedImage* currentRsImage=0;

    string dvsFileNameYAML;
    string rsFileNameYAML;
    string outFileNameYAML;
    int  capturedGoodFramePairs=0;
    int totalFramesChecked=0;
    static const int  REQUIRED_NUMBER_OF_FRAMES = 5;//25?
    bool isEnoughFrames = false;
    bool isCalibrationCalculated = false;
    std::string winName = "Camera view";
    cv::Size boardSize=cv::Size(8,6);
    cv::Size imageSize=cv::Size(arrayWidth,arrayHeight);
    static bool setToSave;
    int imgCounter = 0;
    float squareSize= 25;//mm ???????????????????
    int cameraID=2;
    VideoCapture inputCapture;
    bool checkCorners(Mat cvImage);
    bool addDvsImage(SharedImage *dvsImage);//uint8_t (*array)[arrayWidth][arrayHeight]);
    bool addRsImage(SharedImage *rsImage);

    std::vector<std::vector<cv::Point2f>> imagePointsRs;
    std::vector<std::vector<cv::Point2f>> imagePointsDvs;
vector<cv::Point2f> pointBufDvs,pointBufRs;
    static void calcBoardCornerPositions(cv::Size boardSize, float squareSize, vector<cv::Point3f>& corners);
    void startCalibration();
    void setStarted();

    static void callBackFunc(int event, int x, int y, int flags, void *userdata);
    void saveImage(cv::Mat mat);
};

#endif // EXTRINSICCALIBRATION_HPP
