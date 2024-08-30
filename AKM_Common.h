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
 * \file AKM_Common.h
 * \brief Common definitions for AKM software.
 */
#ifndef INCLUDE_AKM_COMMON_H
#define INCLUDE_AKM_COMMON_H

#include "AKM_Config.h"
#include "AKM_Platform.h"

#ifdef AKM_TIMESTAMP_NANOSECOND
/*! Time stamp (in nano seconds unit) for the data.*/
typedef int64_t AKM_TIMESTAMP;
#else
/*! Time stamp (in micro seconds unit) for the data.*/
typedef int32_t AKM_TIMESTAMP;
#endif

/*! \defgroup AKM_RETURN_VALUE AKM library's error code definition.
 * The return value (or error code) of AKM library API.
 @{*/
/*! This value represents success of operation. */
#define AKM_SUCCESS                 (0)
/*! This value represents some error happend while operation execution. */
#define AKM_ERROR                   (-1)
/*! This value represents argument of fuction is out of range or invalid. */
#define AKM_ERR_INVALID_ARG         (-2)
/*! This value represents the operation is not supported on this platform. */
#define AKM_ERR_NOT_SUPPORT         (-3)
/*! This value represents some I/O error happend while operation execution. */
#define AKM_ERR_IO                  (-4)
/*! This value represents device is busy or other process blocks the operation
 * execution. */
#define AKM_ERR_BUSY                (-5)
/*! This value represents the operation could not be done within the specified
 * duration time. */
#define AKM_ERR_TIMEOUT             (-6)
/*! This value represents that the calculation is not yet done. */
#define AKM_ERR_NOT_YET_CALCULATED  (-7)
/*@}*/

/*! Type of sensors. */
typedef enum {
    /*! No sensor type. This is used as initial value. */
    AKM_ST_NONE = 0x00,
    /*! Magnetometer\n
     * e.g. AK09911, AK09912. */
    AKM_ST_MAG = 0x01,
    /*! Accelerometer\n
     * e.g. ADXL345. */
    AKM_ST_ACC = 0x02,
    /*! Gyroscope\n
     * e.g. L3G4200D. */
    AKM_ST_GYR = 0x04,
    /*! Magnetometer and accelerometer combined device.\n
     * e.g. AK8976, AK09921. */
    AKM_ST_MAG_ACC_COMPOSIT = 0x03,
    /*! Accelerometer and gyroscope combiled device. */
    AKM_ST_ACC_GYR_COMPOSIT = 0x06,
    /*! 9D device. */
    AKM_ST_9D_COMPOSIT = 0x07,
    /*! TBD */
    AKM_ST_ALL_SENSORS = 0xFF
} AKM_SENSOR_TYPE;

/*! \defgroup AKM_VECTOR_TYPE_SIZE Library output vector size definition.
 * The size of vector which is got from #AKL_GetVector API.
 @{*/
/*! data[0,1,2] = (X,Y,Z) data[3,4,5] = (bias_X,bias_Y,bias_Z). */
#define AKM_VT_MAG_SIZE      6U
/*! data[0,1,2] = (X,Y,Z). */
#define AKM_VT_ACC_SIZE      3U
/*! data[0,1,2] = (X,Y,Z) data[3,4,5] = (bias_X,bias_Y,bias_Z). */
#define AKM_VT_GYR_SIZE      6U
/*! data[0,1,2] = (yaw,pitch,roll). */
#define AKM_VT_ORI_SIZE      3U
/*! data[0,1,2] = (X,Y,Z). */
#define AKM_VT_GRAVITY_SIZE  3U
/*! data[0,1,2] = (X,Y,Z). */
#define AKM_VT_LACC_SIZE     3U
/*! data[0,1,2,3] = (x,y,z,w). */
#define AKM_VT_QUAT_SIZE     4U
/*@}*/

/*!
 * Type of vector. When get this type of vector,
 * the buffer should be larger than the corresponding definition size.
 */
typedef enum {
    /*! Magnetometer.\n
     * A data is stored in the vector in order of x, y, z,
     * bias_x, bias_y, and bias_z.\n
     * The data format is micro tesla in Q16 format. */
    AKM_VT_MAG = 0x01,
    /*! Accelerometer.\n
     * A data is stored in the vector in order of x, y, and z.\n
     * The data format is SI unit (m/s^2) in Q16 format. */
    AKM_VT_ACC = 0x02,
    /*! Gyroscope.\n
     * A data is stored in the vector in order of x, y, z,
     * bias_x, bias_y, and bias_z.\n
     * The data format is degree/second in Q16 format. */
    AKM_VT_GYR = 0x04,
    /*! Orientation.\n
     * A data is stored in the vector in order of yaw, pitch, and roll.
     *   Yaw angle   (0 to 360)    degree in Q16.\n
     *   Pitch angle (-180 to 180) degree in Q16.\n
     *   Roll angle  (-90 to 90)   degree in Q16.*/
    AKM_VT_ORI = 0x08,
    /*! Gravity.
     * The data format is the same as #AKM_VT_ACC. */
    AKM_VT_GRAVITY = 0x10,
    /*! Linear accelerometer.
     * The data format is the same as #AKM_VT_ACC. */
    AKM_VT_LACC = 0x20,
    /*! Quaternion.\n
     * A data is stored in the vector in order of x, y, z, and w. */
    AKM_VT_QUAT = 0x40,
    /*! Magnetometer for DOEaG.\n
     * A data is stored in the vector in order of x, y, z,
     * bias_x, bias_y, and bias_z.\n
     * The data format is micro tesla in Q16 format. */
    AKM_VT_MAG_DOEAG = 0x80,
} AKM_VECTOR_TYPE;

/*!
 * A structure which holds set of sensor data.
 * The sensor's axis direction definition is same as Android.\n
 * i.e.\n
 *  \li from center to right: positive X.
 *  \li from center to forward: positive Y.
 *  \li from center to top (toward to sky): positive Z.
 *
 * The data unit is a little bit different because this structure
 *  takes only integer value.
 *  \li \b MAGNETOMETER: \n
 *  micro Tesla in Q16 format. e.g. 1 uT = 65536 (Q16)
 *  \li \b ACCELEROMETER: \n
 *  SI unit (m/s^2) in Q16 format. e.g. 1G = 9.81 m/s^2 = 642908 (Q16)
 *  \li \b GYROSCOPE: \n
 *  degree/second in Q16 format. e.g. 1 dps = 65536 (Q16)
 *
 *
 */
struct AKM_SENSOR_DATA {
    union {
        struct {
            int32_t x;
            int32_t y;
            int32_t z;
        } s;
        struct {
            int32_t i1;
            int32_t i2;
            int32_t i3;
            int32_t i4;
            int32_t t;
        } r;
        int32_t v[5];
    } u;
    /*!
     * This field holds time stamp for the data.
     * The unit of timestamp depends is nano seconds when AKM_TIMESTAMP_NANOSECOND
     * is defined. Otherwise, the unit of timestamp is micro seconds.
     * Ideally, this value is calculated from system tick, but fixed interval
     * value is acceptable.\n */
    AKM_TIMESTAMP timestamp;

    /*!
     * This field is used to store device status register value.
     * In case of AKM compass, \c ST1 and \c ST2 value is put to
     * \c status[0] and \c status[1] respectively.*/
    int16_t status[2];
    /*!
     * This field is used to store sensor type of this data.
     * Some APIs use this field, in order to determine from which sensor type
     * this data was acquired.*/
    AKM_SENSOR_TYPE stype;
};
#endif /* INCLUDE_AKM_COMMON_H */
