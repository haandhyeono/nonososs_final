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
#ifndef INCLUDE_AK0994X_REGISTER_H
#define INCLUDE_AK0994X_REGISTER_H

#include "AKM_Config.h"

#define AK0994X_REG_WIA1                    0x00
#define AK0994X_REG_WIA2                    0x01
#define AK0994X_REG_ST                      0x0F
#define AK0994X_REG_ST1                     0x10
#define AK0994X_REG_HXL                     0x11
#define AK0994X_REG_HXM                     0x12
#define AK0994X_REG_HXH                     0x13
#define AK0994X_REG_HYL                     0x14
#define AK0994X_REG_HYM                     0x15
#define AK0994X_REG_HYH                     0x16
#define AK0994X_REG_HZL                     0x17
#define AK0994X_REG_HZM                     0x18
#define AK0994X_REG_HZH                     0x19
#define AK0994X_REG_TMPS                    0x1A
#define AK0994X_REG_ST2                     0x1B
#define AK0994X_REG_SXL                     0x20
#define AK0994X_REG_SXH                     0x21
#define AK0994X_REG_SYL                     0x22
#define AK0994X_REG_SYH                     0x23
#define AK0994X_REG_SZL                     0x24
#define AK0994X_REG_SZH                     0x25
#define AK0994X_REG_CNTL1                   0x30
#define AK0994X_REG_CNTL2                   0x31
#define AK0994X_REG_CNTL3                   0x32
#define AK0994X_REG_CNTL4                   0x33

#define AK0994X_BDATA_SIZE                  12
#define AK0994X_SDATA_SIZE                  6

#define AK0994X_MODE_SNG_MEASURE            0x01
#define AK0994X_MODE_CONT_MEASURE_MODE1     0x02
#define AK0994X_MODE_CONT_MEASURE_MODE2     0x04
#define AK0994X_MODE_CONT_MEASURE_MODE3     0x06
#define AK0994X_MODE_CONT_MEASURE_MODE4     0x08
#define AK0994X_MODE_CONT_MEASURE_MODE5     0x0A
#define AK0994X_MODE_CONT_MEASURE_MODE6     0x0C
#define AK0994X_MODE_CONT_MEASURE_MODE7     0x0E
#define AK0994X_MODE_CONT_MEASURE_MODE8     0x0F
#define AK0994X_MODE_SELF_TEST              0x10
#define AK0994X_MODE_POWER_DOWN             0x00
#define AK0994X_MODE_LOW_POWER_DRIVE1       0x00
#define AK0994X_MODE_LOW_POWER_DRIVE2       0x20
#define AK0994X_MODE_LOW_NOISE_DRIVE1       0x40
#define AK0994X_MODE_LOW_NOISE_DRIVE2       0x60
#define AK0994X_MODE_ULTRA_LOW_POWER_DRIVE  0x80

#define AK0994X_SOFT_RESET                  0x01

#define AK09940_WIA_VAL                     0xA148
#define AK09940A_WIA_VAL                    0xA348

#endif /* INCLUDE_AK0994X_REGISTER_H */
