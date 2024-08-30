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
#ifndef INCLUDE_AK8963_REGISTER_H
#define INCLUDE_AK8963_REGISTER_H

#include "AKM_Config.h"

#define AK8963_REG_WIA                   0x00
#define AK8963_REG_INFO                  0x01
#define AK8963_REG_ST1                   0x02
#define AK8963_REG_HXL                   0x03
#define AK8963_REG_HXH                   0x04
#define AK8963_REG_HYL                   0x05
#define AK8963_REG_HYH                   0x06
#define AK8963_REG_HZL                   0x07
#define AK8963_REG_HZH                   0x08
#define AK8963_REG_ST2                   0x09
#define AK8963_REG_CNTL1                 0x0A
#define AK8963_REG_CNTL2                 0x0B
#define AK8963_REG_ASTC                  0x0C
#define AK8963_REG_TS1                   0x0D
#define AK8963_REG_TS2                   0x0E
#define AK8963_REG_I2CDIS                0x0F

#define AK8963_FUSE_ASAX                 0x10
#define AK8963_FUSE_ASAY                 0x11
#define AK8963_FUSE_ASAZ                 0x12

#define AK8963_BDATA_SIZE                8

#define AK8963_CNTL1_SNG_MEASURE         0x01
#define AK8963_CNTL1_CONT_MEASURE_MODE1  0x02
#define AK8963_CNTL1_CONT_MEASURE_MODE2  0x06
#define AK8963_CNTL1_TRIG_MEASURE        0x04
#define AK8963_CNTL1_SELF_TEST           0x08
#define AK8963_CNTL1_FUSE_ACCESS         0x0F
#define AK8963_CNTL1_POWER_DOWN          0x00

#define AK8963_CNTL2_SOFT_RESET          0x01

#ifdef AK8963_14BIT_MODE
#define AK8963_bit_mode(mode)  (mode)
#else
#define AK8963_bit_mode(mode)  ((0x10) | (mode))
#endif
#endif /* INCLUDE_AK8963_REGISTER_H */
