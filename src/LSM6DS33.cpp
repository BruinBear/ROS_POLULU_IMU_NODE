#include "LSM6DS33.h"

/*
Relevant Pololu products:
#2738  LSM6DS33 + LIS3MDL Carrier (v5)
*/

// LSM6DS33 addresses:
#define DS33_SA0_HIGH_ADDRESS              0x6a
#define DS33_SA0_LOW_ADDRESS               0x6b

#define DS33_WHO_ID    0x69

LSM6DS33::LSM6DS33(const char * i2cDeviceName) :
  i2c_gyro(i2cDeviceName), i2c_acc(i2cDeviceName)
{
    I2CBus i2c(i2cDeviceName);
    bool sa0;
    if (i2c.tryReadByte(DS33_SA0_HIGH_ADDRESS, WHO_AM_I) == DS33_WHO_ID)
    {
        // Detected LSM6DS33 with SA0 high.
	// printf("LSM6DS33 SA0 high\n");
        device = Device::LSM6DS33;
        sa0 = true;
    }
    else if (i2c.tryReadByte(DS33_SA0_LOW_ADDRESS, WHO_AM_I) == DS33_WHO_ID)
    {
        // device responds to address 0011110 with D ID; it's a D with SA0 low
        // printf("LSM6DS33 SA0 low\n");
	device = Device::LSM6DS33;
        sa0 = false;
    }
    else
    {
        throw std::runtime_error("Could not detect accelerometer.");
    }

    // Set the I2C addresses.
    
    uint8_t address = sa0 ? DS33_SA0_HIGH_ADDRESS : DS33_SA0_LOW_ADDRESS;
    i2c_acc.addressSet(address);
    i2c_gyro.addressSet(address);

    // Make sure we can actually read an accelerometer control register.
    // (This throws an exception if there is a problem.)
    readAccReg(CTRL1_XL);
}

uint8_t LSM6DS33::readGyroReg(uint8_t reg)
{
    return i2c_gyro.readByte(reg);
}

uint8_t LSM6DS33::readAccReg(uint8_t reg)
{
    return i2c_acc.readByte(reg);
}

void LSM6DS33::writeGyroReg(uint8_t reg, uint8_t value)
{
    i2c_gyro.writeByte(reg, value);
}

void LSM6DS33::writeAccReg(uint8_t reg, uint8_t value)
{
    i2c_acc.writeByte(reg, value);
}

// Turns on the LSM6DS33's accelerometer and gyroscope and places them in normal
// mode.
void LSM6DS33::enable(void)
{
    if (device == Device::LSM6DS33)
    {
	// Accelerometer
	// pg 46
	// AODR = 0011 52HZ, 8g
	writeAccReg(CTRL1_XL, 0b01001100);
	
	// Gyro
	// pg 48
	// full scale 2000dps
        writeGyroReg(CTRL2_G, 0b10001100);
    }
}

void LSM6DS33::readAcc(void)
{
    uint8_t block[6];
    i2c_acc.readBlock(OUTX_L_XL, sizeof(block), block);
    a[0] = (int16_t)(block[0] | block[1] << 8);
    a[1] = (int16_t)(block[2] | block[3] << 8);
    a[2] = (int16_t)(block[4] | block[5] << 8);
}

void LSM6DS33::readGyro(void)
{
    uint8_t block[6];

    if (device == Device::LSM6DS33)
    {
        i2c_gyro.readBlock(OUTX_L_G, sizeof(block), block);
        g[0] = (int16_t)(block[0] | block[1] << 8);
        g[1] = (int16_t)(block[2] | block[3] << 8);
        g[2] = (int16_t)(block[4] | block[5] << 8);
    }
}

void LSM6DS33::read(void)
{
    readAcc();
    readGyro();
}
