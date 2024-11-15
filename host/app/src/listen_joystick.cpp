#include "../include/main.h"
#include "../include/listen_joystick.h"
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

string jy_dir = "UP";

static pthread_t listen_js_thread; 
static const char* host2 = "192.168.7.1";
static int port2 = 8899;

static bool save_pic = false;

static const int buffSize = 1024;

// get info whether save pic
bool get_save_pic(void) {
    return save_pic;
}

// set save_pic to false
void set_save_pic(void) {
    save_pic = false;
}

static void* lsiten_js_function(void* unused) {
    (void)unused;
    int sockfd2 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd2 < 0) {
        cerr << "Failed to create socket!!!" << std::endl;
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port2);
    server_addr.sin_addr.s_addr = inet_addr(host2);
    int bind_ret = bind(sockfd2, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret < 0) {
        cerr << "Bind failed!!!" << std::endl;
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    while (isRun()) {
        vector<char> buf(buffSize);
        int recv_len = recvfrom(sockfd2, buf.data(), buffSize, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len < 0) {
            continue;
        } 
        string msg(buf.begin(), buf.begin() + recv_len);
        if (msg == "MIDDLE") {
            Program_terminate();
        }
        if (msg == "SAVE") {
            save_pic = true;
            continue;
        }
        if (msg != "NODIR"){
            jy_dir = msg;
        }
    }
    close(sockfd2);
    return NULL;
}

// get joystick direction
int getDir(void) {
    if (jy_dir == "UP"){
        return 1;
    }
    return 0;
}

// Createa a  camera thread
void Listen_js_init(void) {
    int result_thread;
    result_thread = pthread_create(&listen_js_thread, NULL, lsiten_js_function, NULL);
    //check if the thread is created sucessfully
    if (result_thread != 0){
        // if thread cration fails, exit the program
        perror("Thread Creation Error!\n");
        exit(1);
    }
}

// Shut down the thread
void Listen_js_cleanup(void) {
    pthread_join(listen_js_thread, NULL);
}