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
#include "ak8963_register.h"
#include "aks_common.h"
#include "aks_mag_ak8963.h"
#include "AKH_APIs.h"

#define TLIMIT_NO_RST         0x100
#define TLIMIT_NO_RST_SPI     0x101
#define TLIMIT_NO_RST_READ    0x102

#define TLIMIT_NO_RST_WIA     0x103
#define TLIMIT_LO_RST_WIA     0x48
#define TLIMIT_HI_RST_WIA     0x48

#define TLIMIT_NO_RST_INFO    0x104
#define TLIMIT_LO_RST_INFO    0
#define TLIMIT_HI_RST_INFO    255

#define TLIMIT_NO_RST_ST1     0x105
#define TLIMIT_LO_RST_ST1     0
#define TLIMIT_HI_RST_ST1     0

#define TLIMIT_NO_RST_HXL     0x106
#define TLIMIT_LO_RST_HXL     0
#define TLIMIT_HI_RST_HXL     0

#define TLIMIT_NO_RST_HXH     0x107
#define TLIMIT_LO_RST_HXH     0
#define TLIMIT_HI_RST_HXH     0

#define TLIMIT_NO_RST_HYL     0x108
#define TLIMIT_LO_RST_HYL     0
#define TLIMIT_HI_RST_HYL     0

#define TLIMIT_NO_RST_HYH     0x109
#define TLIMIT_LO_RST_HYH     0
#define TLIMIT_HI_RST_HYH     0

#define TLIMIT_NO_RST_HZL     0x10A
#define TLIMIT_LO_RST_HZL     0
#define TLIMIT_HI_RST_HZL     0

#define TLIMIT_NO_RST_HZH     0x10B
#define TLIMIT_LO_RST_HZH     0
#define TLIMIT_HI_RST_HZH     0

#define TLIMIT_NO_RST_ST2     0x10C
#define TLIMIT_LO_RST_ST2     0
#define TLIMIT_HI_RST_ST2     0

#define TLIMIT_NO_RST_CNTL1   0x10D
#define TLIMIT_LO_RST_CNTL1   0
#define TLIMIT_HI_RST_CNTL1   0

#define TLIMIT_NO_RST_CNTL2   0x10E
#define TLIMIT_LO_RST_CNTL2   0
#define TLIMIT_HI_RST_CNTL2   0

#define TLIMIT_NO_RST_ASTC    0x10F
#define TLIMIT_LO_RST_ASTC    0
#define TLIMIT_HI_RST_ASTC    0

#define TLIMIT_NO_RST_I2CDIS  0x110
#define TLIMIT_LO_RST_USEI2C  0
#define TLIMIT_HI_RST_USEI2C  0
#define TLIMIT_LO_RST_USESPI  1
#define TLIMIT_HI_RST_USESPI  1

#define TLIMIT_NO_FUSE_SET    0x111
#define TLIMIT_NO_FUSE_READ   0x112

#define TLIMIT_NO_ASAX        0x113
#define TLIMIT_LO_ASAX        1
#define TLIMIT_HI_ASAX        254

#define TLIMIT_NO_ASAY        0x114
#define TLIMIT_LO_ASAY        1
#define TLIMIT_HI_ASAY        254

#define TLIMIT_NO_ASAZ        0x115
#define TLIMIT_LO_ASAZ        1
#define TLIMIT_HI_ASAZ        254

#define TLIMIT_NO_FUSE_PDN    0x116

#define TLIMIT_NO_SNG_CNTL1   0x201
#define TLIMIT_NO_SNG_WAIT    0x202

#define TLIMIT_NO_SNG_ST1     0x203
#define TLIMIT_LO_SNG_ST1     1
#define TLIMIT_HI_SNG_ST1     1

#define TLIMIT_NO_SNG_HX      0x204
#define TLIMIT_LO_SNG_HX      -32759
#define TLIMIT_HI_SNG_HX      32759

