
#include "calibworker.hpp"
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <cstring>
#include <cstdlib>
#include <cstdio>



#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "socketserver.h"
#include "intrinsiccalibration.hpp"

//! Log and Log Forward:
#ifndef FROM_MAIN
#define INIT_LOG_FORWARD_FOR_ALL()
#define logOutput std::cout
#define LF std::endl
#else
#include "logforward.hpp"
#include "logger.hpp"
#include "remotecontroller.hpp"
extern Logger *p_log;
#define logOutput (p_log->printableWithForward(RemoteController::printableDEST(RemoteController::DESTS::DVS_WORKER), log_forward))
#define LF LoggerHelper::endl()
#define INIT_LOG_FORWARD_FOR_ALL() \
    INIT_LOG_FORWARD(); \
    RawToAedatConverter::SET_LOG_FORWARDER(log_forward); \
    DVSFileSaver::SET_LOG_FORWARDER(log_forward); \
    ConfigFileReader::SET_LOG_FORWARDER(log_forward);
#endif


//! WorkersConfig:
#include "workersconfig.hpp"
extern WorkersConfig *p_conf;
#define getConfigOrDefault(key,defv) \
    p_conf->getConfigOrDefault("dvs-worker", key, defv)


using std::string;
using std::stringstream;
using std::cerr;
using std::endl;


//! Default fixed settings:
constexpr const char *DEFAULT__PERFORM_RAW_TO_AEDAT_CONVERSION      = "y";


constexpr unsigned int	DEFAULT__MAX_WAIT__MS			= 1000;
constexpr size_t		DEFAULT__MAX_RECV_BYTES			= 1024;
constexpr unsigned int	DEFAULT__SLEEP_INTERVAL__MS		= 1000;
constexpr size_t		DEFAULT__FILE_SIZE				= 32*1024*1024;
typedef uint32_t		ALIGNED_DATA_TYPE;



CalibWorker::CalibWorker()
{
    MAX_WAIT__MS = DEFAULT__MAX_WAIT__MS;
    MAX_RECV_BYTES = DEFAULT__MAX_RECV_BYTES;
    SLEEP_INTERVAL__MS = DEFAULT__SLEEP_INTERVAL__MS;
    FILE_SIZE = DEFAULT__FILE_SIZE;

    hDumpFile = 0;
    allocated_size = 0;
    written_bytes = 0;

    INIT_LOG_FORWARD_FOR_ALL();

    state = STATES::STARTUP;
}


CalibWorker::p_CallableThreadFunction_t CalibWorker::getCallableThreadFunction(void)
{
    return threadFunctionWrapper<CalibWorker>;
}


void CalibWorker::setSupportedRequestsAndResponses(RequestsSet_t& requests, ResponsesSet_t& responses) const
{
    requests.clear();
    requests.insert(WORKER_REQUEST::RESERVE_FILE);
    requests.insert(WORKER_REQUEST::WAIT_FOR_CLOCK_SYNC);
    requests.insert(WORKER_REQUEST::START);
    requests.insert(WORKER_REQUEST::LENGTH_OF_DUMP);

    responses.clear();
    responses.insert(WORKER_RESPONSE::FILE_RESERVE_OK);
    responses.insert(WORKER_RESPONSE::FILE_RESERVE_ERROR);
    responses.insert(WORKER_RESPONSE::STARTED);
    responses.insert(WORKER_RESPONSE::NOT_STARTED);
    responses.insert(WORKER_RESPONSE::FILE_WRITE_ERROR);
    responses.insert(WORKER_RESPONSE::LENGTH_OF_DUMP);
    responses.insert(WORKER_RESPONSE::DVS_IS_SLAVE);
    responses.insert(WORKER_RESPONSE::DVS_IS_SYNC);
    responses.insert(WORKER_RESPONSE::LOG_FORWARD);
}


void CalibWorker::init(void)
{
    // doing nothing
}


