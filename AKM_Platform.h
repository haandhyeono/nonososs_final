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
/*!
 * \file AKM_Platform.h
 * \brief Common definitions for AKM Platform.
 */
#ifndef INCLUDE_AKM_PLATFORM_H
#define INCLUDE_AKM_PLATFORM_H

#include "mbed.h"
#include "arm_math.h" /* for flaot32_t */

/* Additional definition */
#ifndef INT16_MIN
#define INT16_MIN (-32768)
#endif

#ifndef INT16_MAX
#define INT16_MAX (32767)
#endif


#endif /* INCLUDE_AKM_PLATFORM_H */
