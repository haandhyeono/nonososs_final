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
#include "aks_acc_adxl34x.h"
#include "aks_common.h"
#include "AKH_APIs.h"

#define ADXL34X_REG_DEVID            0x00
#define ADXL34X_REG_BW_RATE          0x2C
#define ADXL34X_REG_POWER_CTL        0x2D
#define ADXL34X_REG_INT_ENABLE       0x2E
#define ADXL34X_REG_INT_SOURCE       0x30
#define ADXL34X_REG_DATA_FORMAT      0x31
#define ADXL34X_REG_DATAX0           0x32
#define ADXL34X_REG_DATAX1           0x33
#define ADXL34X_REG_DATAY0           0x34
#define ADXL34X_REG_DATAY1           0x35
#define ADXL34X_REG_DATAZ0           0x36
#define ADXL34X_REG_DATAZ1           0x37

#define ADXL34X_VAL_DEVID_ADXL345    (0xE5)
#define ADXL34X_VAL_DEVID_ADXL346    (0xE6)
#define ADXL34X_VAL_PCTL_MEASURE     (1 << 3)
#define ADXL34X_VAL_DATA_RATE_200HZ  (0x0B)
#define ADXL34X_VAL_RANGE_2G         (0x00)
#define ADXL34X_VAL_RESOLUTION_2G    (256)
#define ADXL34X_VAL_RANGE_4G         (0x01)
#define ADXL34X_VAL_RESOLUTION_4G    (128)
#define ADXL34X_VAL_INT_ENABLED      (0x80) /* DATA_READY is enabled */

#define ADXL34X_VAL_RANGE            ADXL34X_VAL_RANGE_4G
#define ADXL34X_VAL_RESOLUTION       ADXL34X_VAL_RESOLUTION_4G

static AKM_DEVICES   g_device = AKM_DEVICE_NONE;
static uint8_t       g_acc_axis_order[3];
static uint8_t       g_acc_axis_sign[3];
static AKM_TIMESTAMP g_acc_ts;

static struct aks_interface adxl34x_interface = {
    .aks_init = adxl34x_init,
    .aks_get_info = adxl34x_get_info,
    .aks_start = adxl34x_start,
    .aks_stop = adxl34x_stop,
    .aks_check_rdy = adxl34x_check_rdy,
    .aks_get_data = adxl34x_get_data,
    .aks_self_test = adxl34x_self_test
};

void acc_irq_handler(void)
{
    g_acc_ts = AKH_GetTimestamp();
}

/******************************************************************************/
/***** AKS public APIs ********************************************************/
int16_t adxl34x_config(
    AKM_DEVICES          *acc_dev,
    struct aks_interface **acc_if)
{
    uint8_t devid;
    int16_t fret;

