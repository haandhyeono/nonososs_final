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
#include "aks_acc_gyr_bmi160.h"
#include "aks_common.h"
#include "AKH_APIs.h"


#define BMI160_REG_CHIPID      0x00
#define BMI160_REG_ERR_REG     0x02
#define BMI160_REG_PMU_STATUS  0x03
#define BMI160_REG_GYR_DATA    0x0C
#define BMI160_REG_ACC_DATA    0x12
#define BMI160_REG_STATUS      0x1B
#define BMI160_REG_ACC_CONF    0x40
#define BMI160_REG_ACC_RANGE   0x41
#define BMI160_REG_GYR_CONF    0x42
#define BMI160_REG_GYR_RANGE   0x43
#define BMI160_REG_CMD         0x7E

#define BMI160_VAL_CHIPID      (0xD1)
/* datasheet section 2.11.12 */
#define BMI160_VAL_ACC_FS2G    (3)
#define BMI160_VAL_ACC_FS4G    (5)
#define BMI160_VAL_ACC_FS8G    (8)
#define BMI160_VAL_ACC_FS16G   (12)
/* datasheet section 2.11.14 */
#define BMI160_VAL_GYR_FS125   (4)
#define BMI160_VAL_GYR_FS250   (3)
#define BMI160_VAL_GYR_FS500   (2)
#define BMI160_VAL_GYR_FS1000  (1)
#define BMI160_VAL_GYR_FS2000  (0)
/* filter mode (accelerometer / gyroscope) */
/* datasheet section 2.4.1 and 2.4.2 */
#define BMI160_BWP_NORMAL  (2)
#define BMI160_BWP_OSR2    (1)
#define BMI160_BWP_OSR4    (0)

/* datasheet section 2.11.11 and 2.11.13 */
#define BMI160_DATA_RATE_0_78HZ  (1)
#define BMI160_DATA_RATE_1_56HZ  (2)
#define BMI160_DATA_RATE_3_12HZ  (3)
#define BMI160_DATA_RATE_6_25HZ  (4)
#define BMI160_DATA_RATE_12_5HZ  (5)
#define BMI160_DATA_RATE_25HZ    (6)
#define BMI160_DATA_RATE_50HZ    (7)
#define BMI160_DATA_RATE_100HZ   (8)
#define BMI160_DATA_RATE_200HZ   (9)
#define BMI160_DATA_RATE_400HZ   (10)
#define BMI160_DATA_RATE_800HZ   (11)
#define BMI160_DATA_RATE_1600HZ  (12)
#define BMI160_DATA_RATE_3200HZ  (13)

/* datasheet section 2.11.38 */
#define BMI160_CMD_ACC_SUSPEND  (0x10)
#define BMI160_CMD_ACC_NORMAL   (0x11)
#define BMI160_CMD_GYR_SUSPEND  (0x14)
#define BMI160_CMD_GYR_NORMAL   (0x15)
#define BMI160_CMD_SOFTRESET    (0xB6)

/***** sensor configuration (alternative setting) *****/
/***** use this definition in source code *************/
#define BMI160_ACC_RANGE  BMI160_VAL_ACC_FS4G
#define BMI160_GYR_RANGE  BMI160_VAL_GYR_FS2000
#define BMI160_ACC_BWP    BMI160_BWP_NORMAL
#define BMI160_GYR_BWP    BMI160_BWP_NORMAL
/******************************************************/

/* strict error check for debugging */
#define BMI160_CHECK_ERR_REG

#ifdef BMI160_CHECK_ERR_REG
#define RETURN_CHECK(ret)  check_err_reg(ret)
#else
#define RETURN_CHECK(ret)  (ret)
#endif

static AKM_DEVICES   g_acc_device = AKM_DEVICE_NONE;
static AKM_DEVICES   g_gyr_device = AKM_DEVICE_NONE;
static uint8_t       g_axis_order[3];
static uint8_t       g_axis_sign[3];
static int32_t       g_acc_sensitivity;
static int32_t       g_gyr_sensitivity;
static AKM_TIMESTAMP g_bmi_ts;

static struct aks_interface bmi160_acc_interface = {
    .aks_init = bmi160_init,
    .aks_get_info = bmi160_get_info,
    .aks_start = bmi160_acc_start,
    .aks_stop = bmi160_acc_stop,
    .aks_check_rdy = bmi160_acc_check_rdy,
    .aks_get_data = bmi160_acc_get_data,
    .aks_self_test = bmi160_acc_self_test
};

