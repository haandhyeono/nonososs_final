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
#include "AKS_APIs.h"
#include "AKH_APIs.h" // Ensure this is included

#include "aks_acc_adxl34x.h"
#include "aks_acc_gyr_bmi160.h"
#include "aks_gyr_l3g4200d.h"
#include "aks_mag_ak0994x.h"
#include "aks_mag_ak099xx.h"
#include "aks_mag_ak8963.h"
#include <stdint.h>

#define MEASUREMENT_SIZE 10 // 평균을 낼 데이터 개수

// 평균 계산을 위한 변수
static int32_t sum_x = 0;
static int32_t sum_y = 0;
static int32_t sum_z = 0;
static uint8_t measurement_index = 0; // 평균을 계산할 데이터 개수



#define NUMBER_OF_SLOT  3

struct aks_sensor_slot {
    AKM_SENSOR_TYPE      type;
    AKM_DEVICES          dev;
    struct aks_interface *interface;
};

static int16_t no_device_init(
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3])
{
    return AKM_ERR_NOT_SUPPORT;
}

static int16_t no_device_get_info(struct AKS_DEVICE_INFO *info)
{
    return AKM_ERR_NOT_SUPPORT;
}

static int16_t no_device_start(const int32_t interval_us)
{
    return AKM_ERR_NOT_SUPPORT;
}

static int16_t no_device_stop(void)
{
    return AKM_ERR_NOT_SUPPORT;
}

static int16_t no_device_check_rdy(const int32_t timeout_us)
{
    return AKM_ERR_NOT_SUPPORT;
}

static int16_t no_device_get_data(
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num)
{
    return AKM_ERR_NOT_SUPPORT;
}

static int16_t no_device_self_test(int32_t *result)
{
    return AKM_ERR_NOT_SUPPORT;
}

/* empty sensor interface. this is used as initial value. */
static struct aks_interface no_device_interface = {
    .aks_init = no_device_init,
    .aks_get_info = no_device_get_info,
    .aks_start = no_device_start,
    .aks_stop = no_device_stop,
    .aks_check_rdy = no_device_check_rdy,
    .aks_get_data = no_device_get_data,
    .aks_self_test = no_device_self_test
};

/* sensor slots */
static struct aks_sensor_slot g_slots[NUMBER_OF_SLOT];
static uint8_t                g_num_of_device = 0;

static void aks_slot_init(struct aks_sensor_slot *slots)
{
    uint8_t i;

    for (i = 0; i < NUMBER_OF_SLOT; i++) {
        slots->type = AKM_ST_NONE;
        slots->dev = AKM_DEVICE_NONE;
        slots->interface = &no_device_interface;
        slots++;
    }

    g_num_of_device = 0;
}

int16_t AKS_Config(
    const uint8_t     num,
    const AKM_DEVICES dev[])
{
    uint8_t                i;
    uint8_t                slot_id;
    int16_t                ret;
    struct aks_sensor_slot *slot;

    AKH_Print("AKS_Config: Start\n");
    /* initialize slots */
    aks_slot_init(g_slots);

    slot_id = 0;

    for (i = 0; i < num; i++) {
        slot = &g_slots[slot_id];

        switch (dev[i]) {
        case AKM_MAGNETOMETER_AK8963:
            AKH_Print("AKS_Config: Configuring AKM_MAGNETOMETER_AK8963\n");
            ret = ak8963_config(&slot->dev, &slot->interface);

            if (ret == AKM_SUCCESS) {
                slot->type = AKM_ST_MAG;
                g_num_of_device = ++slot_id;
            }

            break;

        case AKM_MAGNETOMETER_AK09911:
        case AKM_MAGNETOMETER_AK09912:
        case AKM_MAGNETOMETER_AK09913:
        case AKM_MAGNETOMETER_AK09915:
        case AKM_MAGNETOMETER_AK09916C:
        case AKM_MAGNETOMETER_AK09916D:
        case AKM_MAGNETOMETER_AK09917D:
        case AKM_MAGNETOMETER_AK09918:
        case AKM_MAGNETOMETER_AK09919:
            AKH_Print("AKS_Config: Configuring AKM_MAGNETOMETER_AK099xx\n");
            ret = ak099xx_config(&slot->dev, &slot->interface);

            if (ret == AKM_SUCCESS) {
                slot->type = AKM_ST_MAG;
                g_num_of_device = ++slot_id;
            }

            break;

        case AKM_MAGNETOMETER_AK09940A:
            AKH_Print("AKS_Config: Configuring AKM_MAGNETOMETER_AK09940A\n");
            ret = ak0994x_config(&slot->dev, &slot->interface);

            if (ret == AKM_SUCCESS) {
                slot->type = AKM_ST_MAG;
                g_num_of_device = ++slot_id;
            }

            break;

        case AKM_ACCELEROMETER_ADXL345:
        case AKM_ACCELEROMETER_ADXL346:
            AKH_Print("AKS_Config: Configuring AKM_ACCELEROMETER_ADXL34x\n");
            ret = adxl34x_config(&slot->dev, &slot->interface);

            if (ret == AKM_SUCCESS) {
                slot->type = AKM_ST_ACC;
                g_num_of_device = ++slot_id;
            }

            break;

        case AKM_ACCELEROMETER_BMI160:
            AKH_Print("AKS_Config: Configuring AKM_ACCELEROMETER_BMI160\n");
            ret = bmi160_acc_config(&slot->dev, &slot->interface);

            if (ret == AKM_SUCCESS) {
                slot->type = AKM_ST_ACC;
                g_num_of_device = ++slot_id;
            }

            break;

        case AKM_GYROSCOPE_L3G4200D:
        case AKM_GYROSCOPE_L3GD20:
            AKH_Print("AKS_Config: Configuring AKM_GYROSCOPE_L3G4xx\n");
            ret = l3g4200d_config(&slot->dev, &slot->interface);

            if (ret == AKM_SUCCESS) {
                slot->type = AKM_ST_GYR;
                g_num_of_device = ++slot_id;
            }

            break;

        case AKM_GYROSCOPE_BMI160:
            AKH_Print("AKS_Config: Configuring AKM_GYROSCOPE_BMI160\n");
            ret = bmi160_gyr_config(&slot->dev, &slot->interface);

            if (ret == AKM_SUCCESS) {
                slot->type = AKM_ST_GYR;
                g_num_of_device = ++slot_id;
            }

            break;

        case AKM_DEVICE_NONE:
            /* It's fine, go next. */
            AKH_Print("AKS_Config: No device configured in this slot\n");
            continue;

        case AKM_DEVICE_TEST:
            /* put some test sensor here. */
            AKH_Print("AKS_Config: Configuring AKM_DEVICE_TEST\n");
            break;

        default:
            /* unknown parameter */
            AKH_Print("AKS_Config: Unknown device parameter\n");
            return AKM_ERR_INVALID_ARG;
        }

        /* if slot is already full, exit */
        if (slot_id > NUMBER_OF_SLOT) {
            break;
        }
    }

    AKH_Print("AKS_Config: End with %d devices configured\n", g_num_of_device);
    return g_num_of_device;
}

