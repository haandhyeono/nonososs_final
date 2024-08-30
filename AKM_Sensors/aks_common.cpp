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
#include "AKM_Common.h"

void AKS_MyStrcpy(
    char          *dst,
    const char    *src,
    const int16_t dst_len)
{
    int i = 1;

    while (*src != '\0') {
        *dst = *src;
        dst++;
        src++;
        i++;

        if (i >= dst_len) {
            break;
        }
    }

    *dst = '\0';
}

void AKS_ConvertCoordinate(
    int32_t       vec[3],
    const uint8_t axis_order[3],
    const uint8_t axis_sign[3])
{
    int32_t val32[3];
    uint8_t i;

    /* Axis conversion */
    for (i = 0; i < 3; i++) {
        val32[i] = vec[axis_order[i]];

        if (axis_sign[i]) {
            val32[i] *= -1;
        }
    }

    /* Copy to argument */
    for (i = 0; i < 3; i++) {
        vec[i] = val32[i];
    }
}

int16_t aks_fst_test_data(
    uint16_t testno,
    int16_t  testdata,
    int16_t  lolimit,
    int16_t  hilimit,
    int32_t  *err)
{
#ifdef DEBUG
    AKH_Print("DBG: FST 0x%08X\n", AKM_FST_ERRCODE(testno, testdata));
#endif

    if ((lolimit <= testdata) && (testdata <= hilimit)) {
        return AKM_SUCCESS;
    } else {
        *err = AKM_FST_ERRCODE(testno, testdata);
        return AKM_ERROR;
    }
}

int16_t aks_fst_test_data32(
    uint16_t testno,
    int32_t  testdata,
    int32_t  lolimit,
    int32_t  hilimit,
    int32_t  *err)
{
#ifdef DEBUG
    AKH_Print("DBG: FST 0x%08X\n", AKM_FST_ERRCODE(testno, testdata));
#endif

    if ((lolimit <= testdata) && (testdata <= hilimit)) {
        return AKM_SUCCESS;
    } else {
        *err = AKM_FST_ERRCODE(testno, testdata);
        return AKM_ERROR;
    }
}