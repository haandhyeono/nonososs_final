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
#ifndef AKFS_INC_MATH_H
#define AKFS_INC_MATH_H

#include "akfs_configure.h"
#include <math.h>

/***** Constant definition ****************************************************/
#define AKFS_PI  (3.141592654f)
#define RAD2DEG(rad)  ((rad) * 180.0f / AKFS_PI)

/***** Macro definition *******************************************************/

#ifdef AKFS_PRECISION_DOUBLE
#define AKFS_SIN(x)       sin(x)
#define AKFS_COS(x)       cos(x)
#define AKFS_ASIN(x)      asin(x)
#define AKFS_ACOS(x)      acos(x)
#define AKFS_ATAN2(y, x)  atan2((y), (x))
#define AKFS_SQRT(x)      sqrt(x)
#define AKFS_ABS(x)       fabs(x)
#else
#define AKFS_SIN(x)       sinf(x)
#define AKFS_COS(x)       cosf(x)
#define AKFS_ASIN(x)      asinf(x)
#define AKFS_ACOS(x)      acosf(x)
#define AKFS_ATAN2(y, x)  atan2f((y), (x))
#define AKFS_SQRT(x)      sqrtf(x)
#define AKFS_ABS(x)       fabsf(x)
#endif
#endif
