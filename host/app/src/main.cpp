#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "../include/main.h"
#include "../include/distance.h"
#include "../include/receive.h"
#include "../include/listen_joystick.h"
#include "../include/send_dst.h"

using namespace std;
using namespace cv;


static bool flag = true;

void Program_terminate(void) {
    flag = false;
}

bool isRun(void) {
    return flag;
}

static void wait_for_shutdown(void){
    Receive_cleanup();
    Listen_js_cleanup();
    Send_dst_cleanup();
}

int main(void) {
    Receive_init();
    Listen_js_init();
    Send_dst_init();
    wait_for_shutdown();
    return 0;
}
    
    
    
    
    
