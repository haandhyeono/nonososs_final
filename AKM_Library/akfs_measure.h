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
#ifndef AKFS_INC_MEASURE_H
#define AKFS_INC_MEASURE_H

/* Include files for AK8975 library. */
#include "akfs_compass.h"

/*** Constant definition ******************************************************/
#define AKFS_MAG_SENSE   (1)
#define AKFS_ACC_SENSE   (1)
#define AKFS_GEOMAG_MAX  (70)
#define AKFS_GEOMAG_MIN  (10)
#define AKFS_HNAVE_D     (4)
#define AKFS_ANAVE_D     (4)
#define AKFS_HNAVE_V     (8)
#define AKFS_ANAVE_V     (8)

/*** Type declaration *********************************************************/

/*** Global variables *********************************************************/

/*** Prototype of function ****************************************************/
/*!
 * Initialize #AKL_SCL_PRMS structure.
 * \param[out] prms A pointer to #AKL_SCL_PRMS structure.
 */
void AKFS_InitPRMS(
    struct AKL_SCL_PRMS *prms
);

void AKFS_SetDefaultNV(
    struct AKL_NV_PRMS *nv
);

int16_t AKFS_InitMeasure(
    struct AKL_SCL_PRMS *prms
);

/*! This function is called when new magnetometer data is available.  The
  coordination system of input vector is sensor local coordination system.
  The input vector will be converted to micro tesla unit (i.e. uT), then
  rotated using layout matrix (i.e. i16_hlayout).
  A magnetic offset is estimated automatically in this function.
  As a result of it, offset subtracted vector is stored in #AKMPRMS structure.

  @return #AKM_SUCCESS on success. Otherwise the return value is #AKM_ERROR.
  @param[in] prms A pointer to #AKMPRMS structure.
  @param[in] mag A set of measurement data from magnetometer.  X axis value
  should be in mag[0], Y axis value should be in mag[1], Z axis value should be
  in mag[2].
  @param[in] status A status of magnetometer.  This status indicates the result
  of measurement data, i.e. overflow, success or fail, etc.
 */
int16_t AKFS_Set_MAGNETIC_FIELD(
    struct AKL_SCL_PRMS *prms,
    const AKFLOAT       mag[3],
    const AKM_TIMESTAMP ts_mag
);

/*! This function is called when new accelerometer data is available.  The
  coordination system of input vector is Android coordination system.
  The input vector will be converted to SI unit (i.e. m/s/s).

  @return #AKM_SUCCESS on success. Otherwise the return value is #AKM_ERROR.
  @param[in] prms A pointer to #AKMPRMS structure.
  @param[in] acc A set of measurement data from accelerometer.  X axis value
  should be in acc[0], Y axis value should be in acc[1], Z axis value should be
  in acc[2].
  @param[in] status A status of accelerometer.  This status indicates the result
  of accelerometer data. Currently, this parameter is not used.
 */
int16_t AKFS_Set_ACCELEROMETER(
    struct AKL_SCL_PRMS *prms,
    const AKFLOAT       acc[3],
    const AKM_TIMESTAMP ts_acc
);

#endif
