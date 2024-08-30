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
#ifndef INCLUDE_AKS_COMMON_H
#define INCLUDE_AKS_COMMON_H

#include "AKM_Common.h"

#define ACC_1G_IN_Q16  (642908)

#define AKM_FST_ERRCODE(testno, data) \
    (int32_t)((((uint32_t)testno) << 16) | ((uint16_t)data))
#define AKM_FST(no, data, lo, hi, err) \
    if (aks_fst_test_data((no), (data), (lo), (hi), (err)) != AKM_SUCCESS) \
    { goto SELFTEST_FAIL; }
#define AKM_FST32(no, data, lo, hi, err) \
    if (aks_fst_test_data32((no), (data), (lo), (hi), (err)) != AKM_SUCCESS) \
    { goto SELFTEST_FAIL; }

struct aks_interface {
    int16_t (* aks_init)(const uint8_t axis_order[3], const uint8_t axis_sign[3]);
    int16_t (* aks_get_info)(struct AKS_DEVICE_INFO *info);
    int16_t (* aks_start)(const int32_t interval_us);
    int16_t (* aks_stop)(void);
    int16_t (* aks_check_rdy)(const int32_t timeout_us);
    int16_t (* aks_get_data)(struct AKM_SENSOR_DATA *data, uint8_t *num);
    int16_t (* aks_self_test)(int32_t *result);
};

void AKS_MyStrcpy(
    char          *dst,
    const char    *src,
    const int16_t dst_len
);

void AKS_ConvertCoordinate(
    int32_t vec[3],
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3]
);

int16_t aks_fst_test_data(
    uint16_t testno,
    int16_t  testdata,
    int16_t  lolimit,
    int16_t  hilimit,
    int32_t  *err
);

int16_t aks_fst_test_data32(
    uint16_t testno,
    int32_t  testdata,
    int32_t  lolimit,
    int32_t  hilimit,
    int32_t  *err
);

#endif /*INCLUDE_AKS_COMMON_H*/

