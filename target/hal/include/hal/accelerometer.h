#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

// the accelerometer.c program receives the input from acceleration info (by reading the i2c registers), 

// define the i2c
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
// define i2c device address
#define I2C_DEVICE_ADDRESS_ 0x18
// I2C register for accelerometer
#define CTRL_REG1 0x20
#define CTRL_REG4 0x23
// I2C registers for x y z
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D
// I2C register for status
#define STATUS_REG 0x27

// create the thread
void Accelerometer_init(void);
// clean up the thread
void Accelerometer_cleanup(void);

// get save_pic value
bool get_save_pic(void);
// set save_pic to false;
void set_save_pic(void);

#endif