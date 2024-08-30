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
#include "AKH_APIs.h"
#include "ak0994x_register.h"
#include "aks_common.h"
#include "aks_mag_ak0994x.h"

#define SENS_0010_Q16  ((int32_t)(655)) /* 0.010 in Q16 format */

#define AVE_TIMES   4

/* Global variable for ASA value. */
static AKM_DEVICES g_device = AKM_DEVICE_NONE;
static int32_t     g_raw_to_micro_q16[3];
static uint8_t     g_mag_axis_order[3];
static uint8_t     g_mag_axis_sign[3];

#ifdef AKM_MAGNETOMETER_DRDY_EN
static AKM_TIMESTAMP g_mag_ts;
void mag_ak0994x_irq_handler(void)
{
    g_mag_ts = AKH_GetTimestamp();
}
#endif

static struct aks_interface ak0994x_interface = {
    .aks_init = ak0994x_init,
    .aks_get_info = ak0994x_get_info,
    .aks_start = ak0994x_start,
    .aks_stop = ak0994x_stop,
    .aks_check_rdy = ak0994x_check_rdy,
    .aks_get_data = ak0994x_get_data,
    .aks_self_test = NULL
};

static int32_t ak0994x_self_test_compensation(void)
{
    uint8_t i2cData[AK0994X_BDATA_SIZE];
    uint8_t i;
    uint8_t num;
    int16_t fret;
    int32_t tmp;
    int32_t ave_cur[3] = {0};
    int16_t ref[3];

    for (num = 0; num < AVE_TIMES; num++) {
        /* Set to self-test mode. */
        i2cData[0] = 0x70;
        fret = AKH_TxData(AKM_ST_MAG, AK0994X_REG_CNTL3, &i2cData[0], 1);

        if (fret != AKM_SUCCESS) {
            return fret;
        }

        /* Wait over 5 ms */
        AKH_DelayMilli(5);

        /* Read data */
        fret = AKH_RxData(
                AKM_ST_MAG, AK0994X_REG_ST1, i2cData, AK0994X_BDATA_SIZE);

        if (fret != AKM_SUCCESS) {
            return fret;
        }

        for (i = 0; i < 3; i++) {
            /* convert to int32 data */
            tmp = (int32_t)(
                    ((uint32_t)i2cData[i * 3 + 3] << 24) |
                    ((uint32_t)i2cData[i * 3 + 2] << 16) |
                    ((uint32_t)i2cData[i * 3 + 1] << 8)) >> 8;
            ave_cur[i] += tmp;
        }
    }

    /* Averaging */
    for (i = 0; i < 3; i++) {
        ave_cur[i] = ave_cur[i] / AVE_TIMES;
    }

    /* Read reference data */
    fret = AKH_RxData(
            AKM_ST_MAG, AK0994X_REG_SXL, i2cData, AK0994X_SDATA_SIZE);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    for (i = 0; i < 3; i++) {
        /* convert to int16 data */
        tmp = (int16_t)(
              ((uint16_t)i2cData[i * 2 + 1] << 13) |
              ((uint16_t)i2cData[i * 2] << 5)) >> 5;
        if (i == 2) {
            tmp = (int16_t)(
                  ((uint16_t)i2cData[i * 2 + 1] << 12) |
                  ((uint16_t)i2cData[i * 2] << 4)) >> 4;
        }
        ref[i] = tmp;
    }

    /* Converts from normal to compensated data. */
    g_raw_to_micro_q16[0] = SENS_0010_Q16 * ref[0] / ave_cur[0];
    g_raw_to_micro_q16[1] = SENS_0010_Q16 * ref[1] / ave_cur[1];
    g_raw_to_micro_q16[2] = SENS_0010_Q16 * ref[2] / ave_cur[2];

    return AKM_SUCCESS;
}

/******************************************************************************/
/***** AKS public APIs ********************************************************/
int16_t ak0994x_config(
    AKM_DEVICES          *mag_dev,
    struct aks_interface **mag_if)
{
    uint8_t  i2cData[4];
    uint16_t dev;
    int16_t  fret;

    AKH_Print("ak0994x_config: Start\n");

    /* Read WIA */
    fret = AKH_RxData(AKM_ST_MAG, AK0994X_REG_WIA1, i2cData, 4);

    if (fret != AKM_SUCCESS) {
        AKH_Print("ak0994x_config: Failed to read WIA\n");
        return fret;
    }

