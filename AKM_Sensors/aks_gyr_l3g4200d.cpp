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
#include "aks_common.h"
#include "aks_gyr_l3g4200d.h"
#include "AKH_APIs.h"

/* From Data sheet:
 * the 7 LSb represent the actual register address while the MSb enables
 * address auto-increment. If the MSb of the SUB field is 1, the SUB (register
 * address) will be automatically incremented to allow multiple data
 * read/write.*/
#define L3G4200D_REG_WHO_AM_I     0x0F
#define L3G4200D_REG_CTRL_REG1    0x20
#define L3G4200D_REG_CTRL_REG2    0x21
#define L3G4200D_REG_CTRL_REG3    0x22
#define L3G4200D_REG_CTRL_REG4    0x23
#define L3G4200D_REG_CTRL_REG5    0x24
#define L3G4200D_REG_STATUS_REG   0x27
#define L3G4200D_REG_OUT_X_L_REG  0x28
#define L3G4200D_REG_OUT_X_H_REG  0x29
#define L3G4200D_REG_OUT_Y_L_REG  0x2A
#define L3G4200D_REG_OUT_Y_H_REG  0x2B
#define L3G4200D_REG_OUT_Z_L_REG  0x2C
#define L3G4200D_REG_OUT_Z_H_REG  0x2D

#define WHOAMI_VAL_L3G4200D       0xD3
#define WHOAMI_VAL_L3GD20         0xD4

/* So, add 0x80 when multiple read/write operation is requested.*/
#define L3G4200D_REG_MULTIPLE(reg)  ((reg) + 0x80)

/* Power */
#define L3G4200D_CTRL_REG1_SET_POWERDOWN(val)  ((val) & 0xF7)
#define L3G4200D_CTRL_REG1_SET_NORMAL(val)     ((val) | 0x08)

/* */
#define L3G4200D_SENSITIVITY_2000_Q16  (4588) /* 0.070 in Q16 */
#define L3G4200D_SENSITIVITY_500_Q16   (1147) /* 0.0175 in Q16 */
#define L3G4200D_SENSITIVITY_250_Q16   (574)  /* 0.00875 in Q16 */

static AKM_DEVICES g_device = AKM_DEVICE_NONE;
static uint8_t     g_gyr_axis_order[3];
static uint8_t     g_gyr_axis_sign[3];

static struct aks_interface l3g4200d_interface = {
    .aks_init = l3g4200d_init,
    .aks_get_info = l3g4200d_get_info,
    .aks_start = l3g4200d_start,
    .aks_stop = l3g4200d_stop,
    .aks_check_rdy = l3g4200d_check_rdy,
    .aks_get_data = l3g4200d_get_data,
    .aks_self_test = l3g4200d_self_test
};

/******************************************************************************/
/***** AKS public APIs ********************************************************/
int16_t l3g4200d_config(
    AKM_DEVICES          *gyr_dev,
    struct aks_interface **gyr_if)
{
    uint8_t whoami;
    int16_t fret;

