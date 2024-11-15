#include "../include/hal/fourteen_seg_display.h"
#include "../../app/include/main.h"
#include "../../app/include/rec_dst.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <time.h>
#include <string>

using namespace std;

static char gpio_export[] = GPIO_EXPORT;
static char gpio_unexport[] = "/sys/class/gpio/unexport";
static char gpio61_dir[] = GPIO61_DIR;
static char gpio44_dir[] = GPIO44_DIR;
static char gpio61_val[] = GPIO61_VAL;
static char gpio44_val[] = GPIO44_VAL;
// static char i2c_b0[] = I2CDRV_LINUX_BUS0;
static char i2c_b1[] = I2CDRV_LINUX_BUS1;
// static char i2c_b2[] = I2CDRV_LINUX_BUS2;
static char pin_61[] = "61";
static char pin_44[] = "44";
static char out_[] = "out";
static char one[] = "1";
static char zero[] = "0";


static int i2cFileDesc;
static pthread_t background_thread; 
static double display_value = 0;
//static char left_display[10] = {0xD0, 0xC0, 0x98, 0xD8, 0x0A, 0x58, 0x58, 0x02, 0xD8, 0xD8}; 
static char left_display1[10] = {0xF0, 0xE0, 0xB8, 0xF8, 0x2A, 0x78, 0x78, 0x22, 0xF8, 0xF8}; 
static char right_display[10] = {0xE1, 0x00, 0x83, 0x03, 0x2A, 0x23, 0xA3, 0x05, 0xA3, 0x23}; 
static int left_value;
static int right_value;

// Run a Linux command
void runCommand(const char* command) {
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it 
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL) {
            break;
            // printf("--> %s", buffer); // Uncomment for debugging
        }
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

// Write the file 
void writeFile (char* fileName, char* value) {
    FILE *pFile = fopen(fileName, "w");
    if (pFile == NULL) {
        printf("ERROR: Unable to open %s.\n", fileName);
        exit(1);
    }

    int charWritten = fprintf(pFile, value);
    if (charWritten <= 0) {
        printf("ERROR: Cannot write %s.\n", fileName);
        exit(1);
    }

    fclose(pFile);
}

// Initialize the I2C device
static int initI2cBus(char* bus, int address) { 
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

// Write an I2C device's register
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value) {
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

// Initianize the I2c setup
void initI2c(void) {
    // (bbg)$ config-pin P9_18 i2c
    // (bbg)$ config-pin P9_17 i2c
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c"); 
    writeFile(gpio_unexport, pin_61);
    writeFile(gpio_unexport, pin_44);
    writeFile(gpio_export, pin_61);
    writeFile(gpio_export, pin_44);
    writeFile(gpio61_dir, out_);
    writeFile(gpio44_dir, out_);
    writeFile(gpio61_val, one);
    writeFile(gpio44_val, one);
    i2cFileDesc = initI2cBus(i2c_b1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
    writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
}

// Display the value on fourteen segmentation display
static void *display_function(void* unused) {
    (void)unused;
    while (isRun()) {
        display_value = get_shortest_dst();
        // if (display_value > 999) {
        //     int value = (int) (display_value / 100);
        //     left_value = value / 10;
        //     right_value = value % 10;
        //     writeFile(gpio61_val, zero);
        //     writeFile(gpio44_val, zero);
        //     writeI2cReg(i2cFileDesc, REG_OUTA, left_display[left_value]);
        //     writeI2cReg(i2cFileDesc, REG_OUTB, right_display[left_value]);
        //     writeFile(gpio61_val, one);
        //     sleepForMs(4); // sleep 
        //     writeFile(gpio61_val, zero);
        //     writeFile(gpio44_val, zero);
        //     writeI2cReg(i2cFileDesc, REG_OUTA, left_display[right_value]);
        //     writeI2cReg(i2cFileDesc, REG_OUTB, right_display[right_value]);
        //     writeFile(gpio44_val, one);
        //     sleepForMs(4); // sleep 
        // } else if (display_value > 99) {
            int value = (int) (display_value / 10);
            left_value = value / 10;
            right_value = value % 10;
            writeFile(gpio61_val, zero);
            writeFile(gpio44_val, zero);
            writeI2cReg(i2cFileDesc, REG_OUTA, left_display1[left_value]);
            writeI2cReg(i2cFileDesc, REG_OUTB, right_display[left_value]);
            writeFile(gpio61_val, one);
            sleepForMs(5); // sleep 
            writeFile(gpio61_val, zero);
            writeFile(gpio44_val, zero);
            writeI2cReg(i2cFileDesc, REG_OUTA, left_display1[right_value]);
            writeI2cReg(i2cFileDesc, REG_OUTB, right_display[right_value]);
            writeFile(gpio44_val, one);
            sleepForMs(5); // sleep 
        // } else {
        //     int value = (int) (display_value / 10);
        //     left_value = 0;
        //     right_value = value;
        //     writeI2cReg(i2cFileDesc, REG_OUTA, left_display1[left_value]);
        //     writeI2cReg(i2cFileDesc, REG_OUTB, right_display[left_value]);
        //     writeFile(gpio61_val, one);
        //     sleepForMs(4); // sleep 
        //     writeFile(gpio61_val, zero);
        //     writeFile(gpio44_val, zero);
        //     writeI2cReg(i2cFileDesc, REG_OUTA, left_display1[right_value]);
        //     writeI2cReg(i2cFileDesc, REG_OUTB, right_display[right_value]);
        //     writeFile(gpio44_val, one);
        //     sleepForMs(4); // sleep 
        // }
        sleepForMs(10);
    }
    return NULL;
}

// Createa a background thread
void Display_init(void) {
    initI2c();
    int result_thread;
    result_thread = pthread_create(&background_thread, NULL, display_function, NULL);
    //check if the thread is created sucessfully
    if (result_thread != 0){
        // if thread cration fails, exit the program
        perror("Thread Creation Error!\n");
        exit(1);
    }
}


// Shut down thread, and close i2cFileDesc
void Display_cleanup(void) {
    pthread_join(background_thread, NULL);
    writeFile(gpio61_val, zero);
    writeFile(gpio44_val, zero);
    close(i2cFileDesc);
}
