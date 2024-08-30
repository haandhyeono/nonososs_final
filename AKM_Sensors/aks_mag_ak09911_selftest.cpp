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
#include "ak099xx_register.h"
#include "aks_common.h"
#include "aks_mag_ak099xx.h"
#include "AKH_APIs.h"

#define TLIMIT_NO_RST        0x101
#define TLIMIT_NO_RST_READ   0x102

#define TLIMIT_NO_RST_WIA1   0x103
#define TLIMIT_LO_RST_WIA1   0x48
#define TLIMIT_HI_RST_WIA1   0x48
#define TLIMIT_NO_RST_WIA2   0x104
#define TLIMIT_LO_RST_WIA2   0x05
#define TLIMIT_HI_RST_WIA2   0x05

#define TLIMIT_NO_FUSE_SET   0x105
#define TLIMIT_NO_FUSE_READ  0x106

#define TLIMIT_NO_ASAX       0x107
#define TLIMIT_LO_ASAX       1
#define TLIMIT_HI_ASAX       254

#define TLIMIT_NO_ASAY       0x108
#define TLIMIT_LO_ASAY       1
#define TLIMIT_HI_ASAY       254

#define TLIMIT_NO_ASAZ       0x109
#define TLIMIT_LO_ASAZ       1
#define TLIMIT_HI_ASAZ       254

#define TLIMIT_NO_FUSE_PDN   0x10A

#define TLIMIT_NO_SNG_CNTL2  0x201
#define TLIMIT_NO_SNG_WAIT   0x202

#define TLIMIT_NO_SNG_ST1    0x203
#define TLIMIT_LO_SNG_ST1    1
#define TLIMIT_HI_SNG_ST1    1

#define TLIMIT_NO_SNG_HX     0x204
#define TLIMIT_LO_SNG_HX     -8189
#define TLIMIT_HI_SNG_HX     8189

#define TLIMIT_NO_SNG_HY     0x206
#define TLIMIT_LO_SNG_HY     -8189
#define TLIMIT_HI_SNG_HY     8189

#define TLIMIT_NO_SNG_HZ     0x208
#define TLIMIT_LO_SNG_HZ     -8189
#define TLIMIT_HI_SNG_HZ     8189

#define TLIMIT_NO_SNG_ST2    0x20A
#define TLIMIT_LO_SNG_ST2    0
#define TLIMIT_HI_SNG_ST2    0

#define TLIMIT_NO_SLF_CNTL2  0x20B
#define TLIMIT_NO_SLF_WAIT   0x20C

#define TLIMIT_NO_SLF_ST1    0x20D
#define TLIMIT_LO_SLF_ST1    1
#define TLIMIT_HI_SLF_ST1    1

#define TLIMIT_NO_SLF_RVHX   0x20E
#define TLIMIT_LO_SLF_RVHX   -30
#define TLIMIT_HI_SLF_RVHX   30

#define TLIMIT_NO_SLF_RVHY   0x210
#define TLIMIT_LO_SLF_RVHY   -30
#define TLIMIT_HI_SLF_RVHY   30

#define TLIMIT_NO_SLF_RVHZ   0x212
#define TLIMIT_LO_SLF_RVHZ   -400
#define TLIMIT_HI_SLF_RVHZ   -50

#define TLIMIT_NO_SLF_ST2    0x214
#define TLIMIT_LO_SLF_ST2    0
#define TLIMIT_HI_SLF_ST2    0

/*
 * \result upper_16bit test number
 * \result lower_16bit test result data.
 */
