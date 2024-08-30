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
#ifndef AKFS_INC_COMPASS_H
#define AKFS_INC_COMPASS_H

/* Root file of AKL module. */
#include "AKL_APIs.h"

/* Include files for AKM OSS library */
#include "akfs_aoc.h"
#include "akfs_configure.h"
#include "akfs_device.h"
#include "akfs_direction.h"
#include "akfs_math.h"
#include "akfs_vnorm.h"

/*** Type declaration *********************************************************/
struct AKSENSOR_DATA {
    AKFLOAT x;
    AKFLOAT y;
    AKFLOAT z;
    int8_t  status;
};

struct AKL_NV_PRMS {
    /*! This value is used to identify the data area is AKL_NV_PRMS.
     * This value should be #AKL_NV_MAGIC_NUMBER. */
    uint32_t magic;
    /*! Offset of magnetic vector */
    AKFVEC   fv_hsuc_ho;
};

struct AKL_SCL_PRMS {
    struct AKL_NV_PRMS *ps_nv;

    /* Variables forAOC. */
    AKFVEC             fva_hdata[AKFS_HDATA_SIZE];
    AKFS_AOC_VAR       s_aocv;

    /* Variables for Magnetometer buffer. */
    AKFVEC             fva_hvbuf[AKFS_HDATA_SIZE];
    AKFVEC             fv_ho;
    AKFVEC             fv_hs;

    /* Variables for Accelerometer buffer. */
    AKFVEC             fva_avbuf[AKFS_ADATA_SIZE];
    AKFVEC             fv_ao;
    AKFVEC             fv_as;

    /* Variables for Direction. */
    AKFLOAT            f_azimuth;
    AKFLOAT            f_pitch;
    AKFLOAT            f_roll;

    /* Variables for vector output */
    AKFVEC             fv_hvec;
    AKFVEC             fv_avec;
    int16_t            i16_hstatus;

    /* Time stamp for v_hvec */
    AKM_TIMESTAMP      m_ts_hvec;
    /* Time stamp for v_avec */
    AKM_TIMESTAMP      m_ts_avec;
};
#endif
