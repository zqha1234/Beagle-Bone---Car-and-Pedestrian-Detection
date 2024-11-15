
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <vector>
#include <stdbool.h>
#include <stdio.h>
#include "../include/main.h"
#include "../include/rec_dst.h"
#include "../include/sharedMem-Linux.h"
#include "../../hal/include/hal/camera.h"
#include "../../hal/include/hal/joystick.h"
#include "../../hal/include/hal/fourteen_seg_display.h"
#include "../../hal/include/hal/buzzer_led.h"
#include "../../hal/include/hal/accelerometer.h"

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
    Camera_cleanup();
    Joystick_cleanup();
    Rec_dst_cleanup();
    Display_cleanup();
    Accelerometer_cleanup();
    Buzzer_led_cleanup();
    sharedMemCleanUp();
}

int main(void) {
    sharedMemInit();
    Camera_init();
    Joystick_init();
    Rec_dst_init();
    Display_init();
    Accelerometer_init();
    Buzzer_led_init();
    wait_for_shutdown();
    return 0;
}
    
    
    