int16_t ak09911_self_test(int32_t *result)
{
    int16_t fret;
    uint8_t i2cData[AK099XX_BDATA_SIZE];
    uint8_t asa[3];
    int16_t xval_i16, yval_i16, zval_i16;

    /* initialize arg */
    *result = 0;

    /**********************************************************************
     * Step 1
     **********************************************************************/

    /* Soft Reset */
    fret = ak099xx_soft_reset();

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_RST, fret);
        goto SELFTEST_FAIL;
    }

    /* Wait over 100 us */
    AKH_DelayMicro(100);

    /* Read values. */
    fret = AKH_RxData(AKM_ST_MAG, AK099XX_REG_WIA1, i2cData, 2);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_RST_READ, fret);
        goto SELFTEST_FAIL;
    }

    AKM_FST(TLIMIT_NO_RST_WIA1, i2cData[0], TLIMIT_LO_RST_WIA1,
            TLIMIT_HI_RST_WIA1, result);
    AKM_FST(TLIMIT_NO_RST_WIA2, i2cData[1], TLIMIT_LO_RST_WIA2,
            TLIMIT_HI_RST_WIA2, result);

    /* Read FUSE ROM value */
    fret = ak099xx_set_mode(AK099XX_MODE_FUSE_ACCESS);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_FUSE_SET, fret);
        goto SELFTEST_FAIL;
    }

    fret = AKH_RxData(AKM_ST_MAG, AK099XX_FUSE_ASAX, asa, 3);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_FUSE_READ, fret);
        goto SELFTEST_FAIL;
    }

    AKM_FST(TLIMIT_NO_ASAX, asa[0], TLIMIT_LO_ASAX, TLIMIT_HI_ASAX, result);
    AKM_FST(TLIMIT_NO_ASAY, asa[1], TLIMIT_LO_ASAY, TLIMIT_HI_ASAY, result);
    AKM_FST(TLIMIT_NO_ASAZ, asa[2], TLIMIT_LO_ASAZ, TLIMIT_HI_ASAZ, result);

    fret = ak099xx_set_mode(AK099XX_MODE_POWER_DOWN);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_FUSE_PDN, fret);
        goto SELFTEST_FAIL;
    }


    /**********************************************************************
     * Step 2
     **********************************************************************/

    /* Set to SNG measurement pattern. */
    fret = ak099xx_set_mode(AK099XX_MODE_SNG_MEASURE);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SNG_CNTL2, fret);
        goto SELFTEST_FAIL;
    }

    /* Wait for single measurement. */
    AKH_DelayMilli(10);

    /*
     * Get measurement data from AK09911
     * ST1 + (HXL/H) + (HYL/H) + (HZL/H) + TMPS + ST2 = 9bytes */
    fret = AKH_RxData(
            AKM_ST_MAG, AK099XX_REG_ST1, i2cData, AK099XX_BDATA_SIZE);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SNG_WAIT, fret);
        goto SELFTEST_FAIL;
    }

    /* Convert to 16-bit integer value. */
    xval_i16 = (int16_t)(((uint16_t)i2cData[1]) | ((uint16_t)i2cData[2] << 8));
    yval_i16 = (int16_t)(((uint16_t)i2cData[3]) | ((uint16_t)i2cData[4] << 8));
    zval_i16 = (int16_t)(((uint16_t)i2cData[5]) | ((uint16_t)i2cData[6] << 8));

    AKM_FST(TLIMIT_NO_SNG_ST1, i2cData[0], TLIMIT_LO_SNG_ST1,
            TLIMIT_HI_SNG_ST1, result);
    AKM_FST(TLIMIT_NO_SNG_HX, xval_i16, TLIMIT_LO_SNG_HX,
            TLIMIT_HI_SNG_HX, result);
    AKM_FST(TLIMIT_NO_SNG_HY, yval_i16, TLIMIT_LO_SNG_HY,
            TLIMIT_HI_SNG_HY, result);
    AKM_FST(TLIMIT_NO_SNG_HZ, zval_i16, TLIMIT_LO_SNG_HZ,
            TLIMIT_HI_SNG_HZ, result);
    AKM_FST(TLIMIT_NO_SNG_ST2, i2cData[8], TLIMIT_LO_SNG_ST2,
            TLIMIT_HI_SNG_ST2, result);

    /* Set to self-test mode. */
    fret = ak099xx_set_mode(AK099XX_MODE_SELF_TEST);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SLF_CNTL2, fret);
        goto SELFTEST_FAIL;
    }

    /* Wait for self-test measurement. */
    AKH_DelayMilli(10);

    /*
     * Get measurement data from AK09911
     * ST1 + (HXL + HXH) + (HYL + HYH) + (HZL + HZH) + TMPS + ST2 = 9bytes */
    fret = AKH_RxData(
            AKM_ST_MAG, AK099XX_REG_ST1, i2cData, AK099XX_BDATA_SIZE);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SLF_WAIT, fret);
        goto SELFTEST_FAIL;
    }

    /* Calculate adjusted magnetic vector. */
    xval_i16 = (int16_t)(((uint16_t)i2cData[1]) | ((uint16_t)i2cData[2] << 8));
    yval_i16 = (int16_t)(((uint16_t)i2cData[3]) | ((uint16_t)i2cData[4] << 8));
    zval_i16 = (int16_t)(((uint16_t)i2cData[5]) | ((uint16_t)i2cData[6] << 8));
    xval_i16 = (xval_i16 * (asa[0] + 128)) >> 7;
    yval_i16 = (yval_i16 * (asa[1] + 128)) >> 7;
    zval_i16 = (zval_i16 * (asa[2] + 128)) >> 7;

    AKM_FST(TLIMIT_NO_SLF_ST1, i2cData[0], TLIMIT_LO_SLF_ST1,
            TLIMIT_HI_SLF_ST1, result);
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX,
            TLIMIT_HI_SLF_RVHX, result);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY,
            TLIMIT_HI_SLF_RVHY, result);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ,
            TLIMIT_HI_SLF_RVHZ, result);
    AKM_FST(TLIMIT_NO_SLF_ST2, i2cData[8], TLIMIT_LO_SLF_ST2,
            TLIMIT_HI_SLF_ST2, result);

    return AKM_SUCCESS;

SELFTEST_FAIL:
    return AKM_ERROR;
}
