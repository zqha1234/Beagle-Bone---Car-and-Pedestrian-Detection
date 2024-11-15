#include "../include/send_dst.h"
#include "../include/main.h"
#include "../include/receive.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <time.h>

using namespace std;

static pthread_t send_dst_thread; 
static const char* host3 = "192.168.7.2";
static int port3 = 9999;

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

static void* send_dst_function(void* unused) {
    (void)unused;
    int sockfd3 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd3 < 0) {
        cerr << "Failed to create socket!!!" << std::endl;
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port3);
    server_addr.sin_addr.s_addr = inet_addr(host3);
    int connect_ret = connect(sockfd3, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_ret < 0) {
        cerr << "Connect failed!!!" << std::endl;
        exit(1);
    }
    while (isRun()) {
        string buf = to_string(getShortDst());
        send(sockfd3, buf.c_str(), buf.size(), 0);
        sleepForMs(5);
    }
    close(sockfd3);
    return NULL;
}

// Createa a  camera thread
void Send_dst_init(void) {
    int result_thread;
    result_thread = pthread_create(&send_dst_thread, NULL, send_dst_function, NULL);
    //check if the thread is created sucessfully
    if (result_thread != 0){
        // if thread cration fails, exit the program
        perror("Thread Creation Error!\n");
        exit(1);
    }
}

// Shut down the thread, and close the camera
void Send_dst_cleanup(void) {
    pthread_join(send_dst_thread, NULL);
}