#define TLIMIT_NO_SNG_HY      0x206
#define TLIMIT_LO_SNG_HY      -32759
#define TLIMIT_HI_SNG_HY      32759

#define TLIMIT_NO_SNG_HZ      0x208
#define TLIMIT_LO_SNG_HZ      -32759
#define TLIMIT_HI_SNG_HZ      32759

#define TLIMIT_NO_SNG_ST2     0x20A
#ifdef AK8963_14BIT_MODE
#define TLIMIT_LO_SNG_ST2     0
#define TLIMIT_HI_SNG_ST2     0
#else
#define TLIMIT_LO_SNG_ST2     16
#define TLIMIT_HI_SNG_ST2     16
#endif

#define TLIMIT_NO_SLF_ASTC   0x20B
#define TLIMIT_NO_SLF_CNTL1  0x20C
#define TLIMIT_NO_SLF_WAIT   0x20D

#define TLIMIT_NO_SLF_ST1    0x20E
#define TLIMIT_LO_SLF_ST1    1
#define TLIMIT_HI_SLF_ST1    1

#define TLIMIT_NO_SLF_RVHX   0x20F
#ifdef AK8963_14BIT_MODE
#define TLIMIT_LO_SLF_RVHX   -50
#define TLIMIT_HI_SLF_RVHX   50
#else
#define TLIMIT_LO_SLF_RVHX   -200
#define TLIMIT_HI_SLF_RVHX   200
#endif

#define TLIMIT_NO_SLF_RVHY  0x211
#ifdef AK8963_14BIT_MODE
#define TLIMIT_LO_SLF_RVHY  -50
#define TLIMIT_HI_SLF_RVHY  50
#else
#define TLIMIT_LO_SLF_RVHY  -200
#define TLIMIT_HI_SLF_RVHY  200
#endif

#define TLIMIT_NO_SLF_RVHZ  0x213
#ifdef AK8963_14BIT_MODE
#define TLIMIT_LO_SLF_RVHZ  -800
#define TLIMIT_HI_SLF_RVHZ  -200
#else
#define TLIMIT_LO_SLF_RVHZ  -3200
#define TLIMIT_HI_SLF_RVHZ  -800
#endif

#define TLIMIT_NO_SLF_ST2  0x215
#ifdef AK8963_14BIT_MODE
#define TLIMIT_LO_SLF_ST2  0
#define TLIMIT_HI_SLF_ST2  0
#else
#define TLIMIT_LO_SLF_ST2  16
#define TLIMIT_HI_SLF_ST2  16
#endif

#define TLIMIT_NO_SLF_ASTC_RST  0x216

/*
 * \result upper_16bit test number
 * \result lower_16bit test result data.
 */
int16_t ak8963_self_test(int32_t *result)
{
    int16_t fret;
    uint8_t i2cData[13];
    uint8_t asa[3];
    int16_t xval_i16, yval_i16, zval_i16;

    /**********************************************************************
     * Step 1
     **********************************************************************/

    /* Soft Reset */
    fret = ak8963_soft_reset();

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_RST, fret);
        goto SELFTEST_FAIL;
    }

    /* Wait over 100 us */
    AKH_DelayMicro(100);

    /* When the serial interface is SPI,
     * write "00011011" to I2CDIS register(to disable I2C,). */
#ifdef AKM_SPI_USED
    i2cData[0] = 0x1B;
    fret = AKH_TxData(AKM_ST_MAG, AK8963_REG_I2CDIS, i2cData, 1);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_RST_SPI, fret);
        goto SELFTEST_FAIL;
    }
