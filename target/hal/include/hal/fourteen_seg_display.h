#ifndef FOURTEEN_SEG_DISPLAY_H_
#define FOURTEEN_SEG_DISPLAY_H_

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define GPIO61_DIR "/sys/class/gpio/gpio61/direction"
#define GPIO44_DIR "/sys/class/gpio/gpio44/direction"

#define GPIO61_VAL "/sys/class/gpio/gpio61/value"
#define GPIO44_VAL "/sys/class/gpio/gpio44/value"

#define GPIO_EXPORT "/sys/class/gpio/export"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x02 // Zen Red uses: 0x02
#define REG_DIRB 0x03 // Zen Red uses: 0x03
#define REG_OUTA 0x00 // Zen Red uses: 0x00
#define REG_OUTB 0x01 // Zen Red uses: 0x01

void sleepForMs(long long delayInMs);
void initI2c(void);
void Display_init(void);
void Display_cleanup(void);
void writeFile (char* fileName, char* value);
void runCommand(const char* command);

#endif