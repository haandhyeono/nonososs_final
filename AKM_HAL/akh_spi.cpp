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
#include "akh_spi.h"

#include "AKH_APIs.h"

#define AKS_SPI_SPEED 1000000 // 1[MHz]
#define AKS_SPI_BITS  8
#define AKS_SPI_MODE  3

/* SPI instance for communication of sensors */
#ifdef ACSP_DEFINE
// For ACSP Board
SPI        spi(SPI_MOSI, SPI_MISO, SPI_SCK);
/* Device chip select pin. */
DigitalOut cs_mag(D9);
DigitalOut cs_acc(PC_6);
DigitalOut cs_gyr(PC_6);
#else
// For Arduino Sensor Board
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK); 
/* Device chip select pin. */
DigitalOut cs_mag(SPI_CS);
DigitalOut cs_acc(D9);
DigitalOut cs_gyr(D8);
#endif

static DigitalOut *type_to_cspin(const AKM_SENSOR_TYPE stype)
{
    switch (stype) {
    case AKM_ST_MAG:
        return &cs_mag;

    case AKM_ST_ACC:
        return &cs_acc;

    case AKM_ST_GYR:
        if (CONFIG_SLOT3 == AKM_GYROSCOPE_BMI160) {
            return &cs_acc;
        } else {
            return &cs_gyr;
        }

    default:
        return NULL;
    }
}

int16_t AKH_SPI_Init(void)
{
    /* set spi format */
    spi.format(AKS_SPI_BITS, AKS_SPI_MODE);
    /* set to 1Mhz */
    spi.frequency(AKS_SPI_SPEED);
    /* set all CS pin to high */
    cs_mag = 1;
    cs_acc = 1;

    return AKM_SUCCESS;
}


int16_t AKH_SPI_TxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         address,
    const uint8_t         *data,
    const uint16_t        numberOfBytesToWrite)
{
    DigitalOut *cs = type_to_cspin(stype);
    int i;
    
    cs->write(0);
    /* set RW bit to write(=0) */
    spi.write(address & 0x7F);
    /* write subsequent data */
    for(i = 0 ; i < numberOfBytesToWrite ; i++ ) {
        spi.write(data[i]);
    }
    cs->write(1);

    return AKM_SUCCESS;
}

int16_t AKH_SPI_RxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         address,
    uint8_t               *data,
    const uint16_t        numberOfBytesToRead)
{
    DigitalOut *cs = type_to_cspin(stype);
    int i;

    cs->write(0);
    /* set RW bit to read(=1) */
    spi.write(address | 0x80);
    /* read subsequent data */
    for(i = 0 ; i < numberOfBytesToRead ; i++ ) {
        data[i] = spi.write(0x00);
    }
    cs->write(1);

    return AKM_SUCCESS;
}
