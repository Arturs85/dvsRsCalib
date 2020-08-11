//! @file
//! @brief	main executable

//#include "base/loadestimatingworker.hpp"
#include "calibworker.hpp"
#include "workersconfig.hpp"
//#include "base/remotecontroller.hpp"

#include <unistd.h>
#include <inttypes.h>

#include <iostream>
#include <string>


using std::cout;
using std::cerr;
using std::endl;
using std::string;


//! WorkersConfig (externally accessible):
WorkersConfig *p_conf = nullptr;
#define getConfigOrDefault(key,defv) \
    p_conf->getConfigOrDefault("dvs-worker", key, defv)


//using DESTS = RemoteController::DESTS;
//using TYPES = RemoteController::TYPES;


//! Workers are defined here:
//static LoadEstimatingWorker *p_leworker = nullptr;
static CalibWorker *p_dvsworker = nullptr;

//! Remote controller:
//static RemoteController *p_ctrl = nullptr;


//static void processUDPReceiveWorkerRequest(const RemoteController::Type_t &type, const std::string &data);
//static void processLoadEstimatingWorkerRequest(const RemoteController::Type_t &type, const std::string &data);



int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    WorkersConfig config = WorkersConfig();
    p_conf = &config;
    if (!config.loadFromFile("config.ini"))
    {
        std::cerr << "Cannot load config! Make sure that proper config.ini file exists!" << std::endl;
   //     return 1;
    }

    // setting main thread to Core #0
    {
        cpu_set_t m;
        CPU_ZERO(&m);
        CPU_SET(0, &m);
        int ret = pthread_setaffinity_np(pthread_self(), sizeof(m), &m);
        if (ret != 0)
            cerr << "Error setting main thread affinity!" << endl;
    }



    // Printing intro
    cout << endl;
    cout << "   _          " << endl;
    cout << "  |    /_|  | " << endl;
    cout << "  |_  /  |  |_" << endl << endl;
    cout << "calib worker test program\n" << endl << endl;



    // Generic UDP data logging
    p_dvsworker = new CalibWorker();
    //p_dvsworker->setCPUMask(0x02);
  p_dvsworker->start();

    // Waiting for user commands
    while(true)
    {
        string userCommand;
        std::cout <<"Enter command!";
        std::cin>>userCommand;

        if(!userCommand.compare("exit"))
        {
            break;
        }
        else if(!userCommand.compare("start"))
        {
            p_dvsworker->sendRequest(WORKER_REQUEST::RESERVE_FILE, 1000000);

            p_dvsworker->sendRequest(WORKER_REQUEST::START);

        }
        else if(!userCommand.compare("size"))
        {
            p_dvsworker->sendRequest(WORKER_REQUEST::LENGTH_OF_DUMP);

        }
        else if(!userCommand.compare("convert"))
        {
            std::cout<<"converting";

        std::cout<<"done converting";
        }
        else if(!userCommand.compare("0"))// zero time
        {
            p_dvsworker->sendRequest(WORKER_REQUEST::GET_CPU_LOAD);//temp


        }
        else
        {
            cerr << "Cannot understand request\n";
        }
        userCommand="";
    }
    p_dvsworker->sendRequest(WORKER_REQUEST::EXIT);
    p_dvsworker->stop();

    delete p_dvsworker;


    cout << "Bye." << endl;
    return 0;
}



