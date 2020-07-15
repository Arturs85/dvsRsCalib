#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <vector>
#include <iostream>


using namespace std;
using namespace cv;


class CameraCalibration
{
public:
    CameraCalibration();
  static const int  arrayWidth = 240;
  static const int  arrayHeight = 180;
   int  capturedGoodFrames=0;
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
  bool addImageAsArray();//uint8_t (*array)[arrayWidth][arrayHeight]);
    std::vector<std::vector<cv::Point2f>> imagePoints;
    static void calcBoardCornerPositions(cv::Size boardSize, float squareSize, vector<cv::Point3f>& corners);
   void startCalibration();
void setStarted();

static void callBackFunc(int event, int x, int y, int flags, void *userdata);
void saveImage(cv::Mat mat);
};

#endif // CAMERACALIBRATION_H
