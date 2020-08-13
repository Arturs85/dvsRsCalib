#ifndef CALIBWORKER_H
#define CALIBWORKER_H


#include "iworkerthread.hpp"
#include <libcaercpp/devices/davis.hpp>
#include <libcaer/devices/device.h>
#include <atomic>
#include "sharedimage.hpp"
#include <netdb.h>


class SocketServer;
class IntrinsicCalibration;

class CalibWorker : public IWorkerThread
{
public:

    CalibWorker();

    virtual void init(void) override;
    virtual void threadFunction(void) override;
    virtual void setSupportedRequestsAndResponses(RequestsSet_t& requests, ResponsesSet_t& responses) const override;

    static const int FRAME_HEADER_LENGTH = 9*4;//bytes or (sizeof(struct caer_frame_event) - sizeof(uint16_t))
    static const int IMU6_DATA_LENGTH = 9*4;//bytes

    static  std::string containerStartLabel;// = "containr";//8 bytes
    static  std::string polarityPackageLabel;// = "polarity";//8 bytes
    static  std::string framesPackageLabel;// = "frames__";//8 bytes
    static  std::string imuPackageLabel;// = "imu_pkg_";//8 bytes
    static  std::string specialPackageLabel;// = "special_";//8 bytes

    std::string dvsPreviewFilename = "dvsPreview.png";
protected:

    virtual p_CallableThreadFunction_t getCallableThreadFunction(void) override;

private:
    SocketServer* socketserver;
    IntrinsicCalibration* dvsIntrinsicCalibration;
    SharedImage* currentDvsImage=0;
    SharedImage* currentRsImage=0;


    typedef enum class STATES : uint8_t
    {
        STARTUP,
        IDLE,
        INTRINSIC_ACCUMULATION,
        EXTRINSIC_ACCUMULATION,
        ERROR,

    } State_t;

    // process different states:
    void onSTARTUP(void);
    void onERROR(void);
    void onIDLE(void);
    void onINTRINSIC_ACCUMULATION(void);
    void onEXTRINSIC_ACCUMULATION(void);

    void sendImage(cv::Mat dvsImage);
    //void onERROR(void);

    //virtual void onStop(void) override;


    void saveFrameToFile(const libcaer::events::FrameEvent *frameEvent);// for preview

    unsigned int MAX_WAIT__MS;
    size_t MAX_RECV_BYTES;
    unsigned int SLEEP_INTERVAL__MS;
    const char* LISTEN_ADDRESS;
    const char* LISTEN_PORT;
    size_t FILE_SIZE;


    State_t state;



    // ----------------------------------------
    uint8_t* recv_buffer;
    std::string addr;
    std::string port;
    struct addrinfo hints;
    addrinfo *f_addrinfo;
    int f_socket;
    fd_set fds_rd;
    struct timeval timeout;

    std::string dump_filename;
    int hDumpFile;
    size_t allocated_size;
    size_t written_bytes;
    int xSize=0;
    int ySize=0;
};

#endif // CALIBWORKER_H