    /* Read WIA */
    fret = AKH_RxData(AKM_ST_ACC, ADXL34X_REG_DEVID, &devid, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    switch (devid) {
    case ADXL34X_VAL_DEVID_ADXL345:
        g_device = AKM_ACCELEROMETER_ADXL345;
        break;

    case ADXL34X_VAL_DEVID_ADXL346:
        g_device = AKM_ACCELEROMETER_ADXL346;
        break;

    default:
        return AKM_ERR_NOT_SUPPORT;
    }

    *acc_dev = g_device;
    *acc_if = &adxl34x_interface;
    return AKM_SUCCESS;
}

int16_t adxl34x_init(
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3])
{
    uint8_t i2cData;
    int16_t fret;

    /* read 'bw rate' register */
    fret = AKH_RxData(AKM_ST_ACC, ADXL34X_REG_BW_RATE, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* set bw rate */
    i2cData = ((i2cData & 0xF0) | ADXL34X_VAL_DATA_RATE_200HZ);
    fret = AKH_TxData(AKM_ST_ACC, ADXL34X_REG_BW_RATE, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* set INT pin enable */
    i2cData = ADXL34X_VAL_INT_ENABLED;
    fret = AKH_TxData(AKM_ST_ACC, ADXL34X_REG_INT_ENABLE, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* read 'range' register */
    fret = AKH_RxData(AKM_ST_ACC, ADXL34X_REG_DATA_FORMAT, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* set range */
    i2cData = ((i2cData & 0xFC) | ADXL34X_VAL_RANGE);
    fret = AKH_TxData(AKM_ST_ACC, ADXL34X_REG_DATA_FORMAT, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    fret = AKH_SetIRQHandler(IRQ_ACC_1, acc_irq_handler);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* set to power down mode. */
    i2cData = 0;
    fret = AKH_TxData(AKM_ST_ACC, ADXL34X_REG_POWER_CTL, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* axis conversion parameter */
    g_acc_axis_order[0] = axis_order[0];
    g_acc_axis_order[1] = axis_order[1];
    g_acc_axis_order[2] = axis_order[2];
    g_acc_axis_sign[0] = axis_sign[0];
    g_acc_axis_sign[1] = axis_sign[1];
    g_acc_axis_sign[2] = axis_sign[2];
    return AKM_SUCCESS;
}

int16_t adxl34x_get_info(struct AKS_DEVICE_INFO *info)
{
    switch (g_device) {
    case AKM_ACCELEROMETER_ADXL345:
        AKS_MyStrcpy(info->name, "ADXL345", AKS_INFO_NAME_SIZE);
        break;

    case AKM_ACCELEROMETER_ADXL346:
        AKS_MyStrcpy(info->name, "ADXL346", AKS_INFO_NAME_SIZE);
        break;

    default:
        return AKM_ERR_NOT_SUPPORT;
    }

    info->device = g_device;
    return AKM_SUCCESS;
}

int16_t adxl34x_start(const int32_t interval_us)
{
    uint8_t i2cData;
    int16_t fret;

    /* Read power control register */
    fret = AKH_RxData(AKM_ST_ACC, ADXL34X_REG_POWER_CTL, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* set measure bit to 1 */
    i2cData |= ADXL34X_VAL_PCTL_MEASURE;
    fret = AKH_TxData(AKM_ST_ACC, ADXL34X_REG_POWER_CTL, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    return AKM_SUCCESS;
}

int16_t adxl34x_stop(void)
{
    uint8_t i2cData;
    int16_t fret;

    /* Read power control register */
    fret = AKH_RxData(AKM_ST_ACC, ADXL34X_REG_POWER_CTL, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* set measure bit to 0 */
    i2cData &= ~ADXL34X_VAL_PCTL_MEASURE;
    fret = AKH_TxData(AKM_ST_ACC, ADXL34X_REG_POWER_CTL, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    return AKM_SUCCESS;
}

int16_t adxl34x_check_rdy(const int32_t timeout_us)
{
    uint8_t i2cData;
    int16_t fret;

    /* Read power control register */
    fret = AKH_RxData(AKM_ST_ACC, ADXL34X_REG_INT_SOURCE, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* FIFO is not supported yet. */
    return ((0x80 & i2cData) != 0);
}

int16_t adxl34x_get_data(
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

    /* Read data */
#ifdef AKH_USE_SPI
    /* set MB bit for multiple read operation */
    /* this bit is required only SPI mode */
    fret = AKH_RxData(
            AKM_ST_ACC, ADXL34X_REG_DATAX0 | 0x40, i2cData, 6);
#else
    fret = AKH_RxData(
            AKM_ST_ACC, ADXL34X_REG_DATAX0, i2cData, 6);
#endif

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    for (i = 0; i < 3; i++) {
        /* convert to int16 data */
        tmp = (int16_t)(((uint16_t)i2cData[i * 2 + 1] << 8)
                        | i2cData[i * 2]);
        data->u.v[i] = (tmp * ACC_1G_IN_Q16 / ADXL34X_VAL_RESOLUTION);
    }

    AKS_ConvertCoordinate(data->u.v, g_acc_axis_order, g_acc_axis_sign);

    data->stype = AKM_ST_ACC;
    data->timestamp = g_acc_ts;
    *num = 1;
    return AKM_SUCCESS;
}

int16_t adxl34x_self_test(int32_t *result)
{
    return AKM_SUCCESS;
}
