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
#ifndef AKFS_INC_DEVICE_H
#define AKFS_INC_DEVICE_H

#include "akfs_configure.h"
#include <float.h>
#include <stdint.h>

/***** Constant definition ****************************************************/
#define AKFS_ERROR       (0)
#define AKFS_SUCCESS     (1)

#define AKFS_HDATA_SIZE  (32)
#define AKFS_ADATA_SIZE  (32)

/***** Type declaration *******************************************************/

#ifdef AKFS_PRECISION_DOUBLE
typedef double AKFLOAT;
#define AKFS_EPSILON  DBL_EPSILON
#define AKFS_FMAX     DBL_MAX
#define AKFS_FMIN     DBL_MIN

#else
typedef float AKFLOAT;
#define AKFS_EPSILON  FLT_EPSILON
#define AKFS_FMAX     FLT_MAX
#define AKFS_FMIN     FLT_MIN
#endif

/* Treat maximum value as initial value */
#define AKFS_INIT_VALUE_F  AKFS_FMAX

/***** Vector ****************************************************************/
typedef union _uint8vec {
    struct {
        uint8_t x;
        uint8_t y;
        uint8_t z;
    } u;
    uint8_t v[3];
} uint8vec;

typedef union _AKFVEC {
    struct {
        AKFLOAT x;
        AKFLOAT y;
        AKFLOAT z;
    } u;
    AKFLOAT v[3];
} AKFVEC;

/***** Layout pattern ********************************************************/
typedef enum _AKFS_PATNO {
    PAT_INVALID = 0,
    PAT1, /* obverse: 1st pin is right down */
    PAT2, /* obverse: 1st pin is left down */
    PAT3, /* obverse: 1st pin is left top */
    PAT4, /* obverse: 1st pin is right top */
    PAT5, /* reverse: 1st pin is left down (from top view) */
    PAT6, /* reverse: 1st pin is left top (from top view) */
    PAT7, /* reverse: 1st pin is right top (from top view) */
    PAT8  /* reverse: 1st pin is right down (from top view) */
} AKFS_PATNO;

/***** Prototype of function **************************************************/
AKLIB_C_API_START
int16_t AKFS_InitBuffer(
    const int16_t ndata,  /*!< Size of raw vector buffer */
    AKFVEC        vdata[] /*!< Raw vector buffer */
);

int16_t AKFS_BufShift(
    const int16_t len,
    const int16_t shift,
    AKFVEC        v[]
);

int16_t AKFS_Rotate(
    const AKFS_PATNO pat,
    AKFVEC           *vec
);

int16_t AKFS_RotateMat(
    const int16_t layout[3][3],
    AKFVEC        *vec
);

AKLIB_C_API_END
#endif
