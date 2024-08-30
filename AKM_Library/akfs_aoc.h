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
#ifndef AKFS_INC_AOC_H
#define AKFS_INC_AOC_H

#include "akfs_device.h"

/***** Constant definition ****************************************************/
#define AKFS_HBUF_SIZE   (20)
#define AKFS_HOBUF_SIZE  (4)
#define AKFS_HR_TH       (10)
#define AKFS_HO_TH       (0.15f)

/***** Macro definition *******************************************************/

/***** Type declaration *******************************************************/
typedef struct _AKFS_AOC_VAR {
    AKFVEC  hbuf[AKFS_HBUF_SIZE];
    AKFVEC  hobuf[AKFS_HOBUF_SIZE];
    AKFLOAT hraoc;
} AKFS_AOC_VAR;

/***** Prototype of function **************************************************/
AKLIB_C_API_START
int16_t AKFS_AOC(
    AKFS_AOC_VAR *haocv,
    const AKFVEC *hdata,
    AKFVEC       *ho
);

void AKFS_InitAOC(
    AKFS_AOC_VAR *haocv
);

AKLIB_C_API_END
#endif
