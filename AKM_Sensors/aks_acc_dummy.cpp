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
#include <stdlib.h>
#include <string.h>
// #include <unistd.h>

#include "aks_acc_dummy.h"
#include "aks_common.h"
#include "AKH_APIs.h"

#define ACC_NOISE_RANGE_G         (0.1)
#define ACC_DUMMY_INIT_FAIL       0
#define ACC_DUMMY_INFO_FAIL       0
#define ACC_DUMMY_START_FAIL      0
#define ACC_DUMMY_STOP_FAIL       0
#define ACC_DUMMY_CHECK_RDY_FAIL  0
#define ACC_DUMMY_GET_DATA_FAIL   0

/******************************************************************************/
/***** AKS static functions ***************************************************/
int32_t make_noise(void)
{
    long r = rand();
    /* normalize to 1G */
    r = r % ACC_1G_IN_Q16;

    return (int32_t)((r * ACC_NOISE_RANGE_G)
                     - (ACC_1G_IN_Q16 * ACC_NOISE_RANGE_G / 2));
}

/******************************************************************************/
/***** AKS public APIs ********************************************************/
int16_t acc_dummy_init(
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3])
{
#if ACC_DUMMY_INIT_FAIL
    return AKM_ERR_IO;
#else
    return AKM_SUCCESS;
#endif
}

int16_t acc_dummy_get_info(struct AKS_DEVICE_INFO *info)
{
#if ACC_DUMMY_INFO_FAIL
#else
    strncpy(info->name, "Dummy acc", AKS_INFO_NAME_SIZE);
    info->device = AKM_DEVICE_TEST;
    info->parameter[0] = 0xde;
    info->parameter[1] = 0xad;
    info->parameter[2] = 0xbe;
    info->parameter[3] = 0xef;

    return AKM_SUCCESS;
#endif
}

int16_t acc_dummy_start(const int32_t interval_us)
{
#if ACC_DUMMY_START_FAIL
    return AKM_ERR_IO;
#else
    return AKM_SUCCESS;
#endif
}

int16_t acc_dummy_stop(void)
{
#if ACC_DUMMY_STOP_FAIL
    return AKM_ERR_IO;
#else
    return AKM_SUCCESS;
#endif
}

int16_t acc_dummy_check_rdy(const int32_t timeout_us)
{
#if ACC_DUMMY_CHECK_RDY_FAIL
    AKH_DelayMicro(timeout_us);
    return AKM_ERR_TIMEOUT;
#else
    /* return the number of available data */
    return 1;
#endif
}

int16_t acc_dummy_get_data(
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num)
{
    if (*num < 1) {
        return AKM_ERR_INVALID_ARG;
    }

#if ACC_DUMMY_GET_DATA_FAIL
    return AKM_ERR_IO;
#else
    data->stype = AKM_ST_ACC;
    data->u.s.x = 0 + make_noise();
    data->u.s.y = 0 + make_noise();
    data->u.s.z = ACC_1G_IN_Q16 + make_noise();
    data->timestamp = AKH_GetTimestamp();
    data->status[0] = 1;
    *num = 1;
    return AKM_SUCCESS;
#endif
}

int16_t acc_dummy_self_test(int32_t *result)
{
    /* initialize arg */
    *result = 0;

#if ACC_DUMMY_GET_DATA_FAIL
    *result = 0xAB;
    return AKM_ERR_IO;
#else
    return AKM_SUCCESS;
#endif
}
