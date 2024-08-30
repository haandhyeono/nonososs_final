/******************************************************************************
 *
 * COPYRIGHT 2017 ASAHI KASEI MICRODEVICES CORPORATION ("AKM")
 * All Rights Reserved.
 *
 * This software is licensed to you under the Apache License, Version 2.0
 * (http://www.apache.org/licenses/LICENSE-2.0) except for using, copying,
 * modifying, merging, publishing and/or distributing in combination with
 * AKM's Proprietary Software defined below. 
 *
 * "Proprietary Software" means the software and its related documentations
 * which AKM will provide only to those who have entered into the commercial
 * license agreement with AKM separately. If you wish to use, copy, modify,
 * merge, publish and/or distribute this software in combination with AKM's
 * Proprietary Software, you need to request AKM to enter into such agreement
 * and grant commercial license to you.
 *
 ******************************************************************************/
#include "AKM_Config.h"
#include "akh_i2c.h"
 
 /* For error handling */
#define INVALID_SLAVE_ADDR  0x00

/*** Device Slave addresses. **************************************************/
/* Slave address is defined as 8-bit address. */
/* It means that R/W bit 0 is added at LSB. */

/* AKM 3D Mag devices. */
#define MAGNETOMETER_SLAVE_ADDR          0x18
#define MAGNETOMETER_SLAVE_ADDR_AK09919  0x1C   //AK09919

/* AKM IR devices. */
#define INFRARED_SLAVE_ADDR  0xC8

/* Accelerometer */
#define ADXL34X_SLAVE_ADDR  0xA6
#define BMI160_SLAVE_ADDR   0xD0  // or 0xD2

/* Gyroscope */
#define L3G4200D_SLAVE_ADDR 0xD0
#define L3GD20_SLAVE_ADDR   0xD4


/* I2C instance for communication of sensors */
#ifdef ACSP_DEFINE
// For ACSP Board
I2C i2c(PB_7, PB_6);
#else
// For Arduino Sensor Board
I2C i2c(I2C_SDA, I2C_SCL);
#endif

static uint8_t type_to_slave(const AKM_SENSOR_TYPE stype)
{
    switch (stype) {
    case AKM_ST_MAG:
        if (CONFIG_SLOT1 == AKM_MAGNETOMETER_AK09919) {
            return MAGNETOMETER_SLAVE_ADDR_AK09919;
        } else {
            return MAGNETOMETER_SLAVE_ADDR;
        }

    case AKM_ST_ACC:
        if (CONFIG_SLOT2 == AKM_ACCELEROMETER_ADXL345) {
            return ADXL34X_SLAVE_ADDR;
        } else if (CONFIG_SLOT2 == AKM_ACCELEROMETER_ADXL346) {
            return ADXL34X_SLAVE_ADDR;
        } else if (CONFIG_SLOT2 == AKM_ACCELEROMETER_BMI160) {
            return BMI160_SLAVE_ADDR;
        } else {
            return INVALID_SLAVE_ADDR;
        }

    case AKM_ST_GYR:
        if (CONFIG_SLOT3 == AKM_GYROSCOPE_L3G4200D) {
            return L3G4200D_SLAVE_ADDR;
        } else if (CONFIG_SLOT3 == AKM_GYROSCOPE_L3GD20) {
            return L3GD20_SLAVE_ADDR;
        } else if (CONFIG_SLOT3 == AKM_GYROSCOPE_BMI160) {
            return BMI160_SLAVE_ADDR;
        } else {
            return INVALID_SLAVE_ADDR;
        }

    default:
        return INVALID_SLAVE_ADDR;
    }
}

int16_t AKH_I2C_Init(void)
{
    return AKM_SUCCESS;
}


int16_t AKH_I2C_TxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         address,
    const uint8_t         *data,
    const uint16_t        numberOfBytesToWrite)
{
    int  slave = type_to_slave(stype);
    int  err;
    char write_addr[1];

    if (slave == INVALID_SLAVE_ADDR) {
        return AKM_ERROR;
    }

    /* set register address once, then transfer data to the device. */
    write_addr[0] = address;
    err = i2c.write(slave, write_addr, 1, true);
    if (err != 0) {
        return AKM_ERR_IO;
    }
    err = i2c.write(slave, (char *)data, numberOfBytesToWrite, false);
    if (err != 0) {
        return AKM_ERR_IO;
    }

    return AKM_SUCCESS;
}

int16_t AKH_I2C_RxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         address,
    uint8_t               *data,
    const uint16_t        numberOfBytesToRead)
{
    int  slave = type_to_slave(stype);
    int  err;
    char write_addr[1];

    if (slave == INVALID_SLAVE_ADDR) {
        return AKM_ERROR;
    }

    /* set register address once, then transfer data to the device. */
    write_addr[0] = address;
    err = i2c.write(slave, write_addr, 1, true);
    if (err != 0) {
        return AKM_ERR_IO;
    }
    err = i2c.read(slave, (char *)data, numberOfBytesToRead, false);
    if (err != 0) {
        return AKM_ERR_IO;
    }

    return AKM_SUCCESS;
}