void CalibWorker::threadFunction(void)
{
    while(true)
    {
        // performing main work here:
        // ----------------------------------------------------------------------------------------------------------
        switch(state)
        {
        case STATES::STARTUP:
            onSTARTUP();
            break;
case STATES::IDLE:
            onIDLE();
            
            break;
        case STATES::INTRINSIC_ACCUMULATION:
                    onIDLE();
                   onINTRINSIC_ACCUMULATION();
                    //int ret =dvsIntrinsicCalibrator.addImage();
                    //if(ret) dvs done
                    //rs..
                    //if (dvsDone&&rs done) state ir calcIntrinsic
                    break;


            // all other states are interpreted as error
        case STATES::ERROR:
        default:

            onERROR();
        }
        // ----------------------------------------------------------------------------------------------------------

        // polling for requests, including stop request
        if (requests.areMessagesPresent())
        {
            IWorkerMessageQueue<WorkerRequest_t>::pMessage_t pRequest = requests.getMessage();
            bool must_break = false;

            switch(pRequest->type)
            {
            case WORKER_REQUEST::EXIT:
            {
                // stop requested
                must_break = true;
                break;
            }

            case WORKER_REQUEST::GET_CPU_LOAD:
                //temporary for testing timestamp reset
                logOutput<<"issuing timestamp reset command" << LF;

                break;
            case WORKER_REQUEST::RESERVE_FILE:
            {
                logOutput << "Requested RESERVE_FILE with " << pRequest->data << LF;

                break;
            }


            case WORKER_REQUEST::START:
            {
                logOutput << "Requested START." << LF;
state = STATES::INTRINSIC_ACCUMULATION;

//sendImage();
                sendResponse(WORKER_RESPONSE::STARTED);
                break;
            }



            default:
            {
                // unrecognized request - ignoring it
            }
            }

            requests.destroyMessage(pRequest);

            // we process stop request (exit) after destroying the request object
            if (must_break)
                break;
        }
    }

    // exiting//




}



void CalibWorker::onSTARTUP(void)
{
socketserver = new SocketServer();
dvsIntrinsicCalibration = new IntrinsicCalibration();

state = STATES::IDLE;


    logOutput<<"calib onStart finished \n";
}





void CalibWorker::onERROR(void)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

void CalibWorker::onIDLE()
{
    //read images from workers and send trough the server socket
    if(currentDvsImage!=0)delete currentDvsImage;
    currentDvsImage = SharedImage::dvsSharedImage.blockingCloneImage();

    sendImage(currentDvsImage->image);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

}

void CalibWorker::onINTRINSIC_ACCUMULATION()
{
 dvsIntrinsicCalibration->addImage(currentDvsImage->image);
}

void CalibWorker::sendImage(cv::Mat dvsImage)
{
    //test image sending
//  std::string dvsFileName = "/home/vnpc/Pictures/mill.png";
  std::string dvsFileName = "/home/arturs/Pictures/partial.png";

//  cv::Mat dvsImage=cv::imread(dvsFileName);

  auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);

      std::ostringstream oss;
      oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
      auto str = oss.str();


  cv::putText(dvsImage, "Calib Worker "+str, cv::Point(30,30),
      FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200,200,20), 1, CV_8U);
  cv::putText(dvsImage, "Dvs images checked/valid "+std::to_string(dvsIntrinsicCalibration->totalFramesChecked)+"/"+std::to_string(dvsIntrinsicCalibration->capturedGoodFrames), cv::Point(30,60),
      FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200,200,20), 1, CV_8U);
  cv::putText(dvsImage, "Dvs images age "+std::to_string(SharedImage::getSystemTimeSec()-currentDvsImage->timeModifiedSec), cv::Point(30,90),
      FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200,200,20), 1, CV_8U);

  std::vector<uchar> buff;//buffer for coding
     std::vector<int> param(2);
     param[0] = cv::IMWRITE_JPEG_QUALITY;
     param[1] = 80;//default(95) 0-100
     cv::imencode(".jpg", dvsImage, buff, param);

  SocketServer::sendImage(buff.data(),buff.size());




}




