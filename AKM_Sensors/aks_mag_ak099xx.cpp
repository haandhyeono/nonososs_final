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

#include "ak099xx_register.h"
#include "aks_common.h"
#include "aks_mag_ak099xx.h"

#define SENS_0600_Q16  ((int32_t)(39322)) /* 0.6  in Q16 format */
#define SENS_0150_Q16  ((int32_t)(9830))  /* 0.15 in Q16 format */

/* Global variable for ASA value. */
static AKM_DEVICES g_device = AKM_DEVICE_NONE;
static uint8_t     g_asa[3];
static int32_t     g_raw_to_micro_q16[3];
static uint8_t     g_mag_axis_order[3];
static uint8_t     g_mag_axis_sign[3];
#ifdef AKM_USE_FIFO
static AKM_TIMESTAMP g_prev_fifo_timestamp;
#endif

#ifdef AKM_MAGNETOMETER_DRDY_EN
static AKM_TIMESTAMP g_mag_ts;
void mag_ak099xx_irq_handler(void)
{
    g_mag_ts = AKH_GetTimestamp();
}

#endif

static struct aks_interface ak099xx_interface = {
    .aks_init = ak099xx_init,
    .aks_get_info = ak099xx_get_info,
    .aks_start = ak099xx_start,
    .aks_stop = ak099xx_stop,
    .aks_check_rdy = ak099xx_check_rdy,
    .aks_get_data = ak099xx_get_data,
    .aks_self_test = NULL
};

/******************************************************************************/
/***** AKS public APIs ********************************************************/
int16_t ak099xx_config(
    AKM_DEVICES          *mag_dev,
    struct aks_interface **mag_if)
{
    uint8_t  i2cData[4];
    uint16_t dev;
    int16_t  fret;

    /* Read WIA */
    fret = AKH_RxData(AKM_ST_MAG, AK099XX_REG_WIA1, i2cData, 4);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* Store device id (actually, it is company id.) */
    dev = (((uint16_t)i2cData[1] << 8) | i2cData[0]);

    /* Detect device number */
    switch (dev) {
    case AK09911_WIA_VAL:
        g_device = AKM_MAGNETOMETER_AK09911;
        ak099xx_interface.aks_self_test = ak09911_self_test;
        break;

    case AK09912_WIA_VAL:
        g_device = AKM_MAGNETOMETER_AK09912;
        ak099xx_interface.aks_self_test = ak09912_self_test;
        break;

    case AK09913_WIA_VAL:
        g_device = AKM_MAGNETOMETER_AK09913;
        ak099xx_interface.aks_self_test = ak09913_self_test;
        break;

    case AK09915_WIA_VAL:

        if (i2cData[3] == AK09915D_INFO_VAL) {
            g_device = AKM_MAGNETOMETER_AK09915D;
        } else {
            g_device = AKM_MAGNETOMETER_AK09915;
        }

        ak099xx_interface.aks_self_test = ak09915_self_test;
        break;

    case AK09916C_WIA_VAL:
        g_device = AKM_MAGNETOMETER_AK09916C;
        ak099xx_interface.aks_self_test = ak09916_self_test;
        break;

    case AK09916D_WIA_VAL:
        g_device = AKM_MAGNETOMETER_AK09916D;
        ak099xx_interface.aks_self_test = ak09916_self_test;
        break;

    case AK09917D_WIA_VAL:
        g_device = AKM_MAGNETOMETER_AK09917D;
        ak099xx_interface.aks_self_test = ak09917_self_test;
        break;

    case AK09918_WIA_VAL:
        g_device = AKM_MAGNETOMETER_AK09918;
        ak099xx_interface.aks_self_test = ak09918_self_test;
        break;

    case AK09919_WIA_VAL:
        g_device = AKM_MAGNETOMETER_AK09919;
        ak099xx_interface.aks_self_test = ak09919_self_test;
        break;

    default:
        return AKM_ERR_NOT_SUPPORT;
    }

    *mag_dev = g_device;
    *mag_if = &ak099xx_interface;

    return AKM_SUCCESS;
}