    /* Store device id (actually, it is company id.) */
    dev = (((uint16_t)i2cData[1] << 8) | i2cData[0]);
    AKH_Print("ak0994x_config: Device ID: 0x%04X\n", dev);

    /* Detect device number */
    switch (dev) {
    case AK09940A_WIA_VAL:
        g_device = AKM_MAGNETOMETER_AK09940A;
        ak0994x_interface.aks_self_test = ak09940a_self_test;
        AKH_Print("ak0994x_config: Detected AKM_MAGNETOMETER_AK09940A\n");
        break;
    default:
        AKH_Print("ak0994x_config: Unsupported device ID: 0x%04X\n", dev);
        return AKM_ERR_NOT_SUPPORT;
    }

    *mag_dev = g_device;
    *mag_if = &ak0994x_interface;

    AKH_Print("ak0994x_config: End\n");

    return AKM_SUCCESS;
}

int16_t ak0994x_set_mode(const uint8_t mode)
{
    uint8_t i2cData;
    int16_t fret;

#ifdef AKM_USE_ULTRA_LOW_POWER_DRIVE_AK09940
    i2cData = 0x80;
    fret = AKH_TxData(AKM_ST_MAG, AK0994X_REG_CNTL1, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }
#endif

    i2cData = mode;
#ifdef AKM_USE_LOW_POWER_DRIVE_2_AK09940
    i2cData |= 0x20;
#endif

#ifdef AKM_USE_LOW_NOISE_DRIVE_1_AK09940
    i2cData |= 0x40;
#endif

#ifdef AKM_USE_LOW_NOISE_DRIVE_2_AK09940
    i2cData |= 0x60;
#endif
    if(mode == AK0994X_MODE_SELF_TEST) {
        // Set Low noise drive 2
        i2cData |= 0x60;
    }
    fret = AKH_TxData(AKM_ST_MAG, AK0994X_REG_CNTL3, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* When succeeded, sleep 'Twait' */
    AKH_DelayMicro(100);
    return AKM_SUCCESS;
}

int16_t ak0994x_soft_reset(void)
{
    uint8_t i2cData;
    int16_t fret;

    /* Soft Reset */
    i2cData = AK0994X_SOFT_RESET;
    fret = AKH_TxData(AKM_ST_MAG, AK0994X_REG_CNTL4, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* When succeeded, sleep 'Twait' */
    AKH_DelayMicro(100);
    return AKM_SUCCESS;
}

int16_t ak0994x_init(
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3])
{
    int16_t fret;

    fret = ak0994x_soft_reset();

    if (fret != AKM_SUCCESS) {
        return fret;
    }

#ifdef AKM_MAGNETOMETER_DRDY_EN
    fret = AKH_SetIRQHandler(IRQ_MAG_1, mag_ak0994x_irq_handler);

    if (fret != AKM_SUCCESS) {
        return fret;
    }
#endif

    /* Trust g_device parameter */
    if (g_device == AKM_DEVICE_NONE) {
        return AKM_ERR_NOT_SUPPORT;
    }

    /* Converts from raw to micro-tesla unit. */
    g_raw_to_micro_q16[0] = SENS_0010_Q16;
    g_raw_to_micro_q16[1] = SENS_0010_Q16;
    g_raw_to_micro_q16[2] = SENS_0010_Q16;

    /* axis conversion parameter */
    g_mag_axis_order[0] = axis_order[0];
    g_mag_axis_order[1] = axis_order[1];
    g_mag_axis_order[2] = axis_order[2];
    g_mag_axis_sign[0] = axis_sign[0];
    g_mag_axis_sign[1] = axis_sign[1];
    g_mag_axis_sign[2] = axis_sign[2];

#ifdef AKM_USE_SELF_TEST_COMPENSATION_AK09940
    fret = ak0994x_self_test_compensation();

    if (fret != AKM_SUCCESS) {
        return fret;
    }
#endif

    return AKM_SUCCESS;
}

int16_t ak0994x_get_info(struct AKS_DEVICE_INFO *info)
{
    switch (g_device) {
    case AKM_MAGNETOMETER_AK09940A:
        AKS_MyStrcpy(info->name, "AK09940A", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9940;
        info->parameter[1] = AK09940A_WIA_VAL;
        break;

    default:
        return AKM_ERR_NOT_SUPPORT;
    }

    info->device = g_device;
    info->parameter[2] = 0x80;
    info->parameter[3] = 0x80;
    info->parameter[4] = 0x80;

    return AKM_SUCCESS;
}

int16_t ak0994x_start(const int32_t interval_us)
{
    int16_t ret;

    if (0 > interval_us) {
        /* Single Measurement */
        ret = ak0994x_set_mode(AK0994X_MODE_SNG_MEASURE);
    } else if (400 > interval_us) {
        /* Out of range */
        ret = AKM_ERR_INVALID_ARG;
    } else if (1000 > interval_us) {
#if defined (AKM_USE_ULTRA_LOW_POWER_DRIVE_AK09940)
        /* 1000 - 2500 Hz */
        ret = ak0994x_set_mode(AK0994X_MODE_CONT_MEASURE_MODE8);
#else
        ret = AKM_ERR_INVALID_ARG;
#endif
    } else if (2500 > interval_us) {
#if defined (AKM_USE_LOW_POWER_DRIVE_1_AK09940) | defined(AKM_USE_ULTRA_LOW_POWER_DRIVE_AK09940)
        /* 400 - 1000 Hz */
        ret = ak0994x_set_mode(AK0994X_MODE_CONT_MEASURE_MODE7);
#else
        ret = AKM_ERR_INVALID_ARG;
#endif
    } else if (5000 > interval_us) {
#if defined (AKM_USE_LOW_POWER_DRIVE_1_AK09940) | defined(AKM_USE_LOW_POWER_DRIVE_2_AK09940) | defined(AKM_USE_ULTRA_LOW_POWER_DRIVE_AK09940)
        /* 200 - 400 Hz */
        ret = ak0994x_set_mode(AK0994X_MODE_CONT_MEASURE_MODE6);
#else
        ret = AKM_ERR_INVALID_ARG;
#endif
    } else if (10000 > interval_us) {
        /* 100 - 200 Hz */
        ret = ak0994x_set_mode(AK0994X_MODE_CONT_MEASURE_MODE5);
    } else if (20000 > interval_us) {
        /* 50 - 100 Hz */
        ret = ak0994x_set_mode(AK0994X_MODE_CONT_MEASURE_MODE4);
    } else if (50000 > interval_us) {
        /* 20 - 50 Hz */
        ret = ak0994x_set_mode(AK0994X_MODE_CONT_MEASURE_MODE3);
    } else if (100000 > interval_us) {
        /* 10 - 20 Hz */
        ret = ak0994x_set_mode(AK0994X_MODE_CONT_MEASURE_MODE2);
    } else {
        /* 10 Hz or slower */
        ret = ak0994x_set_mode(AK0994X_MODE_CONT_MEASURE_MODE1);
    }

    return ret;
}

int16_t ak0994x_stop(void)
{
    return ak0994x_set_mode(AK0994X_MODE_POWER_DOWN);
}

int16_t ak0994x_check_rdy(const int32_t timeout_us)
{
    uint8_t i2cData;
    int16_t fret;

    /* Check DRDY bit of ST register */
    fret = AKH_RxData(AKM_ST_MAG, AK0994X_REG_ST, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    return (i2cData & 0x01);
}

int16_t ak0994x_get_data(
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num)
{
    uint8_t i2cData[AK0994X_BDATA_SIZE];
    int32_t tmp;
    int16_t fret;
    uint8_t i;

    /* check arg */
    if (*num < 1) {
        return AKM_ERR_INVALID_ARG;
    }

    /* Read data */
    fret = AKH_RxData(
            AKM_ST_MAG, AK0994X_REG_ST1, i2cData, AK0994X_BDATA_SIZE);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    for (i = 0; i < 3; i++) {
        /* convert to int32 data */
        tmp = (int32_t)(
                ((uint32_t)i2cData[i * 3 + 3] << 24) |
                ((uint32_t)i2cData[i * 3 + 2] << 16) |
                ((uint32_t)i2cData[i * 3 + 1] << 8)) >> 8;
        /* convert to micro tesla in Q16 */
        /* tmp value is 18-bit data, so result will not over flow */
        data->u.v[i] = tmp * g_raw_to_micro_q16[i];
    }

    AKS_ConvertCoordinate(data->u.v, g_mag_axis_order, g_mag_axis_sign);

    data->stype = AKM_ST_MAG;
#ifdef AKM_MAGNETOMETER_DRDY_EN
    data->timestamp = g_mag_ts;
#else
    data->timestamp = AKH_GetTimestamp();
#endif
    data->status[0] = i2cData[0];
    data->status[1] = i2cData[AK0994X_BDATA_SIZE - 1];
    *num = 1;
    return AKM_SUCCESS;
}
