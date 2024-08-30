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
#include "AKM_Common.h"




int16_t AKH_I2C_Init(void);

int16_t AKH_I2C_TxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         address,
    const uint8_t         *data,
    const uint16_t        numberOfBytesToWrite
);

int16_t AKH_I2C_RxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         address,
    uint8_t               *data,
    const uint16_t        numberOfBytesToRead
);