#endif
    /* Read values. */
    fret = AKH_RxData(AKM_ST_MAG, AK8963_REG_WIA, i2cData, 13);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_RST_READ, fret);
        goto SELFTEST_FAIL;
    }

    AKM_FST(TLIMIT_NO_RST_WIA, i2cData[0], TLIMIT_LO_RST_WIA,
            TLIMIT_HI_RST_WIA, result);
    AKM_FST(TLIMIT_NO_RST_INFO, i2cData[1], TLIMIT_LO_RST_INFO,
            TLIMIT_HI_RST_INFO, result);
    AKM_FST(TLIMIT_NO_RST_ST1, i2cData[2], TLIMIT_LO_RST_ST1,
            TLIMIT_HI_RST_ST1, result);
    AKM_FST(TLIMIT_NO_RST_HXL, i2cData[3], TLIMIT_LO_RST_HXL,
            TLIMIT_HI_RST_HXL, result);
    AKM_FST(TLIMIT_NO_RST_HXH, i2cData[4], TLIMIT_LO_RST_HXH,
            TLIMIT_HI_RST_HXH, result);
    AKM_FST(TLIMIT_NO_RST_HYL, i2cData[5], TLIMIT_LO_RST_HYL,
            TLIMIT_HI_RST_HYL, result);
    AKM_FST(TLIMIT_NO_RST_HYH, i2cData[6], TLIMIT_LO_RST_HYH,
            TLIMIT_HI_RST_HYH, result);
    AKM_FST(TLIMIT_NO_RST_HZL, i2cData[7], TLIMIT_LO_RST_HZL,
            TLIMIT_HI_RST_HZL, result);
    AKM_FST(TLIMIT_NO_RST_HZH, i2cData[8], TLIMIT_LO_RST_HZH,
            TLIMIT_HI_RST_HZH, result);
    AKM_FST(TLIMIT_NO_RST_ST2, i2cData[9], TLIMIT_LO_RST_ST2,
            TLIMIT_HI_RST_ST2, result);
    AKM_FST(TLIMIT_NO_RST_CNTL1, i2cData[10], TLIMIT_LO_RST_CNTL1,
            TLIMIT_HI_RST_CNTL1, result);
    AKM_FST(TLIMIT_NO_RST_CNTL2, i2cData[11], TLIMIT_LO_RST_CNTL2,
            TLIMIT_HI_RST_CNTL2, result);
    AKM_FST(TLIMIT_NO_RST_ASTC, i2cData[12], TLIMIT_LO_RST_ASTC,
            TLIMIT_HI_RST_ASTC, result);

    /* Read I2CDIS value. */
    fret = AKH_RxData(AKM_ST_MAG, AK8963_REG_I2CDIS, i2cData, 1);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_RST_I2CDIS, fret);
        goto SELFTEST_FAIL;
    }

    /* TEST I2CDIS value. */
#ifdef AKM_SPI_USED
    AKM_FST(TLIMIT_NO_RST_I2CDIS, i2cData[0], TLIMIT_LO_RST_USESPI,
            TLIMIT_HI_RST_USESPI, result);
#else
    AKM_FST(TLIMIT_NO_RST_I2CDIS, i2cData[0], TLIMIT_LO_RST_USEI2C,
            TLIMIT_HI_RST_USEI2C, result);