/******************************************************************************/
/***** AKS debug APIs ********************************************************/
//int16_t AKS_Get_Error();

/******************************************************************************/
/***** AKS public APIs ********************************************************/
int16_t AKS_Init(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         axis_order[3],
    const uint8_t         axis_sign[3])
{
    uint8_t                id;
    int16_t                ret;
    struct aks_sensor_slot *slot = g_slots;

    for (id = 0; id < g_num_of_device; id++) {
        if (stype & slot->type) {
            AKH_Print("AKS_Init: Initializing device %d of type %d\n", id, slot->type);
            ret = slot->interface->aks_init(axis_order, axis_sign);

            if ((ret != AKM_SUCCESS) && (ret != AKM_ERR_NOT_SUPPORT)) {
                AKH_Print("AKS_Init: Initialization failed for device %d\n", id);
                return ret;
            }
        }

        slot++;
    }

    AKH_Print("AKS_Init: Initialization successful\n");
    return AKM_SUCCESS;
}

int16_t AKS_GetDeviceInfo(
    const AKM_SENSOR_TYPE  stype,
    struct AKS_DEVICE_INFO *info,
    uint8_t                *num)
{
    uint8_t                id;
    struct aks_sensor_slot *slot;
    struct AKS_DEVICE_INFO *head;
    uint8_t                num_of_remain;
    int16_t                ret = AKM_ERR_NOT_SUPPORT;

    /* the number of remaining buffer */
    slot = g_slots;
    head = info;
    num_of_remain = *num;

    for (id = 0; id < g_num_of_device; id++) {
        if (stype & slot->type) {
            AKH_Print("AKS_GetDeviceInfo: Getting info for device %d of type %d\n", id, slot->type);
            ret = slot->interface->aks_get_info(head);

            if (ret == AKM_SUCCESS) {
                /* it's fine. goto next device */
                head++;
                num_of_remain--;
            } else if (ret == AKM_ERR_NOT_SUPPORT) {
                /* ignore this device */
                ;
            } else {
                /* woops! */
                AKH_Print("AKS_GetDeviceInfo: Failed to get info for device %d\n", id);
                return ret;
            }

            if (num_of_remain == 0) {
                break;
            }
        }

        slot++;
    }

    /* update num */
    *num -= num_of_remain;

    AKH_Print("AKS_GetDeviceInfo: End\n");
    return AKM_SUCCESS;
}

int16_t AKS_Start(
    const AKM_SENSOR_TYPE stype,
    const int32_t         interval_us)
{
    uint8_t                id;
    int16_t                ret;
    struct aks_sensor_slot *slot = g_slots;

    for (id = 0; id < g_num_of_device; id++) {
        if (stype & slot->type) {
            AKH_Print("AKS_Start: Starting device %d of type %d\n", id, slot->type);
            ret = slot->interface->aks_start(interval_us);

            if ((ret != AKM_SUCCESS) && (ret != AKM_ERR_NOT_SUPPORT)) {
                AKH_Print("AKS_Start: Failed to start device %d\n", id);
                return ret;
            }
        }

        slot++;
    }

    AKH_Print("AKS_Start: All devices started successfully\n");
    return AKM_SUCCESS;
}