static struct aks_interface bmi160_gyr_interface = {
    .aks_init = bmi160_init,
    .aks_get_info = bmi160_get_info,
    .aks_start = bmi160_gyr_start,
    .aks_stop = bmi160_gyr_stop,
    .aks_check_rdy = bmi160_gyr_check_rdy,
    .aks_get_data = bmi160_gyr_get_data,
    .aks_self_test = bmi160_gyr_self_test
};

void acc_bmi160_irq_handler(void)
{
    g_bmi_ts = AKH_GetTimestamp();
}

/* This function just convert the number. Validation of the value
 * should be done in each sensor API.
 */
static uint8_t bmi160_interval_us_to_odr(const int32_t interval_us)
{
    uint8_t setting;

    if (interval_us > 1280000) {
        setting = BMI160_DATA_RATE_0_78HZ; /* 0.78 Hz */
    } else if (interval_us > 640000) {
        setting = BMI160_DATA_RATE_1_56HZ; /* 1.56 Hz */
    } else if (interval_us > 320000) {
        setting = BMI160_DATA_RATE_3_12HZ; /* 3.12 Hz */
    } else if (interval_us > 160000) {
        setting = BMI160_DATA_RATE_6_25HZ; /* 6.25 Hz */
    } else if (interval_us > 80000) {
        setting = BMI160_DATA_RATE_12_5HZ; /* 12.5 Hz */
    } else if (interval_us > 40000) {
        setting = BMI160_DATA_RATE_25HZ;   /*   25 Hz */
    } else if (interval_us > 20000) {
        setting = BMI160_DATA_RATE_50HZ;   /*   50 Hz */
    } else if (interval_us > 10000) {
        setting = BMI160_DATA_RATE_100HZ;  /*  100 Hz */
    } else if (interval_us > 5000) {
        setting = BMI160_DATA_RATE_200HZ;  /*  200 Hz */
    } else if (interval_us > 2500) {
        setting = BMI160_DATA_RATE_400HZ;  /*  400 Hz */
    } else if (interval_us > 1250) {
        setting = BMI160_DATA_RATE_800HZ;  /*  800 Hz */
    } else if (interval_us > 625) {
        setting = BMI160_DATA_RATE_1600HZ; /* 1600 Hz */
    } else {
        setting = BMI160_DATA_RATE_3200HZ; /* 3200 Hz  or higher */
    }

    return setting;
}