#endif

    /* Read FUSE ROM value */
    fret = ak8963_set_mode(AK8963_CNTL1_FUSE_ACCESS);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_FUSE_SET, fret);
        goto SELFTEST_FAIL;
    }

    fret = AKH_RxData(AKM_ST_MAG, AK8963_FUSE_ASAX, asa, 3);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_FUSE_READ, fret);
        goto SELFTEST_FAIL;
    }

    AKM_FST(TLIMIT_NO_ASAX, asa[0], TLIMIT_LO_ASAX,
            TLIMIT_HI_ASAX, result);
    AKM_FST(TLIMIT_NO_ASAY, asa[1], TLIMIT_LO_ASAY,
            TLIMIT_HI_ASAY, result);
    AKM_FST(TLIMIT_NO_ASAZ, asa[2], TLIMIT_LO_ASAZ,
            TLIMIT_HI_ASAZ, result);

    fret = ak8963_set_mode(AK8963_CNTL1_POWER_DOWN);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_FUSE_PDN, fret);
        goto SELFTEST_FAIL;
    }

    /**********************************************************************
     * Step 2
     **********************************************************************/

    /* Set to SNG measurement pattern (Set CNTL1 register) */
    fret = ak8963_set_mode(AK8963_CNTL1_SNG_MEASURE);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SNG_CNTL1, fret);
        goto SELFTEST_FAIL;
    }

    AKH_DelayMilli(10);

    /*
     * Get measurement data from AK8963
     * ST1 + (HXL + HXH) + (HYL + HYH) + (HZL + HZH) + ST2
     *  = 1 + (1 + 1) + (1 + 1) + (1 + 1) + 1 = 8Byte */
    fret = AKH_RxData(AKM_ST_MAG, AK8963_REG_ST1, i2cData, AK8963_BDATA_SIZE);

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
    AKM_FST(TLIMIT_NO_SNG_ST2, i2cData[7], TLIMIT_LO_SNG_ST2,
            TLIMIT_HI_SNG_ST2, result);

    /* Generate magnetic field for self-test (Set ASTC register) */
    i2cData[0] = 0x40;
    fret = AKH_TxData(AKM_ST_MAG, AK8963_REG_ASTC, i2cData, 1);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SLF_ASTC, fret);
        goto SELFTEST_FAIL;
    }

    /* Set to Self-test mode (Set CNTL1 register) */
    fret = ak8963_set_mode(AK8963_CNTL1_SELF_TEST);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SLF_CNTL1, fret);
        goto SELFTEST_FAIL;
    }

    AKH_DelayMilli(10);

    /*
     * Get measurement data from AK8963
     * ST1 + (HXL + HXH) + (HYL + HYH) + (HZL + HZH) + ST2
     *  = 1 + (1 + 1) + (1 + 1) + (1 + 1) + 1 = 8Byte */
    fret = AKH_RxData(AKM_ST_MAG, AK8963_REG_ST1, i2cData,
                      AK8963_BDATA_SIZE);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SNG_WAIT, fret);
        goto SELFTEST_FAIL;
    }

    /* Calculate adjusted magnetic vector. */
    xval_i16 = (int16_t)(((uint16_t)i2cData[1]) | ((uint16_t)i2cData[2] << 8));
    yval_i16 = (int16_t)(((uint16_t)i2cData[3]) | ((uint16_t)i2cData[4] << 8));
    zval_i16 = (int16_t)(((uint16_t)i2cData[5]) | ((uint16_t)i2cData[6] << 8));
    xval_i16 = (xval_i16 * (asa[0] + 128)) >> 8;
    yval_i16 = (yval_i16 * (asa[1] + 128)) >> 8;
    zval_i16 = (zval_i16 * (asa[2] + 128)) >> 8;

    AKM_FST(TLIMIT_NO_SLF_ST1, i2cData[0], TLIMIT_LO_SLF_ST1,
            TLIMIT_HI_SLF_ST1, result);
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX,
            TLIMIT_HI_SLF_RVHX, result);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY,
            TLIMIT_HI_SLF_RVHY, result);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ,
            TLIMIT_HI_SLF_RVHZ, result);
    AKM_FST(TLIMIT_NO_SLF_ST2, i2cData[7], TLIMIT_LO_SLF_ST2,
            TLIMIT_HI_SLF_ST2, result);

    /* Reset ASTC register */
    i2cData[0] = 0x00;
    fret = AKH_TxData(AKM_ST_MAG, AK8963_REG_ASTC, i2cData, 1);

    if (AKM_SUCCESS != fret) {
        *result = AKM_FST_ERRCODE(TLIMIT_NO_SLF_ASTC_RST, fret);
        goto SELFTEST_FAIL;
    }

    return AKM_SUCCESS;

SELFTEST_FAIL:
    return AKM_ERROR;
}
