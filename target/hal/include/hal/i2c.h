// controls the 14-seg display by using i2c and gpio

#ifndef BEATBOX_I2C_H
#define BEATBOX_I2C_H

void i2c_runCommand(const char* command);
int i2c_initI2cBus(const char* bus, int address);
void i2c_writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
unsigned char i2c_readI2cReg(int i2cFileDesc, unsigned char regAddr);



#endif //BEATBOX_I2C_H
