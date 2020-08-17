#include "intrinsiccalibration.hpp"

#include <unistd.h>



IntrinsicCalibration::IntrinsicCalibration(std::string fileNameYAML)
{this->fileNameYAML=fileNameYAML;
    //                inputCapture.open(cameraID);
    //                while(true){
    //                    addImageAsArray();
    //                    usleep(10000);

    //                }
}

bool IntrinsicCalibration::checkCorners(cv::Mat cvImage)
{
    bool found =false;
    vector<cv::Point2f> pointBuf;

    int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;
    chessBoardFlags |= cv::CALIB_CB_FAST_CHECK;

    found = findChessboardCorners( cvImage, cv::Size(8,6), pointBuf, chessBoardFlags);
    return found;
}

bool IntrinsicCalibration::addImage(cv::Mat cvImage)//uint8_t (*array)[CameraCalibration::arrayWidth][CameraCalibration::arrayHeight])
{
    if(isEnoughFrames)
        return isEnoughFrames;

    // cv::Mat cvImage;//(arrayWidth, arrayHeight, CV_8UC1, array);
    // inputCapture >> cvImage;

    //   cv::namedWindow( winName, WINDOW_AUTOSIZE );
    //  cv::setMouseCallback(winName, callBackFunc);
    vector<cv::Point2f> pointBuf;
    //cv::Vec<float,2> pointBuf;
    if(setToSave){
        saveImage(cvImage);
        setToSave = false;
    }
    bool found =false;
    int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;
    chessBoardFlags |= cv::CALIB_CB_FAST_CHECK;

    found = findChessboardCorners( cvImage, cv::Size(8,6), pointBuf, chessBoardFlags);
    if(found)
    {
        cv::cornerSubPix( cvImage, pointBuf, cv::Size(11,11),cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
        capturedGoodFrames++;
        imagePoints.push_back(pointBuf);
        if(capturedGoodFrames>=REQUIRED_NUMBER_OF_FRAMES)
            isEnoughFrames=true;
    }

    cv::drawChessboardCorners( cvImage,  cv::Size(8,6), cv::Mat(pointBuf), found );
    //   imshow( winName, cvImage );                   // Show our image inside it.
    //cv::waitKey(30);
    //  cout<<"found so far: "<<capturedGoodFrames<<endl;
    totalFramesChecked++;
    return isEnoughFrames;


}

void IntrinsicCalibration::calcBoardCornerPositions(cv::Size boardSize, float squareSize, vector<cv::Point3f> &corners)
{
    corners.clear();
    for( int i = 0; i < boardSize.height; ++i )
        for( int j = 0; j < boardSize.width; ++j )
            corners.push_back(Point3f(j*squareSize, i*squareSize, 0));

}

void IntrinsicCalibration::startCalibration()
{
    vector<vector<cv::Point3f> > objectPoints(1);
    calcBoardCornerPositions(boardSize, squareSize, objectPoints[0]);
    objectPoints.resize(imagePoints.size(),objectPoints[0]);

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    cameraMatrix.at<double>(0,0) = 1;//s.aspectRatio;

    Mat distCoeffs = Mat::zeros(8, 1, CV_64F);

    vector<Mat> rvecs, tvecs;

    double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                                 distCoeffs, rvecs, tvecs, CALIB_FIX_ASPECT_RATIO|CALIB_FIX_K4|CALIB_FIX_K5);


    cout<<"calibration calculated with error of: "<<rms<<endl;

    cout<< cameraMatrix.at<double>(0,0)<<" "<<cameraMatrix.at<double>(0,1)<<" "<<cameraMatrix.at<double>(0,2)<<endl;
    cout<< cameraMatrix.at<double>(1,0)<<" "<<cameraMatrix.at<double>(1,1)<<" "<<cameraMatrix.at<double>(1,2)<<endl;
    cout<< cameraMatrix.at<double>(2,0)<<" "<<cameraMatrix.at<double>(2,1)<<" "<<cameraMatrix.at<double>(2,2)<<endl;

    cv::FileStorage dvsFs(fileNameYAML,cv::FileStorage::WRITE);
    dvsFs<< "K" << cameraMatrix;
    dvsFs<< "D" << distCoeffs;

    isCalibrationCalculated = true;
}

void IntrinsicCalibration::setStarted()
{
    //    isCalibrationStarted=true;

}
bool IntrinsicCalibration::setToSave = false;
void IntrinsicCalibration::callBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        // if (button.contains(Point(x, y)))
        {
            cout << "Clicked!" << endl;
            setToSave = true;
            //  rectangle(canvas(button), button, Scalar(0,0,255), 2);
        }
    }
    if (event == EVENT_LBUTTONUP)
    {
        //rectangle(canvas, button, Scalar(200, 200, 200), 2);
    }

    // imshow(winName, canvas);
    waitKey(1);
}

void IntrinsicCalibration::saveImage(cv::Mat mat)
{
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    bool result = false;

    std::string filename = "1-"+std::to_string(imgCounter)+".png";
    imgCounter++;

    try
    {
        result = imwrite(filename, mat, compression_params);
    }
    catch (const cv::Exception& ex)
    {
        fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
    }
    if (result)
        printf("Saved PNG file with alpha data.\n");
    else
        printf("ERROR: Can't save PNG file.\n");
    // return result ? 0 : 1;
}
