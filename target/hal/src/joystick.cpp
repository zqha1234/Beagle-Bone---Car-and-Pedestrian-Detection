#include "../include/hal/joystick.h"
#include "../../app/include/main.h"
#include "../../app/include/rec_dst.h"
#include "../include/hal/fourteen_seg_display.h"
#include "../include/hal/accelerometer.h"
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

using namespace std;

static char jy_up[] = "config-pin p8.14 gpio";
static char jy_dn[] = "config-pin p8.16 gpio";
static char jy_md[] = "config-pin p8.17 gpio";
static char jy_up_dir[] = "cd /sys/class/gpio/gpio26; echo in > direction";
static char jy_dn_dir[] = "cd /sys/class/gpio/gpio46; echo in > direction";
static char jy_md_dir[] = "cd /sys/class/gpio/gpio27; echo in > direction";
static char JSUP[] = "/sys/class/gpio/gpio26/value";
static char JSDN[] = "/sys/class/gpio/gpio46/value";
static char JSPB[] = "/sys/class/gpio/gpio27/value";

static pthread_t js_thread; 
static const char* host2 = "192.168.7.1";
static int port2 = 8899;

// Joytick Initialize (config pins)
static void joystickInit(void){
    runCommand(jy_up);
    runCommand(jy_dn);
    runCommand(jy_md);
    runCommand(jy_up_dir);
    runCommand(jy_dn_dir);
    runCommand(jy_md_dir);
}

// Read gpio value
static int gpioValue (char* gpio_add){
    FILE *pFile = fopen(gpio_add, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", gpio_add);
        exit(1);
    }
    // Read string (line)
    const int MAX_LENGTH = 1024;
    char value[MAX_LENGTH];
    fgets(value, MAX_LENGTH, pFile);
    // Close
    fclose(pFile);
    return atoi(value);
}

// Read the joystick direction info by reading gpio value
static string readStickDirec (void){
    if (gpioValue(JSUP) == 0) {
        return "UP";
    }else if (gpioValue(JSDN) == 0) {
        return "DOWN";
    }else if (gpioValue(JSPB) == 0) {
        return "MIDDLE";
    } else if (get_save_pic()) { 
        set_save_pic();
        return "SAVE";
    } else {
        return "NODIR";
    }
}

static void* js_function(void* unused) {
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
    int connect_ret = connect(sockfd2, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_ret < 0) {
        cerr << "Connect failed!!!" << std::endl;
        exit(1);
    }
    joystickInit();
    while (isRun()) {
        string buf = readStickDirec();
        send(sockfd2, buf.c_str(), buf.size(), 0);
        if (buf == "MIDDLE") {
            Program_terminate();
        }
        sleepForMs(500);
    }
    close(sockfd2);
    return NULL;
}

// Createa a  camera thread
void Joystick_init(void) {
    int result_thread;
    result_thread = pthread_create(&js_thread, NULL, js_function, NULL);
    //check if the thread is created sucessfully
    if (result_thread != 0){
        // if thread cration fails, exit the program
        perror("Thread Creation Error!\n");
        exit(1);
    }
}

// Shut down the thread, and close the camera
void Joystick_cleanup(void) {
    pthread_join(js_thread, NULL);
}
