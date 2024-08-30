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
#include "akfs_device.h"

/******************************************************************************/
/*! Initialize #AKFVEC array.
  @return #AKFS_SUCCESS on success. Otherwise the return value is #AKFS_ERROR.
  @param[in] ndata
  @param[out] vdata
 */
int16_t AKFS_InitBuffer(
    const int16_t ndata,  /*!< Size of vector buffer */
    AKFVEC        vdata[] /*!< Vector buffer */
)
{
    int i;

    /* size check */
    if (ndata <= 0) {
        return AKFS_ERROR;
    }

    for (i = 0; i < ndata; i++) {
        vdata[i].u.x = AKFS_INIT_VALUE_F;
        vdata[i].u.y = AKFS_INIT_VALUE_F;
        vdata[i].u.z = AKFS_INIT_VALUE_F;
    }

    return AKFS_SUCCESS;
}

/******************************************************************************/
/*! Shift #AKFVEC array.
  @return #AKFS_SUCCESS on success. Otherwise the return value is #AKFS_ERROR.
  @param[in] len
  @param[in] shift
  @param[in/out] v
 */
int16_t AKFS_BufShift(
    const int16_t len,   /*!< size of buffer */
    const int16_t shift, /*!< shift size */
    AKFVEC        v[]    /*!< buffer */
)
{
    int16_t i;

    if ((shift < 1) || (len < shift)) {
        return AKFS_ERROR;
    }

    for (i = len - 1; i >= shift; i--) {
        v[i] = v[i - shift];
    }

    return AKFS_SUCCESS;
}

/******************************************************************************/
/*! Rotate vector according to the specified layout pattern number.
  @return #AKFS_SUCCESS on success. Otherwise the return value is #AKFS_ERROR.
  @param[in] pat
  @param[in/out] vec
 */
int16_t AKFS_Rotate(
    const AKFS_PATNO pat,
    AKFVEC           *vec)
{
    AKFLOAT tmp;

    switch (pat) {
    /* Obverse */
    case PAT1:
        /* This is Android default */
        break;

    case PAT2:
        tmp = vec->u.x;
        vec->u.x = vec->u.y;
        vec->u.y = -tmp;
        break;

    case PAT3:
        vec->u.x = -(vec->u.x);
        vec->u.y = -(vec->u.y);
        break;

    case PAT4:
        tmp = vec->u.x;
        vec->u.x = -(vec->u.y);
        vec->u.y = tmp;
        break;

    /* Reverse */
    case PAT5:
        vec->u.x = -(vec->u.x);
        vec->u.z = -(vec->u.z);
        break;

    case PAT6:
        tmp = vec->u.x;
        vec->u.x = vec->u.y;
        vec->u.y = tmp;
        vec->u.z = -(vec->u.z);
        break;

    case PAT7:
        vec->u.y = -(vec->u.y);
        vec->u.z = -(vec->u.z);
        break;

    case PAT8:
        tmp = vec->u.x;
        vec->u.x = -(vec->u.y);
        vec->u.y = -tmp;
        vec->u.z = -(vec->u.z);
        break;

    default:
        return AKFS_ERROR;
    }

    return AKFS_SUCCESS;
}

/******************************************************************************/
/*! Rotate vector according to the specified layout matrix.
  @return #AKFS_SUCCESS on success. Otherwise the return value is #AKFS_ERROR.
  @param[in] pat
  @param[in/out] vec
 */
int16_t AKFS_RotateMat(
    const int16_t layout[3][3],
    AKFVEC        *vec)
{
    AKFVEC tmp;

    tmp.u.x = layout[0][0] * vec->u.x + layout[0][1] * vec->u.y + layout[0][2] *
        vec->u.z;
    tmp.u.y = layout[1][0] * vec->u.x + layout[1][1] * vec->u.y + layout[1][2] *
        vec->u.z;
    tmp.u.z = layout[2][0] * vec->u.x + layout[2][1] * vec->u.y + layout[2][2] *
        vec->u.z;

    *vec = tmp;

    return AKFS_SUCCESS;
}
