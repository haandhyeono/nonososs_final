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
#include "AKL_APIs.h"
#include "AKM_CustomerSpec.h"
#include "AKS_APIs.h"
#include "akm_extra.h"

/**
 * \brief Set certification information to enable DOE function.
 * This function sets LICENSER and LICENSEE string, then
 * get hardware information from driver.
 *
 * \param info A pointer to #AKL_CERTIFICATION_INFO struct.
 *
 * \return When function succeeds, #AKM_SUCCESS is returned.
 */
int16_t set_certification_info(
    struct AKL_CERTIFICATION_INFO *info,
    AKM_DEVICES                   *device)
{
    struct AKS_DEVICE_INFO dev_info;
    uint8_t                n;
    int16_t                ret;

    n = 1U;
    ret = AKS_GetDeviceInfo(AKM_ST_MAG, &dev_info, &n);

    if (AKM_SUCCESS != ret) {
        return ret;
    }

    *device = dev_info.device;

    return AKM_SUCCESS;
}

int16_t set_compensation_info(
    struct AKL_SCL_PRMS *prm)
{
    return AKM_SUCCESS;
}
