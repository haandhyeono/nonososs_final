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

#include "ak8963_register.h"
#include "aks_common.h"
#include "aks_mag_ak8963.h"

#if defined(AK8963_14BIT_MODE)
#define SENS_IN_Q16  (39322) /* 0.6  in Q16 format */
#else
#define SENS_IN_Q16  (9830)  /* 0.15 in Q16 format */
#endif

/* Global variable for ASA value. */
static uint16_t      g_dev;
static uint8_t       g_asa[3];
static int32_t       g_raw_to_micro_q16[3];
static uint8_t       g_mag_axis_order[3];
static uint8_t       g_mag_axis_sign[3];
static AKM_TIMESTAMP g_mag_ts;

void mag_ak8963_irq_handler(void)
{
    g_mag_ts = AKH_GetTimestamp();
}

static struct aks_interface ak8963_interface = {
    .aks_init = ak8963_init,
    .aks_get_info = ak8963_get_info,
    .aks_start = ak8963_start,
    .aks_stop = ak8963_stop,
    .aks_check_rdy = ak8963_check_rdy,
    .aks_get_data = ak8963_get_data,
    .aks_self_test = ak8963_self_test
};


/******************************************************************************/
/***** AKS public APIs ********************************************************/
int16_t ak8963_config(
    AKM_DEVICES          *mag_dev,
    struct aks_interface **mag_if)
{
    *mag_dev = AKM_MAGNETOMETER_AK8963;
    *mag_if = &ak8963_interface;

    return AKM_SUCCESS;
}

int16_t ak8963_set_mode(const uint8_t mode)
{
    uint8_t i2cData[2];
    int16_t fret;

    i2cData[0] = AK8963_bit_mode(mode);
    fret = AKH_TxData(AKM_ST_MAG, AK8963_REG_CNTL1, i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* When succeeded, sleep */
    AKH_DelayMicro(100);
    return AKM_SUCCESS;
}

int16_t ak8963_soft_reset(void)
{
    uint8_t i2cData[2];
    int16_t fret;

    /* Soft Reset */
    i2cData[0] = AK8963_CNTL2_SOFT_RESET;
    fret = AKH_TxData(AKM_ST_MAG, AK8963_REG_CNTL2, i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* When succeeded, sleep */
    AKH_DelayMicro(100);
    return AKM_SUCCESS;
}

int16_t ak8963_init(
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3])
{
    uint8_t i2cData;
    int16_t fret;

    fret = ak8963_soft_reset();

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    fret = AKH_SetIRQHandler(IRQ_MAG_1, mag_ak8963_irq_handler);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* Read WIA */
    fret = AKH_RxData(AKM_ST_MAG, AK8963_REG_WIA, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* Store device id (actually, it is company id.) */
    g_dev = i2cData;

    /* Read FUSE ROM value */
    fret = ak8963_set_mode(AK8963_CNTL1_FUSE_ACCESS);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    fret = AKH_RxData(AKM_ST_MAG, AK8963_FUSE_ASAX, g_asa, 3);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    fret = ak8963_set_mode(AK8963_CNTL1_POWER_DOWN);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* Calculate coeff which converts from raw to micro-tesla unit. */
    /* The equation is: H_adj = H_raw x ((ASA - 128)/256 + 1)
     * To convert micro tesla in Q16, multiply (SENS x 2^16)
     * Simplify the equation: coeff = ((ASA + 128) x SENS x 2^16) >> 8
     * So coeff = ((ASA + 128) x SENS) >> 8 */
    g_raw_to_micro_q16[0] = ((g_asa[0] + 128) * SENS_IN_Q16) >> 8;
    g_raw_to_micro_q16[1] = ((g_asa[1] + 128) * SENS_IN_Q16) >> 8;
    g_raw_to_micro_q16[2] = ((g_asa[2] + 128) * SENS_IN_Q16) >> 8;

    /* axis conversion parameter */
    g_mag_axis_order[0] = axis_order[0];
    g_mag_axis_order[1] = axis_order[1];
    g_mag_axis_order[2] = axis_order[2];
    g_mag_axis_sign[0] = axis_sign[0];
    g_mag_axis_sign[1] = axis_sign[1];
    g_mag_axis_sign[2] = axis_sign[2];
    return AKM_SUCCESS;
}

int16_t ak8963_get_info(struct AKS_DEVICE_INFO *info)
{
    AKS_MyStrcpy(info->name, "AK8963", AKS_INFO_NAME_SIZE);
    info->device = AKM_MAGNETOMETER_AK8963;
    info->parameter[0] = 8963;
    info->parameter[1] = g_dev;
    info->parameter[2] = g_asa[0];
    info->parameter[3] = g_asa[1];
    info->parameter[4] = g_asa[2];

    return AKM_SUCCESS;
}

int16_t ak8963_start(const int32_t interval_us)
{
    int16_t ret;

    if (0 > interval_us) {
        /* Single Measurement */
        ret = ak8963_set_mode(AK8963_CNTL1_SNG_MEASURE);
    } else if (10000 >= interval_us) {
        /* Out of range */
        ret = AKM_ERR_INVALID_ARG;
    } else if (125000 >= interval_us) {
        /* 8 - 100 Hz */
        ret = ak8963_set_mode(AK8963_CNTL1_CONT_MEASURE_MODE2);
    } else {
        /* 8 Hz or slower */
        ret = ak8963_set_mode(AK8963_CNTL1_CONT_MEASURE_MODE1);
    }

    return ret;
}

int16_t ak8963_stop(void)
{
    return ak8963_set_mode(AK8963_CNTL1_POWER_DOWN);
}

int16_t ak8963_check_rdy(const int32_t timeout_us)
{
    uint8_t i2cData;
    int16_t fret;

    /* Check DRDY bit of ST1 register */
    fret = AKH_RxData(AKM_ST_MAG, AK8963_REG_ST1, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* AK8963 has only one data.
     * So, return is 0 or 1. */
    return (i2cData & 0x01);
}

int16_t ak8963_get_data(
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num)
{
    uint8_t i2cData[AK8963_BDATA_SIZE];
    int16_t tmp;
    int16_t fret;
    uint8_t i;

    /* check arg */
    if (*num < 1) {
        return AKM_ERR_INVALID_ARG;
    }

    /* Read data */
    fret = AKH_RxData(
            AKM_ST_MAG, AK8963_REG_ST1, i2cData, AK8963_BDATA_SIZE);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    for (i = 0; i < 3; i++) {
        /* convert to int16 data */
        tmp = (int16_t)(((uint16_t)i2cData[i * 2 + 2] << 8)
                        | i2cData[i * 2 + 1]);
        /* multiply ASA and convert to micro tesla in Q16 */
        data->u.v[i] = tmp * g_raw_to_micro_q16[i];
    }

    AKS_ConvertCoordinate(data->u.v, g_mag_axis_order, g_mag_axis_sign);

    data->stype = AKM_ST_MAG;
    data->timestamp = g_mag_ts;
    data->status[0] = i2cData[0];
    data->status[1] = i2cData[7];
    *num = 1;
    return AKM_SUCCESS;
}
