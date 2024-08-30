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
#ifndef INCLUDE_PRINT_UTIL_H
#define INCLUDE_PRINT_UTIL_H

#include "AKM_Common.h"

/*!
 * \brief Print software version information.
 */
void print_version(void);

/*!
 * \brief Print vector data.
 *
 * \param stype A sensor type.
 * \param data[] The first 3 elements hold x,y,z data.
 * The latter 3 elements hold bias data. The data format must be Q16.
 * \param data_size Size of the data array.
 * \param st A status of sensor data.
 * \param ts A timestamp of the sensor data.
 */
void print_data(
    const AKM_SENSOR_TYPE stype,
    const int32_t         data[6],
    const int32_t         st,
    AKM_TIMESTAMP         ts
);

/*!
 * \brief Print Yaw, Pitch and Roll angle.
 *
 * \param data[] It holds Yaw, Pitch and Roll angle data
 * respectively. The data format is Q16.
 * \param data_size Size of the data array.
 * \param ts A timestamp of the sensor data.
 */
void print_ypr(
    const int32_t data[],
    AKM_TIMESTAMP ts
);

/*!
 * \brief Print quaternion vector.
 *
 * \param quat[] A pointer to quaternion vector.
 * The element is X,Y,Z and W respectively.
 * \param ts A timestamp of the sensor data.
 */
void print_quat(
    const int32_t quat[],
    AKM_TIMESTAMP ts
);

#endif /* INCLUDE_PRINT_UTIL_H */