int16_t AKS_Stop(const AKM_SENSOR_TYPE stype)
{
    uint8_t                id;
    int16_t                ret;
    struct aks_sensor_slot *slot = g_slots;

    for (id = 0; id < g_num_of_device; id++) {
        if (stype & slot->type) {
            AKH_Print("AKS_Stop: Stopping device %d of type %d\n", id, slot->type);
            ret = slot->interface->aks_stop();

            if ((ret != AKM_SUCCESS) && (ret != AKM_ERR_NOT_SUPPORT)) {
                AKH_Print("AKS_Stop: Failed to stop device %d\n", id);
                return ret;
            }
        }

        slot++;
    }

    AKH_Print("AKS_Stop: All devices stopped successfully\n");
    return AKM_SUCCESS;
}

int16_t AKS_CheckDataReady(
    const AKM_SENSOR_TYPE stype,
    const int32_t         timeout_us)
{
    uint8_t                id;
    struct aks_sensor_slot *slot = g_slots;

    /* this API does not support multi-device */
    if (stype == AKM_ST_ALL_SENSORS) {
        AKH_Print("AKS_CheckDataReady: Invalid argument for multi-device check\n");
        return AKM_ERR_INVALID_ARG;
    }

    for (id = 0; id < g_num_of_device; id++) {
        if (stype & slot->type) {
            AKH_Print("AKS_CheckDataReady: Checking data ready for device %d of type %d\n", id, slot->type);
            return slot->interface->aks_check_rdy(timeout_us);
        }

        slot++;
    }

    AKH_Print("AKS_CheckDataReady: Data ready check not supported\n");
    return AKM_ERR_NOT_SUPPORT;
}




int16_t AKS_GetData(
    const AKM_SENSOR_TYPE  stype,
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num)
{
    uint8_t                id;
    struct aks_sensor_slot *slot;
    struct AKM_SENSOR_DATA *head;
    uint8_t                num_of_remain;
    uint8_t                tmp_num;
    int16_t                ret;

    // the number of remaining buffer
    slot = g_slots;
    head = data;
    num_of_remain = *num;

    for (id = 0; id < g_num_of_device; id++) {
        tmp_num = num_of_remain;

        if (stype & slot->type) {
            ret = slot->interface->aks_get_data(head, &tmp_num);

            if (ret == AKM_SUCCESS) {
                // 디버그 출력 추가 및 평균 계산
                for (uint8_t i = 0; i < tmp_num; i++) {
               
                    // 평균을 계산할 데이터 합계에 추가
                    sum_x += head[i].u.v[0];
                    sum_y += head[i].u.v[1];
                    sum_z += head[i].u.v[2];
                    measurement_index++;

                    // 10개의 데이터가 수집되면 평균을 계산하고 출력
                    if (measurement_index >= MEASUREMENT_SIZE) {
                        
                        int32_t avg_x = sum_x / MEASUREMENT_SIZE;
                        int32_t avg_y = sum_y / MEASUREMENT_SIZE;
                        int32_t avg_z = sum_z / MEASUREMENT_SIZE;

                        AKH_Print("Retrived data: x=%d, y=%d, z=%d\n", -avg_x, -avg_y, -avg_z);

                        // 변수 초기화
                        sum_x = 0;
                        sum_y = 0;
                        sum_z = 0;
                        measurement_index = 0; // 카운터 초기화
                    }
                }

                // it's fine. goto next device
                head += tmp_num;
                num_of_remain -= tmp_num;
            } else if (ret == AKM_ERR_NOT_SUPPORT) {
                // ignore this device
                AKH_Print("AKS_GetData: Data retrieval not supported for device %d\n", id);
            } else {
                AKH_Print("AKS_GetData: Failed to get data for device %d\n", id);
                return ret;
            }

            if (num_of_remain == 0) {
                break;
            }
        }

        slot++;
    }

    // update num
    *num -= num_of_remain;

    return AKM_SUCCESS;
}



int16_t AKS_SelfTest(
    const AKM_SENSOR_TYPE stype,
    int32_t               *result)
{
    uint8_t                id;
    struct aks_sensor_slot *slot = g_slots;

    /* this API does not support multi-device */
    if (stype == AKM_ST_ALL_SENSORS) {
        AKH_Print("AKS_SelfTest: Invalid argument for multi-device self-test\n");
        return AKM_ERR_INVALID_ARG;
    }

    for (id = 0; id < g_num_of_device; id++) {
        if (stype & slot->type) {
            AKH_Print("AKS_SelfTest: Performing self-test for device %d of type %d\n", id, slot->type);
            return slot->interface->aks_self_test(result);
        }

        slot++;
    }

    AKH_Print("AKS_SelfTest: Self-test not supported\n");
    return AKM_ERR_NOT_SUPPORT;
}