static int16_t check_err_reg(int16_t default_return)
{
    uint8_t i2cData;
    int16_t fret;

    /* read status register */
    fret = AKH_RxData(AKM_ST_ACC, BMI160_REG_ERR_REG, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    if (i2cData != 0) {
        return AKM_ERROR;
    }

    /* no error */
    return default_return;
}

static int32_t acc_sensitivity_from_range(uint8_t range)
{
    /* calculate sensitivity in Q16 format */
    switch (range) {
    case BMI160_VAL_ACC_FS2G:
        return (int32_t)(ACC_1G_IN_Q16 / 16384);

    case BMI160_VAL_ACC_FS4G:
        return (int32_t)(ACC_1G_IN_Q16 / 8192);

    case BMI160_VAL_ACC_FS8G:
        return (int32_t)(ACC_1G_IN_Q16 / 4096);

    case BMI160_VAL_ACC_FS16G:
        return (int32_t)(ACC_1G_IN_Q16 / 2048);

    default:
        return AKM_ERR_INVALID_ARG;
    }
}

static int32_t gyr_sensitivity_from_range(uint8_t range)
{
    /* calculate sensitivity in Q16 format */

    /* datasheet section 1.2 */
    switch (range) {
    case BMI160_VAL_GYR_FS125:
        /* 262.4 deg/sec in Q16 format */
        return (int32_t)(65536 / 262.4);

    case BMI160_VAL_GYR_FS250:
        /* 131.2 deg/sec in Q16 format */
        return (int32_t)(65536 / 131.2);

    case BMI160_VAL_GYR_FS500:
        /* 65.6 deg/sec in Q16 format */
        return (int32_t)(65536 / 65.6);

    case BMI160_VAL_GYR_FS1000:
        /* 32.8 deg/sec in Q16 format */
        return (int32_t)(65536 / 32.8);

    case BMI160_VAL_GYR_FS2000:
        /* 16.4 deg/sec in Q16 format */
        return (int32_t)(65536 / 16.4);

    default:
        return AKM_ERR_INVALID_ARG;
    }
}

/******************************************************************************/
/***** AKS public APIs ********************************************************/
int16_t bmi160_acc_config(
    AKM_DEVICES          *acc_dev,
    struct aks_interface **acc_if)
{
    uint8_t i2cData;
    int16_t fret;

    /* read status register */
    fret = AKH_RxData(AKM_ST_ACC, BMI160_REG_CHIPID, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    if (i2cData != BMI160_VAL_CHIPID) {
        return AKM_ERR_NOT_SUPPORT;
    }

    g_acc_device = AKM_ACCELEROMETER_BMI160;
    g_gyr_device = AKM_GYROSCOPE_BMI160;

    *acc_dev = g_acc_device;
    *acc_if = &bmi160_acc_interface;
    return AKM_SUCCESS;
}

int16_t bmi160_gyr_config(
    AKM_DEVICES          *gyr_dev,
    struct aks_interface **gyr_if)
{
    uint8_t i2cData;
    int16_t fret;

    /* read status register */
    fret = AKH_RxData(AKM_ST_ACC, BMI160_REG_CHIPID, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    if (i2cData != BMI160_VAL_CHIPID) {
        return AKM_ERR_NOT_SUPPORT;
    }

    g_acc_device = AKM_ACCELEROMETER_BMI160;
    g_gyr_device = AKM_GYROSCOPE_BMI160;

    *gyr_dev = g_gyr_device;
    *gyr_if = &bmi160_gyr_interface;
    return AKM_SUCCESS;
}

int16_t bmi160_init(
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3])
{
    uint8_t i2cData;
    int16_t fret;

    /* Reset BMI160*/
    /* After reset, device should be suspend mode. */
    i2cData = BMI160_CMD_SOFTRESET;
    fret = AKH_TxData(AKM_ST_ACC, BMI160_REG_CMD, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    AKH_DelayMilli(100);

    /* Set Acc range */
    i2cData = BMI160_ACC_RANGE;
    fret = AKH_TxData(AKM_ST_ACC, BMI160_REG_ACC_RANGE, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* delay is required (datasheet section 2.2.1) */
    AKH_DelayMicro(400);
    /* set sensitivity in global memory */
    g_acc_sensitivity = acc_sensitivity_from_range(BMI160_ACC_RANGE);

    /* Set Gyr range */
    i2cData = BMI160_GYR_RANGE;
    fret = AKH_TxData(AKM_ST_ACC, BMI160_REG_GYR_RANGE, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* delay is required (ditto) */
    AKH_DelayMicro(400);
    /* set sensitivity in global memory */
    g_gyr_sensitivity = gyr_sensitivity_from_range(BMI160_GYR_RANGE);

    /* axis conversion parameter */
    g_axis_order[0] = axis_order[0];
    g_axis_order[1] = axis_order[1];
    g_axis_order[2] = axis_order[2];
    g_axis_sign[0] = axis_sign[0];
    g_axis_sign[1] = axis_sign[1];
    g_axis_sign[2] = axis_sign[2];

    return RETURN_CHECK(AKM_SUCCESS);
}

int16_t bmi160_get_info(struct AKS_DEVICE_INFO *info)
{
    if ((g_acc_device != AKM_ACCELEROMETER_BMI160) ||
        (g_gyr_device != AKM_GYROSCOPE_BMI160)) {
        return AKM_ERR_NOT_SUPPORT;
    }

    AKS_MyStrcpy(info->name, "BMI160", AKS_INFO_NAME_SIZE);
    info->device = g_acc_device;
    return AKM_SUCCESS;
}

int16_t bmi160_acc_start(const int32_t interval_us)
{
    uint8_t i2cData;
    int16_t fret;
    uint8_t setting;

    setting = bmi160_interval_us_to_odr(interval_us);

    /* don't use undersampling mode, currently. */
    if ((setting > BMI160_DATA_RATE_1600HZ) ||
        (BMI160_DATA_RATE_12_5HZ > setting)) {
        return AKM_ERR_INVALID_ARG;
    }

    /* accelerometer configuration */
    i2cData = (BMI160_ACC_BWP << 4) | setting;
    fret = AKH_TxData(AKM_ST_ACC, BMI160_REG_ACC_CONF, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    AKH_DelayMicro(400); /* delay is required (ditto) */

    /* set to normal mode */
    i2cData = BMI160_CMD_ACC_NORMAL;
    fret = AKH_TxData(AKM_ST_ACC, BMI160_REG_CMD, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* datasheet section 2.11.38 */
    AKH_DelayMicro(3800);

    return RETURN_CHECK(AKM_SUCCESS);
}

int16_t bmi160_gyr_start(const int32_t interval_us)
{
    uint8_t i2cData;
    int16_t fret;
    uint8_t setting;

    setting = bmi160_interval_us_to_odr(interval_us);

    if ((setting > BMI160_DATA_RATE_3200HZ) ||
        (BMI160_DATA_RATE_25HZ > setting)) {
        return AKM_ERR_INVALID_ARG;
    }

    /* gyroscope configuration */
    i2cData = (BMI160_GYR_BWP << 4) | setting;
    fret = AKH_TxData(AKM_ST_GYR, BMI160_REG_GYR_CONF, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    AKH_DelayMicro(400); /* delay is required (ditto) */

    /* set to normal mode */
    i2cData = BMI160_CMD_GYR_NORMAL;
    fret = AKH_TxData(AKM_ST_GYR, BMI160_REG_CMD, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* datasheet section 2.11.38 */
    AKH_DelayMilli(80);

    return RETURN_CHECK(AKM_SUCCESS);
}

int16_t bmi160_acc_stop(void)
{
    uint8_t i2cData;
    int16_t fret;

    /* set to suspend mode */
    i2cData = BMI160_CMD_ACC_SUSPEND;
    fret = AKH_TxData(AKM_ST_ACC, BMI160_REG_CMD, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    AKH_DelayMicro(400); /* delay is required ? */

    return RETURN_CHECK(AKM_SUCCESS);
}

int16_t bmi160_gyr_stop(void)
{
    uint8_t i2cData;
    int16_t fret;

    /* set to suspend mode */
    i2cData = BMI160_CMD_GYR_SUSPEND;
    fret = AKH_TxData(AKM_ST_GYR, BMI160_REG_CMD, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    AKH_DelayMicro(400); /* delay is required ? */

    return RETURN_CHECK(AKM_SUCCESS);
}

int16_t bmi160_acc_check_rdy(const int32_t timeout_us)
{
    uint8_t i2cData;
    int16_t fret;

    /* read status register */
    fret = AKH_RxData(AKM_ST_ACC, BMI160_REG_STATUS, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    return ((0x80 & i2cData) != 0);
}

int16_t bmi160_gyr_check_rdy(const int32_t timeout_us)
{
    uint8_t i2cData;
    int16_t fret;

    /* read status register */
    fret = AKH_RxData(AKM_ST_GYR, BMI160_REG_STATUS, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    return ((0x40 & i2cData) != 0);
}

int16_t bmi160_acc_get_data(
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num)
{
    uint8_t i2cData[6];
    int16_t tmp;
    int16_t fret;
    uint8_t i;

    /* check arg */
    if (*num < 1) {
        return AKM_ERR_INVALID_ARG;
    }

    /* read data */
    fret = AKH_RxData(AKM_ST_ACC, BMI160_REG_ACC_DATA, i2cData, 6);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    for (i = 0; i < 3; i++) {
        /* convert to int16 data */
        tmp = (int16_t)(((uint16_t)i2cData[i * 2 + 1] << 8)
                        | i2cData[i * 2]);
        data->u.v[i] = (int32_t)tmp * g_acc_sensitivity;
    }

    AKS_ConvertCoordinate(data->u.v, g_axis_order, g_axis_sign);

    data->stype = AKM_ST_ACC;
    data->timestamp = AKH_GetTimestamp();
    *num = 1;
    return AKM_SUCCESS;
}

int16_t bmi160_gyr_get_data(
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num)
{
    uint8_t i2cData[6];
    int16_t tmp;
    int16_t fret;
    uint8_t i;

    /* check arg */
    if (*num < 1) {
        return AKM_ERR_INVALID_ARG;
    }

    /* read data */
    fret = AKH_RxData(AKM_ST_GYR, BMI160_REG_GYR_DATA, i2cData, 6);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    for (i = 0; i < 3; i++) {
        /* convert to int16 data */
        tmp = (int16_t)(((uint16_t)i2cData[i * 2 + 1] << 8)
                        | i2cData[i * 2]);
        data->u.v[i] = (int32_t)tmp * g_gyr_sensitivity;
    }

    AKS_ConvertCoordinate(data->u.v, g_axis_order, g_axis_sign);

    data->stype = AKM_ST_GYR;
    data->timestamp = AKH_GetTimestamp();
    *num = 1;
    return AKM_SUCCESS;
}

int16_t bmi160_acc_self_test(int32_t *result)
{
    return AKM_SUCCESS;
}

int16_t bmi160_gyr_self_test(int32_t *result)
{
    return AKM_SUCCESS;
}
