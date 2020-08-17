#include "extrinsiccalibration.hpp"

#include <unistd.h>



ExtrinsicCalibration::ExtrinsicCalibration(string dvsFileNameYAML, string rsFileNameYAML, string outFileNameYAML)
{
    this->dvsFileNameYAML = dvsFileNameYAML;
    this->rsFileNameYAML = rsFileNameYAML;
    this->outFileNameYAML = outFileNameYAML;

}

bool ExtrinsicCalibration::checkCorners(cv::Mat cvImage)
{
    bool found =false;
    vector<cv::Point2f> pointBuf;

    int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;
    chessBoardFlags |= cv::CALIB_CB_FAST_CHECK;

    found = findChessboardCorners( cvImage, cv::Size(8,6), pointBuf, chessBoardFlags);
    return found;
}

bool ExtrinsicCalibration::addDvsImage(SharedImage* dvsImage)
{
    //check corners
    bool found =false;
    int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;
    chessBoardFlags |= cv::CALIB_CB_FAST_CHECK;
    pointBufDvs.clear();
    found = findChessboardCorners( dvsImage->image, cv::Size(8,6), pointBufDvs, chessBoardFlags);
    if(found)
    {
        currentDvsImage = dvsImage;
        cv::drawChessboardCorners( currentDvsImage->image,  cv::Size(8,6), cv::Mat(pointBufDvs), found );

        //check if there is good recent rs image
        if(currentRsImage!=0 && std::abs(currentRsImage->timeModifiedSec-dvsImage->timeModifiedSec)<dtMax){
            //store both images, get points for stereo calibration,
            cv::cornerSubPix( currentRsImage->image, pointBufRs, cv::Size(11,11),cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
            imagePointsRs.push_back(pointBufRs);
            cv::cornerSubPix( currentDvsImage->image, pointBufRs, cv::Size(11,11),cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
            imagePointsDvs.push_back(pointBufDvs);
            capturedGoodFramePairs++;

            delete (currentDvsImage);
            currentDvsImage=0;

            if(capturedGoodFramePairs>=REQUIRED_NUMBER_OF_FRAMES){
                isEnoughFrames=true;
            }
        }
        delete (currentRsImage);//delete the other image, it is too old anyway
        currentRsImage=0;
    }
    return isEnoughFrames;
}

bool ExtrinsicCalibration::addRsImage(SharedImage* rsImage)
{
    //check corners
    bool found =false;
    int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;
    chessBoardFlags |= cv::CALIB_CB_FAST_CHECK;
    pointBufDvs.clear();
    found = findChessboardCorners( rsImage->image, cv::Size(8,6), pointBufRs, chessBoardFlags);
    if(found)
    {
        currentRsImage = rsImage;
        cv::drawChessboardCorners( currentRsImage->image,  cv::Size(8,6), cv::Mat(pointBufRs), found );

        //check if there is good recent rs image
        if(currentDvsImage!=0 && std::abs(currentDvsImage->timeModifiedSec-rsImage->timeModifiedSec)<dtMax){
            //store both images, get points for stereo calibration,
            cv::cornerSubPix( currentRsImage->image, pointBufRs, cv::Size(11,11),cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
            imagePointsRs.push_back(pointBufRs);
            cv::cornerSubPix( currentDvsImage->image, pointBufRs, cv::Size(11,11),cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
            imagePointsDvs.push_back(pointBufDvs);
            capturedGoodFramePairs++;
            delete (currentRsImage);
            currentRsImage=0;

            if(capturedGoodFramePairs>=REQUIRED_NUMBER_OF_FRAMES){
                isEnoughFrames=true;
            }
        }
        delete (currentDvsImage);//delete the other image, it is too old anyway
        currentDvsImage=0;
    }
    return isEnoughFrames;
}





void ExtrinsicCalibration::calcBoardCornerPositions(cv::Size boardSize, float squareSize, vector<cv::Point3f> &corners)
{
    corners.clear();
    for( int i = 0; i < boardSize.height; ++i )
        for( int j = 0; j < boardSize.width; ++j )
            corners.push_back(Point3f(j*squareSize, i*squareSize, 0));

}

void ExtrinsicCalibration::startCalibration()
{
    Mat Kdvs, Krs, R, F, E;
    Vec3d T;
    Mat Ddvs, Drs;
    vector< vector< Point3f > > object_points;
    cv::FileStorage dvsFs(dvsFileNameYAML,cv::FileStorage::READ);
    cv::FileStorage rsFs(rsFileNameYAML,cv::FileStorage::READ);

    dvsFs["K"] >> Kdvs;
    rsFs["K"] >> Krs;
    dvsFs["D"] >> Ddvs;
    rsFs["D"] >> Drs;
    int flag = 0;
    flag |= cv::CALIB_FIX_INTRINSIC;

    cout << "Read intrinsics" << endl;

    stereoCalibrate(object_points, imagePointsDvs, imagePointsRs, Kdvs, Ddvs, Krs, Drs, Kdvs.size(), R, T, E, F);

    isCalibrationCalculated = true;
}


bool ExtrinsicCalibration::setToSave = false;
void ExtrinsicCalibration::callBackFunc(int event, int x, int y, int flags, void* userdata)
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

void ExtrinsicCalibration::saveImage(cv::Mat mat)
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
