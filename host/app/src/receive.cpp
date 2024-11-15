#include "../include/main.h"
#include "../include/receive.h"
#include "../include/listen_joystick.h"
#include "../include/distance.h"
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

using namespace std;
using namespace cv;

static pthread_t receive_thread; 
static const char* host = "192.168.7.1";
static int port = 8888;
static const int buffSize = 65000;
static double short_dst = 9999;
static int count_obj = 0;
//static int sockfd1;
vector<unsigned char> buffer(buffSize);

// receive the pic from the beagle bone, process the pic and output
static void* receive_function(void* unused) {
    (void)unused;
    int sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd1 < 0) {
        cerr << "Failed to create socket!!!" << std::endl;
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);
    int bind_ret = bind(sockfd1, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret < 0) {
        cerr << "Bind failed!!!" << std::endl;
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    cv::Mat frame;
    cv::Mat frame1;
    cv::dnn::Net net;
    net = cv::dnn::readNet("../../opencv_host/MobileNetSSD_deploy.caffemodel", "../../opencv_host/MobileNetSSD_deploy.prototxt");
    if (net.empty()) {
        cerr << "Error: Could not load the pre-trained model!!!" << endl;
        exit(1);
    }
    vector<std::string> objName;
    objName.push_back("person");
    objName.push_back("car");
    while (isRun()) {
        int recv_len = recvfrom(sockfd1, buffer.data(), buffSize, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len < 0) {
            continue;
        }
        frame = cv::imdecode(cv::Mat(buffer), cv::IMREAD_COLOR);
        frame1 = cv::imdecode(cv::Mat(buffer), cv::IMREAD_COLOR);
        if (get_save_pic()) {
            // cout<<"true"<<endl;
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t(now);
            stringstream ss;
            ss << put_time(localtime(&now_c), "%Y-%m-%d_%H-%M-%S");
            string filename = ss.str() + ".jpg";
            cv::imwrite(filename, frame);
            set_save_pic();
        }
        int h = frame.size().height;
        int w = frame.size().width;
        cv::Mat blob;
        cv::dnn::blobFromImage(frame,blob,1/255.0,cv::Size(300,300));
        net.setInput(blob);
        cv::Mat detection = net.forward("detection_out");
        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
        //set confidence value
        float confidenceThreshold = 0.2;
        int index = 0;
        count_obj = 0;
        vector<double> dst_v;
        cout<<"-----------Real Time Info----------"<<endl;
        for (int i = 0; i < detectionMat.rows;i++) {
            float confidence = detectionMat.at<float>(i,2);
            int idx = static_cast<int>(detectionMat.at<float>(i, 1));
            if (idx == 15 || idx == 7) { // according to the coco data sheet, 15 is person, 7 is car
                if (confidence > confidenceThreshold) {
                    count_obj++;
                    //int idx = static_cast<int>(detectionMat.at<float>(i, 1));
                    //cout << "idx is " << idx << endl;
                    int left = static_cast<int>(detectionMat.at<float>(i, 3) * w);
                    int top = static_cast<int>(detectionMat.at<float>(i, 4) * h);
                    int right = static_cast<int>(detectionMat.at<float>(i, 5) * w);
                    int bottom = static_cast<int>(detectionMat.at<float>(i, 6) * h);
                    cv::rectangle(frame,Rect(left, top, right-left, bottom-top), Scalar(255, 0, 0), 2);
                    if (idx == 15) {
                        index = 0;
                    } else {
                        index = 1;
                    }
                    string label = objName[index];
                    double distance = Distance_Cal(label, bottom-top); //unit: [cm]
                    dst_v.push_back(distance);
                    sort(dst_v.begin(), dst_v.end());
                    short_dst = dst_v[0];
                    distance = distance / 100.0; // convert unity from cm to m
                    cout<<label<<": "<<distance<<"m"<<endl;
                    if (getDir()) {
                        std::ostringstream text_dis;
                        text_dis << "distance: " << std::fixed << std::setprecision(2) << distance << " m";
                        string text = text_dis.str();
                        cv::putText(frame, label,cv::Point(int(left),int(top-5)),
                        FONT_HERSHEY_SIMPLEX,0.5, Scalar(0, 255, 0));
                        cv::putText(frame, text, cv::Point(int(left),int(top+15)),
                        FONT_HERSHEY_SIMPLEX,0.5, Scalar(0, 255, 0));
                    }
                }
            }
        }
        if (getDir()) {
            cv::imshow("frames", frame);
        } else {
            cv::imshow("frames", frame1);
        }
        
        cv::waitKey(1);
    }
    cv::destroyAllWindows();
    close(sockfd1);
    return NULL;
}

// Get shorteset distance
double getShortDst(void) {
    if (count_obj > 0) {
        return short_dst;
    } else {
        return 0;
    }
    
}

// Createa a  camera thread
void Receive_init(void) {
    int result_thread;
    result_thread = pthread_create(&receive_thread, NULL, receive_function, NULL);
    //check if the thread is created sucessfully
    if (result_thread != 0){
        // if thread cration fails, exit the program
        perror("Thread Creation Error!\n");
        exit(1);
    }
}

// Shut down the thread
void Receive_cleanup(void) {
    pthread_join(receive_thread, NULL);
}