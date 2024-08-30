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
#ifndef AKFS_INC_CONFIG_H
#define AKFS_INC_CONFIG_H

/***** Language configuration *************************************************/
#if defined(__cplusplus)
#define AKLIB_C_API_START  extern "C" {
#define AKLIB_C_API_END    }
#else
#define AKLIB_C_API_START
#define AKLIB_C_API_END
#endif

/* If following line is commented in, double type is used for floating point
   calculation */
/* #define AKFS_PRECISION_DOUBLE */
#endif
