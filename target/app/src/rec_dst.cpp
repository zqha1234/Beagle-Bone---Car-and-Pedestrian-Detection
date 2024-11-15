#include "../include/main.h"
#include "../include/rec_dst.h"
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <string.h>
#include <vector>
#include <time.h>

using namespace std;

static pthread_t rec_dst_thread; 
static const char* host3 = "192.168.7.2";
static int port2 = 9999;
static double s_dst = 9999.0;
static const int buffSize = 1024;

// Delay in ms
void sleepForMs(long long delayInMs) {
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

static void* rec_dst_function(void* unused) {
    (void)unused;
    int sockfd3 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd3 < 0) {
        cerr << "Failed to create socket!!!" << std::endl;
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port2);
    server_addr.sin_addr.s_addr = inet_addr(host3);
    int bind_ret = bind(sockfd3, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret < 0) {
        cerr << "Bind failed!!!" << std::endl;
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    while (isRun()) {
        vector<char> buf(buffSize);
        int recv_len = recvfrom(sockfd3, buf.data(), buffSize, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len < 0) {
            continue;
        } 
        string msg(buf.begin(), buf.begin() + recv_len);
        s_dst = stod(msg);
        sleepForMs(10);
        //cout<<msg<<endl;
    }
    close(sockfd3);
    return NULL;
}

// Get shortest distance
double get_shortest_dst(void) {
    return s_dst;
}

// Createa a  camera thread
void Rec_dst_init(void) {
    int result_thread;
    result_thread = pthread_create(&rec_dst_thread, NULL, rec_dst_function, NULL);
    //check if the thread is created sucessfully
    if (result_thread != 0){
        // if thread cration fails, exit the program
        perror("Thread Creation Error!\n");
        exit(1);
    }
}

// Shut down the thread
void Rec_dst_cleanup(void) {
    pthread_join(rec_dst_thread, NULL);
}