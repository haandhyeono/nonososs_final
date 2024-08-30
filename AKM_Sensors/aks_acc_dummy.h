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
#ifndef INCLUDE_AKS_ACC_DUMMY_H
#define INCLUDE_AKS_ACC_DUMMY_H
#include "AKM_Common.h"
#include "AKS_APIs.h"

int16_t acc_dummy_init(
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3]
);

int16_t acc_dummy_get_info(
    struct AKS_DEVICE_INFO *info
);

int16_t acc_dummy_start(
    const int32_t interval_us
);

int16_t acc_dummy_stop(
    void
);

int16_t acc_dummy_check_rdy(
    const int32_t timeout_us
);

int16_t acc_dummy_get_data(
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num
);

int16_t acc_dummy_self_test(
    int32_t *result
);

#endif /* INCLUDE_AKS_ACC_DUMMY_H */