    fret = AKH_RxData(AKM_ST_GYR, L3G4200D_REG_WHO_AM_I, &whoami, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    switch (whoami) {
    case WHOAMI_VAL_L3G4200D:
        g_device = AKM_GYROSCOPE_L3G4200D;
        break;

    case WHOAMI_VAL_L3GD20:
        g_device = AKM_GYROSCOPE_L3GD20;
        break;

    default:
        return AKM_ERR_NOT_SUPPORT;
    }

    *gyr_dev = g_device;
    *gyr_if = &l3g4200d_interface;
    return AKM_SUCCESS;
}

int16_t l3g4200d_init(
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3])
{
    uint8_t i2cData;
    int16_t fret;

    /* Init sequence ignores reset value! */

    /* set ODR and BW, and set to power down mode forcely. */
    /* 1000x111: ODR=380Hz, Cut-Off= 20 */
    /* 1001x111: ODR=380Hz, Cut-Off= 25 */
    /* 1010x111: ODR=380Hz, Cut-Off= 50 */
    /* 1011x111: ODR=380Hz, Cut-Off=100 */
    i2cData = 0x97;
    fret = AKH_TxData(AKM_ST_GYR, L3G4200D_REG_CTRL_REG1, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* Other setting (CTRL_REG4) */
    /* CTRL_REG4[7]  : Block data update. 0 continue update, 1 blocked*/
    /* CTRL_REG4[6]  : Big/Little endian. 0 little, 1 big. AK compass is little.*/
    /* CTRL_REG4[5:4]: Full scale (dps). 00 250, 01 500, 10 2000 11 2000.*/
    /* CTRL_REG4[0]  : SPI serial. 0 4-wire, 1 3-wire.*/
    /* set range */
    i2cData = 0x20;
    fret = AKH_TxData(AKM_ST_GYR, L3G4200D_REG_CTRL_REG4, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    /* axis conversion parameter */
    g_gyr_axis_order[0] = axis_order[0];
    g_gyr_axis_order[1] = axis_order[1];
    g_gyr_axis_order[2] = axis_order[2];
    g_gyr_axis_sign[0] = axis_sign[0];
    g_gyr_axis_sign[1] = axis_sign[1];
    g_gyr_axis_sign[2] = axis_sign[2];
    return AKM_SUCCESS;
}

int16_t l3g4200d_get_info(struct AKS_DEVICE_INFO *info)
{
    switch (g_device) {
    case AKM_GYROSCOPE_L3G4200D:
        AKS_MyStrcpy(info->name, "L3G4200D", AKS_INFO_NAME_SIZE);
        break;

    case AKM_GYROSCOPE_L3GD20:
        AKS_MyStrcpy(info->name, "L3GD20", AKS_INFO_NAME_SIZE);
        break;

    default:
        return AKM_ERR_NOT_SUPPORT;
    }

    info->device = g_device;
    return AKM_SUCCESS;
}

int16_t l3g4200d_start(const int32_t interval_us)
{
    uint8_t i2cData;
    int16_t fret;

    /* Read power control register */
    fret = AKH_RxData(AKM_ST_GYR, L3G4200D_REG_CTRL_REG1, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    i2cData = L3G4200D_CTRL_REG1_SET_NORMAL(i2cData);
    fret = AKH_TxData(AKM_ST_GYR, L3G4200D_REG_CTRL_REG1, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    return AKM_SUCCESS;
}

int16_t l3g4200d_stop(void)
{
    uint8_t i2cData;
    int16_t fret;

    /* Read power control register */
    fret = AKH_RxData(AKM_ST_GYR, L3G4200D_REG_CTRL_REG1, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    i2cData = L3G4200D_CTRL_REG1_SET_POWERDOWN(i2cData);
    fret = AKH_TxData(AKM_ST_GYR, L3G4200D_REG_CTRL_REG1, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    return AKM_SUCCESS;
}

int16_t l3g4200d_check_rdy(const int32_t timeout_us)
{
    uint8_t i2cData;
    int16_t fret;

    /* Read power status register */
    fret = AKH_RxData(AKM_ST_GYR, L3G4200D_REG_STATUS_REG, &i2cData, 1);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    return ((0x80 & i2cData) != 0);
}

int16_t l3g4200d_get_data(
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
    fret = AKH_RxData(
            AKM_ST_GYR,
            L3G4200D_REG_MULTIPLE(L3G4200D_REG_OUT_X_L_REG),
            i2cData,
            6);

    if (fret != AKM_SUCCESS) {
        return fret;
    }

    for (i = 0; i < 3; i++) {
        /* convert to int16 data */
        tmp = (int16_t)(((uint16_t)i2cData[i * 2 + 1] << 8)
                        | i2cData[i * 2]);
        data->u.v[i] = (tmp * L3G4200D_SENSITIVITY_2000_Q16);
    }

    AKS_ConvertCoordinate(data->u.v, g_gyr_axis_order, g_gyr_axis_sign);

    data->stype = AKM_ST_GYR;
    data->timestamp = AKH_GetTimestamp();
    *num = 1;
    return AKM_SUCCESS;
}

int16_t l3g4200d_self_test(int32_t *result)
{
    return AKM_ERR_NOT_SUPPORT;
}
