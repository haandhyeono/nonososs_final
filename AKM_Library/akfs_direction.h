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
#ifndef AKFS_INC_DIRECTION_H
#define AKFS_INC_DIRECTION_H

#include "akfs_device.h"

/***** Prototype of function **************************************************/
AKLIB_C_API_START
int16_t AKFS_Direction(
    const int16_t nhvec,
    const AKFVEC  hvec[],
    const int16_t hnave,
    const int16_t navec,
    const AKFVEC  avec[],
    const int16_t anave,
    AKFLOAT       *azimuth,
    AKFLOAT       *pitch,
    AKFLOAT       *roll
);

AKLIB_C_API_END
#endif