int16_t ak099xx_set_mode(const uint8_t mode)
{
    uint8_t i2cData;
    int16_t fret;

    i2cData = mode;
#ifdef AKM_USE_FIFO
    if((mode == AK099XX_MODE_CONT_MEASURE_MODE1) ||
       (mode == AK099XX_MODE_CONT_MEASURE_MODE2) ||
       (mode == AK099XX_MODE_CONT_MEASURE_MODE3) ||
       (mode == AK099XX_MODE_CONT_MEASURE_MODE4) ||
       (mode == AK099XX_MODE_CONT_MEASURE_MODE5) ||
       (mode == AK099XX_MODE_CONT_MEASURE_MODE6)) {
#ifdef AKM_CUSTOM_CONTINUOUS_MEASURE
        if((g_device == AKM_MAGNETOMETER_AK09917D) ||
        (g_device == AKM_MAGNETOMETER_AK09919)) {
            i2cData = AK099XX_SET_FIFO(i2cData);
        }
#else
        return AKM_ERR_NOT_SUPPORT;
#endif
    }
#endif

#if defined(AKM_USE_LOW_NOISE)
    if ((g_device == AKM_MAGNETOMETER_AK09915) ||
        (g_device == AKM_MAGNETOMETER_AK09915D) ||
        (g_device == AKM_MAGNETOMETER_AK09917D) ||
        (g_device == AKM_MAGNETOMETER_AK09919)) {
        i2cData = AK099XX_SET_LOWNOISE(i2cData);
    }
#endif
    fret = AKH_TxData(AKM_ST_MAG, AK099XX_REG_CNTL2, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

#ifdef AKM_USE_FIFO
    // Reset preview fifo timestamp
    g_prev_fifo_timestamp = 0;
#endif

    /* When succeeded, sleep 'Twait' */
    AKH_DelayMicro(100);
    return AKM_SUCCESS;
}

int16_t ak099xx_soft_reset(void)
{
    uint8_t i2cData;
    int16_t fret;

    /* Soft Reset */
    i2cData = AK099XX_SOFT_RESET;
    fret = AKH_TxData(AKM_ST_MAG, AK099XX_REG_CNTL3, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* When succeeded, sleep 'Twait' */
    AKH_DelayMicro(100);
    return AKM_SUCCESS;
}

int16_t ak099xx_init(
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3])
{
    int16_t fret;

    fret = ak099xx_soft_reset();

    if (fret != AKM_SUCCESS) {
        return fret;
    }

#ifdef AKM_MAGNETOMETER_DRDY_EN
    fret = AKH_SetIRQHandler(IRQ_MAG_1, mag_ak099xx_irq_handler);

    if (fret != AKM_SUCCESS) {
        return fret;
    }
#endif

    /* Trust g_device parameter */
    if (g_device == AKM_DEVICE_NONE) {
        return AKM_ERR_NOT_SUPPORT;
    }

    if ((g_device == AKM_MAGNETOMETER_AK09911) ||
        (g_device == AKM_MAGNETOMETER_AK09912)) {
        /* Read FUSE ROM value */
        fret = ak099xx_set_mode(AK099XX_MODE_FUSE_ACCESS);

        if (fret != AKM_SUCCESS) {
            return fret;
        }

        fret = AKH_RxData(AKM_ST_MAG, AK099XX_FUSE_ASAX, g_asa, 3);

        if (fret != AKM_SUCCESS) {
            return fret;
        }

        fret = ak099xx_set_mode(AK099XX_MODE_POWER_DOWN);

        if (fret != AKM_SUCCESS) {
            return fret;
        }
    } else {
        /* Other device does not have ASA. */
        g_asa[0] = 128;
        g_asa[1] = 128;
        g_asa[2] = 128;
    }

    /* Calculate coeff which converts from raw to micro-tesla unit. */
    if (g_device == AKM_MAGNETOMETER_AK09911) {
        /* The equation is: H_adj = H_raw x (ASA / 128 + 1)
         * Convert from LSB to micro tesla in Q16, multiply (SENS x 2^16)
         * Simplify the equation: coeff = ((ASA + 128) x SENS x 2^16) >> 7
         * In case of AK09911, SENS = 0.6.
         * So coeff = ((ASA + 128) x 39322) >> 7 */
        g_raw_to_micro_q16[0] = ((int32_t)(g_asa[0] + 128) * SENS_0600_Q16) >>
            7;
        g_raw_to_micro_q16[1] = ((int32_t)(g_asa[1] + 128) * SENS_0600_Q16) >>
            7;
        g_raw_to_micro_q16[2] = ((int32_t)(g_asa[2] + 128) * SENS_0600_Q16) >>
            7;
    } else if (g_device == AKM_MAGNETOMETER_AK09912) {
        /* The equation is: H_adj = H_raw x ((ASA - 128) / 256 + 1)
         * To convert micro tesla in Q16, multiply (SENS x 2^16)
         * Simplify the equation: coeff = ((ASA + 128) x SENS x 2^16) >> 8
         * In case of AK09912, SENS = 0.15
         * So coeff = ((ASA + 128) x 9830) >> 8 */
        g_raw_to_micro_q16[0] = ((int32_t)(g_asa[0] + 128) * SENS_0150_Q16) >>
            8;
        g_raw_to_micro_q16[1] = ((int32_t)(g_asa[1] + 128) * SENS_0150_Q16) >>
            8;
        g_raw_to_micro_q16[2] = ((int32_t)(g_asa[2] + 128) * SENS_0150_Q16) >>
            8;
    } else {
        /* AK09913 or newer devices does not have ASA register. It means that user
         * does not need to write adjustment equation.
         */
        g_raw_to_micro_q16[0] = SENS_0150_Q16;
        g_raw_to_micro_q16[1] = SENS_0150_Q16;
        g_raw_to_micro_q16[2] = SENS_0150_Q16;
    }

    /* axis conversion parameter */
    g_mag_axis_order[0] = axis_order[0];
    g_mag_axis_order[1] = axis_order[1];
    g_mag_axis_order[2] = axis_order[2];
    g_mag_axis_sign[0] = axis_sign[0];
    g_mag_axis_sign[1] = axis_sign[1];
    g_mag_axis_sign[2] = axis_sign[2];

#ifdef AKM_USE_FIFO
    g_prev_fifo_timestamp = 0;
#endif
    return AKM_SUCCESS;
}

int16_t ak099xx_get_info(struct AKS_DEVICE_INFO *info)
{
    switch (g_device) {
    case AKM_MAGNETOMETER_AK09911:
        AKS_MyStrcpy(info->name, "AK09911", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9911;
        info->parameter[1] = AK09911_WIA_VAL;
        break;

    case AKM_MAGNETOMETER_AK09912:
        AKS_MyStrcpy(info->name, "AK09912", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9912;
        info->parameter[1] = AK09912_WIA_VAL;
        break;

    case AKM_MAGNETOMETER_AK09913:
        AKS_MyStrcpy(info->name, "AK09913", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9913;
        info->parameter[1] = AK09913_WIA_VAL;
        break;

    case AKM_MAGNETOMETER_AK09915:
        AKS_MyStrcpy(info->name, "AK09915", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9915;
        info->parameter[1] = AK09915_WIA_VAL;
        break;

    case AKM_MAGNETOMETER_AK09915D:
        AKS_MyStrcpy(info->name, "AK09915D", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9915;
        info->parameter[1] = AK09915_WIA_VAL;
        break;

    case AKM_MAGNETOMETER_AK09916C:
        AKS_MyStrcpy(info->name, "AK09916C", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9916;
        info->parameter[1] = AK09916C_WIA_VAL;
        break;

    case AKM_MAGNETOMETER_AK09916D:
        AKS_MyStrcpy(info->name, "AK09916D", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9916;
        info->parameter[1] = AK09916D_WIA_VAL;
        break;

    case AKM_MAGNETOMETER_AK09917D:
        AKS_MyStrcpy(info->name, "AK09917D", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9917;
        info->parameter[1] = AK09917D_WIA_VAL;
        break;

    case AKM_MAGNETOMETER_AK09918:
        AKS_MyStrcpy(info->name, "AK09918", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9918;
        info->parameter[1] = AK09918_WIA_VAL;
        break;

    case AKM_MAGNETOMETER_AK09919:
        AKS_MyStrcpy(info->name, "AK09919", AKS_INFO_NAME_SIZE);
        info->parameter[0] = 9919;
        info->parameter[1] = AK09919_WIA_VAL;
        break;

    default:
        return AKM_ERR_NOT_SUPPORT;
    }

    info->device = g_device;
    info->parameter[2] = g_asa[0];
    info->parameter[3] = g_asa[1];
    info->parameter[4] = g_asa[2];

    return AKM_SUCCESS;
}

int16_t ak099xx_start(const int32_t interval_us)
{
    int16_t ret;

    if (g_device == AKM_MAGNETOMETER_AK09912) {
        /* Set NSF */
        uint8_t i2cData;

        i2cData = AK099XX_NSF_VAL;
        ret = AKH_TxData(AKM_ST_MAG, AK099XX_REG_CNTL1, &i2cData, 1);

        if (ret != AKM_SUCCESS) {
            return ret;
        }
    }
    else if(g_device == AKM_MAGNETOMETER_AK09917D) {
        /* Set NSF */
        uint8_t i2cData;

        i2cData = AK099XX_NSF_VAL;
#if defined(AKM_USE_FIFO)
        /* Set WM[4:0] */
        if(AKM_USE_FIFO_WATERMARK > 32) {
            return AKM_ERR_NOT_SUPPORT;
        }
        i2cData |= AKM_USE_FIFO_WATERMARK - 1;
#endif
        ret = AKH_TxData(AKM_ST_MAG, AK099XX_REG_CNTL1, &i2cData, 1);

        if (ret != AKM_SUCCESS) {
            return ret;
        }
    }
    else if (g_device == AKM_MAGNETOMETER_AK09919) {
        uint8_t i2cData = 0;
#if defined(AKM_USE_LOW_NOISE)
        /* Set ITS */
        i2cData |= AK099XX_ITS_VAL;
#endif
#if defined(AKM_USE_FIFO)
        /* Set WM[3:0] */
        if(AKM_USE_FIFO_WATERMARK > 16) {
            return AKM_ERR_NOT_SUPPORT;
        }
        i2cData |= AKM_USE_FIFO_WATERMARK - 1;
#endif
        ret = AKH_TxData(AKM_ST_MAG, AK099XX_REG_CNTL1, &i2cData, 1);

        if (ret != AKM_SUCCESS) {
            return ret;
        }
    }

    if (0 > interval_us) {
        /* Single Measurement */
        ret = ak099xx_set_mode(AK099XX_MODE_SNG_MEASURE);
    } else if (10000 > interval_us) {
        /* Out of range */
        ret = AKM_ERR_INVALID_ARG;
    } else if (20000 > interval_us) {
        /* 50 - 100 Hz */
        ret = ak099xx_set_mode(AK099XX_MODE_CONT_MEASURE_MODE4);
    } else if (50000 > interval_us) {
        /* 20 - 50 Hz */
        ret = ak099xx_set_mode(AK099XX_MODE_CONT_MEASURE_MODE3);
    } else if (100000 > interval_us) {
        /* 10 - 20 Hz */
        ret = ak099xx_set_mode(AK099XX_MODE_CONT_MEASURE_MODE2);
    } else {
        /* 10 Hz or slower */
        ret = ak099xx_set_mode(AK099XX_MODE_CONT_MEASURE_MODE1);
    }

#ifdef AKM_USE_FIFO
    g_prev_fifo_timestamp = AKH_GetTimestamp();
#endif
    return ret;
}

int16_t ak099xx_stop(void)
{
    return ak099xx_set_mode(AK099XX_MODE_POWER_DOWN);
}

int16_t ak099xx_check_rdy(const int32_t timeout_us)
{
    uint8_t i2cData;
    int16_t fret;

    /* Check DRDY bit of ST1 register */
    fret = AKH_RxData(AKM_ST_MAG, AK099XX_REG_ST1, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* AK09911/09912/09913 has only one data.
     * So, return is 0 or 1. */
    return (i2cData & 0x01);
}

int16_t ak099xx_get_data(
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num)
{
    uint8_t i2cData[AK099XX_BDATA_SIZE * 32];
    int16_t tmp;
    int16_t fret;
    uint8_t i;
    uint8_t st1;

#ifdef AKM_USE_FIFO
    uint8_t j;
    uint16_t read_bytes;
    uint8_t fnum;
    uint8_t copy_index;
    struct AKM_SENSOR_DATA fifoData[32];
    AKM_TIMESTAMP latest_timestamp;
#endif

    /* check arg */
#ifdef AKM_USE_FIFO
    if ((*num < 1) || (*num > AKM_USE_FIFO_WATERMARK)) {
        return AKM_ERR_INVALID_ARG;
    }
#else
    if ((*num < 1) || (*num > 32)) {
        return AKM_ERR_INVALID_ARG;
    }
#endif

#ifdef AKM_USE_FIFO
    // Decide latest timestamp
#ifdef AKM_MAGNETOMETER_DRDY_EN
    latest_timestamp = g_mag_ts;
#else
    latest_timestamp = AKH_GetTimestamp();
#endif
    /* Read ST1 */
    fret = AKH_RxData(AKM_ST_MAG, AK099XX_REG_ST1, i2cData, 1);
    if (fret != AKM_SUCCESS) {
        return fret;
    }
    st1 = i2cData[0];
    fnum = ((st1 & 0x7C) >> 2);

    /* Read HXH/L to ST2 */
    read_bytes = fnum * (AK099XX_BDATA_SIZE - 1);
    fret = AKH_RxData(AKM_ST_MAG, AK099XX_REG_MEASURE_DATA_HEAD, i2cData, read_bytes);
    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* Translate Q16 format */
    for(i = 0; i < fnum; i++) {
        for (j = 0; j < 3; j++) {
            /* convert to int16 data */
            if ((g_device == AKM_MAGNETOMETER_AK09917D)  ||
                (g_device == AKM_MAGNETOMETER_AK09919) ) {
                tmp = MAKE_S16(i2cData[(i * 8) + (j * 2)], i2cData[(i * 8) + (j * 2) + 1]);
            }
            else {
                tmp = MAKE_S16(i2cData[(i * 8) + (j * 2) + 1], i2cData[(i * 8) + (j * 2)]);
            }

            /* multiply ASA and convert to micro tesla in Q16 */
            fifoData[i].u.v[j] = tmp * g_raw_to_micro_q16[j];
        }

        AKS_ConvertCoordinate(fifoData[i].u.v, g_mag_axis_order, g_mag_axis_sign);
        fifoData[i].stype = AKM_ST_MAG;
        fifoData[i].status[0] = st1;
        fifoData[i].status[1] = i2cData[i * 8 + 7];
    }

    /* Calculate copy_index */
    copy_index = 0;
    if(fnum < *num) {
        // Copy latest data
        AKH_Print("read_num less than *num, copy data.\n");
        for(i = fnum; i < *num; i++) {
            fifoData[i] = fifoData[i - 1];
        }
    }
    else if (*num < fnum) {
        copy_index = fnum - *num;
    }

    /* Decide all timestamp and copy arrays */
    AKM_TIMESTAMP diff = latest_timestamp - g_prev_fifo_timestamp;
    AKM_TIMESTAMP addTime = (AKM_TIMESTAMP)(diff / *num);
    for(i = 0; i < *num; i++)
    {
        fifoData[copy_index].timestamp = g_prev_fifo_timestamp + (addTime * (i + 1));
        data[i] = fifoData[copy_index];
        copy_index++;
    }

    /* Update preview timestamp */
    g_prev_fifo_timestamp = latest_timestamp;
    return AKM_SUCCESS;
#else
    if(g_device == AKM_MAGNETOMETER_AK09919) {
        fret = AKH_RxData(
            AKM_ST_MAG, AK099XX_REG_ST1, i2cData, 1);
        st1 = i2cData[0];
        fret = AKH_RxData(
            AKM_ST_MAG, AK099XX_REG_MEASURE_DATA_HEAD, i2cData, AK099XX_BDATA_SIZE - 1);
    } else {
        fret = AKH_RxData(
            AKM_ST_MAG, AK099XX_REG_ST1, i2cData, AK099XX_BDATA_SIZE);
    }

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    for (i = 0; i < 3; i++) {
        /* convert to int16 data */
        if (g_device == AKM_MAGNETOMETER_AK09917D) {
            tmp = MAKE_S16(i2cData[i * 2 + 1], i2cData[i * 2 + 2]);
        } else if (g_device == AKM_MAGNETOMETER_AK09919) {
            tmp = MAKE_S16(i2cData[i * 2], i2cData[i * 2 + 1]);
        } else {
            tmp = MAKE_S16(i2cData[i * 2 + 2], i2cData[i * 2 + 1]);
        }

        /* multiply ASA and convert to micro tesla in Q16 */
        data->u.v[i] = tmp * g_raw_to_micro_q16[i];
    }

    AKS_ConvertCoordinate(data->u.v, g_mag_axis_order, g_mag_axis_sign);

    data->stype = AKM_ST_MAG;
#ifdef AKM_MAGNETOMETER_DRDY_EN
    data->timestamp = g_mag_ts;
#else
    data->timestamp = AKH_GetTimestamp();
#endif
    if(g_device == AKM_MAGNETOMETER_AK09919) {
        data->status[0] = st1;
        data->status[1] = i2cData[AK099XX_BDATA_SIZE - 2];
    } else {
        data->status[0] = i2cData[0];
        data->status[1] = i2cData[AK099XX_BDATA_SIZE - 1];
    }
    *num = 1;
    return AKM_SUCCESS;
#endif
}
