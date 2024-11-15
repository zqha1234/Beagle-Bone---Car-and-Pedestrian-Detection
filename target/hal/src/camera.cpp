#include "../include/hal/camera.h"
#include "../include/hal/accelerometer.h"
#include "../../app/include/main.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

using namespace std;
using namespace cv;

static pthread_t send_thread; 
static const char* host1 = "192.168.7.1";
static int port1 = 8888;

static void* send_function(void* unused) {
    (void)unused;
    int sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd1 < 0) {
        cerr << "Failed to create socket!!!" << std::endl;
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port1);
    server_addr.sin_addr.s_addr = inet_addr(host1);
    int connect_ret = connect(sockfd1, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_ret < 0) {
        cerr << "Connect failed!!!" << std::endl;
        exit(1);
    }
    cv::VideoCapture capture(0);
    capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    capture.set(cv::CAP_PROP_FRAME_WIDTH, 480);
    if (!capture.isOpened()) {
        cerr << "Failed to open camera" << std::endl;
        exit(1);
    }
    cv::Mat frame;
    while (isRun()) {
        if (capture.read(frame)) {
            vector<uchar> buf;
            cv::imencode(".jpg", frame, buf, std::vector<int>{cv::IMWRITE_JPEG_QUALITY, 25});
            send(sockfd1, buf.data(), buf.size(), 0);
        }
    }
    // release camera resource
    capture.release();
    close(sockfd1);
    return NULL;
}


// Createa a  camera thread
void Camera_init(void) {
    int result_thread;
    result_thread = pthread_create(&send_thread, NULL, send_function, NULL);
    //check if the thread is created sucessfully
    if (result_thread != 0){
        // if thread cration fails, exit the program
        perror("Thread Creation Error!\n");
        exit(1);
    }
}

// Shut down the thread, and close the camera
void Camera_cleanup(void) {
    pthread_join(send_thread, NULL);
}


