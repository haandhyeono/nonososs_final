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
#ifndef INCLUDE_AK099XX_REGISTER_H
#define INCLUDE_AK099XX_REGISTER_H

#include "AKM_Config.h"

#define AK099XX_REG_WIA1                 0x00
#define AK099XX_REG_WIA2                 0x01
#define AK099XX_REG_INFO1                0x02
#define AK099XX_REG_INFO2                0x03
#define AK099XX_REG_ST1                  0x10

/* AK099XX_REG_MEASURE_DATA_HEAD refers to the head of the */
/* measurement register.     */
/* HXH: AK09917 and AK09919  */
/* HXL: other devices        */
#define AK099XX_REG_MEASURE_DATA_HEAD    0x11

#define AK099XX_REG_CNTL1                0x30
#define AK099XX_REG_CNTL2                0x31
#define AK099XX_REG_CNTL3                0x32

#define AK099XX_FUSE_ASAX                0x60
#define AK099XX_FUSE_ASAY                0x61
#define AK099XX_FUSE_ASAZ                0x62

#define AK099XX_BDATA_SIZE               9

#define AK099XX_MODE_SNG_MEASURE         0x01
#define AK099XX_MODE_CONT_MEASURE_MODE1  0x02
#define AK099XX_MODE_CONT_MEASURE_MODE2  0x04
#define AK099XX_MODE_CONT_MEASURE_MODE3  0x06
#define AK099XX_MODE_CONT_MEASURE_MODE4  0x08
#define AK099XX_MODE_CONT_MEASURE_MODE5  0x0A
#define AK099XX_MODE_CONT_MEASURE_MODE6  0x0C
#define AK099XX_MODE_SELF_TEST           0x10
#define AK099XX_MODE_FUSE_ACCESS         0x1F
#define AK099XX_MODE_POWER_DOWN          0x00

#define AK099XX_SOFT_RESET               0x01

#define AK09911_WIA_VAL                  0x548
#define AK09912_WIA_VAL                  0x448
#define AK09913_WIA_VAL                  0x848
#define AK09915_WIA_VAL                  0x1048
#define AK09916C_WIA_VAL                 0x948
#define AK09916D_WIA_VAL                 0xB48
#define AK09917D_WIA_VAL                 0xD48
#define AK09918_WIA_VAL                  0xC48
#define AK09919_WIA_VAL                  0xE48
#define AK09915D_INFO_VAL                0x02

#define AK099XX_NSF_VAL  0x40
#define AK099XX_ITS_VAL  0x60
#define AK099XX_SET_LOWNOISE(cntl2)  ((0x40) | (cntl2))
#define AK099XX_SET_FIFO(cntl2)      ((0x80) | (cntl2))
#endif /* INCLUDE_AK099XX_REGISTER_H */
