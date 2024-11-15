#include "../include/hal/buzzer_led.h"
#include "../include/hal/fourteen_seg_display.h"
#include "../include/hal/a2d.h"
#include "../../app/include/rec_dst.h"
#include "../../app/include/main.h"
#include "../../app/include/sharedMem-Linux.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>

static pthread_t buzzer_led_thread; 
static double s_dst = 0.0;

static char red_per[] = RED_PERIOD;
static char green_per[] = GREEN_PERIOD;
static char blue_per[] = BLUE_PERIOD;
static char red_enable[] = RED_ENABLE;
static char green_enable[] = GREEN_ENABLE;
static char blue_enble[] = BLUE_ENABLE;
static char red_cycle[] = RED_CYCLE;
static char green_cycle[] = GREEN_CYCLE;
static char blue_cycle[] = BLUE_CYCLE;
static char buzzer_per[] = BUZZER_PERIOD;
static char buzzer_enable[] = BUZZER_ENABLE;
static char buzzer_cycle[] = BUZZER_CYCLE;
static char one_million[] = "1000000";
static char half_million[] = "500000";
static char hund_thousand[] = "100000";
static char one[] = "1";
static char zero[] = "0";
static double reading = 0.0;
static double warning_dst = 99;
static double pre_warning_dst = 0;

// Initiate the led
static void led_init(void) {
    runCommand("config-pin P9_14 pwm");
    runCommand("config-pin P9_16 pwm"); 
    runCommand("config-pin P8_19 pwm"); 
    writeFile(red_per, hund_thousand);
    writeFile(green_per, hund_thousand);
    writeFile(blue_per, hund_thousand);
    writeFile(red_enable, one);
    writeFile(green_enable, one);
    writeFile(blue_enble, one);
    writeFile(red_cycle, zero);
    writeFile(green_cycle, hund_thousand);
    writeFile(blue_cycle, zero);
}

// Initiate buzzer
static void buzzer_init(void) {
    runCommand("sudo config-pin p9_22 pwm");
    writeFile(buzzer_enable, zero);
    writeFile(buzzer_cycle, zero);
    writeFile(buzzer_per, one_million);
    writeFile(buzzer_cycle, half_million);
}

static void calculateLEDDisplayOption(double s_dst, double warning_dst) {
    double ratio = s_dst / warning_dst;
    if(ratio >= 0.9) {
        globalSharedStruct->LEDDisplayOption = 1;
    } else if (ratio >= 0.8) {
        globalSharedStruct->LEDDisplayOption = 2;
    } else if (ratio >= 0.7) {
        globalSharedStruct->LEDDisplayOption = 3;
    } else if (ratio >= 0.6) {
        globalSharedStruct->LEDDisplayOption = 4;
    } else {
        globalSharedStruct->LEDDisplayOption = 5;
    }
}

// Set buzzer and led
static void *buzzer_led_function(void* unused) {
    (void)unused;
    led_init();
    buzzer_init();

    
    // will be between 2200-3900, but max range is 0.95-1.7V

    while (isRun()) {
        reading = getA2DReading();
        // double scaledVoltage = (voltage - 2200) / (3900 - 2200);
        // double distanceThreshold = scaledVoltage * 400.0 + 100;
        warning_dst = (1000.0 + reading) / 10;
        s_dst = get_shortest_dst(); // unit [cm]
        // printf("s_dst : %f, distanceThreshold: %f\n", s_dst, reading);
        if (abs(pre_warning_dst - warning_dst) > 0.1) {
            pre_warning_dst = warning_dst;
            printf("Warning distance is: %f m \n", (warning_dst/100.0));
        }

        if (s_dst > warning_dst || s_dst == 0) {        // buzzer off
            globalSharedStruct->LEDColorOption = GREEN;
            globalSharedStruct->LEDDisplayOption = 0;
            writeFile(red_cycle, zero);
            writeFile(green_cycle, hund_thousand);
            writeFile(buzzer_enable, zero);
        } else {
            globalSharedStruct->LEDColorOption = RED;
            calculateLEDDisplayOption(s_dst, warning_dst);
            writeFile(red_cycle, hund_thousand);       // buzzer on
            writeFile(green_cycle, zero);
            writeFile(buzzer_enable, one);
        }
        sleepForMs(100);
    }
    return NULL;
}

// Createa a background thread
void Buzzer_led_init(void) {
    int result_thread;
    result_thread = pthread_create(&buzzer_led_thread, NULL, buzzer_led_function, NULL);
    //check if the thread is created sucessfully
    if (result_thread != 0){
        // if thread cration fails, exit the program
        perror("Thread Creation Error!\n");
        exit(1);
    }
}

// Shut down thread, and close i2cFileDesc
void Buzzer_led_cleanup(void) {
    writeFile(buzzer_enable, zero);

    writeFile(red_cycle, zero);
    writeFile(green_cycle, zero);

    writeFile(red_enable, zero);
    writeFile(green_enable, zero);
    writeFile(blue_enble, zero);
    sleepForMs(100);

    pthread_join(buzzer_led_thread, NULL);
}