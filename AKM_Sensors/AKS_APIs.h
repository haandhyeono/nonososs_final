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
 * \file AKS_APIs.h
 * \brief Sensor control API.
 */
#ifndef INCLUDE_AKS_APIS_H
#define INCLUDE_AKS_APIS_H

#include "AKM_Common.h"

#define AKS_INFO_NAME_SIZE  16 /*!< The length of device name. */
#define AKS_PARAMETER_SIZE  8  /*!< The length of device parameter. */

struct AKS_DEVICE_INFO {
    /*! The name of device. */
    char        name[AKS_INFO_NAME_SIZE];
    /*! The type of device. */
    AKM_DEVICES device;
    /*! An array of prameters. The value depends on device. In case of AKM
     * compass device, FUSE ROM value is stored. */
    uint16_t    parameter[AKS_PARAMETER_SIZE];
};

void processInput();
void readInput();

/*!
 * Configure sensor boards which is equipped on the target board.
 * This function must be called before #AKS_Init is called.
 * \retval Psitive The number of sensors which are successfully configured.
 * \retval 0 Sensors are not configured at all. May be something wrong.
 * \retval Negative Something wrong with configuration.
 * \param num The number of sensors in the list. In other words, the
 * length of #dev parameter.
 * \param dev An array of sensors. If same type of sensor is specified,
 * later one will be used.
 */
int16_t AKS_Config(
    const uint8_t     num,
    const AKM_DEVICES dev[]
);

/*!
 * Initialize sensor device.
 * This function resets the device, then intialize the device.
 * This function may be used as RESET function of device.
 * Therefore this API may be called several time to reset the device.
 * After initialization process is done, device should be in low-power
 * mode (i.e. power-down mode etc.)
 * \retval AKM_SUCCESS The operation has done successfully.
 * \retval Negative Something wrong with initialization.
 *  For example, #AKM_ERR_NOT_SUPPORT means the specified device is not
 *  presented.
 * \param stype Specify types of sensor to be initialized. This argument
 *  accepts a 'or' combination of #AKM_SENSOR_TYPE.
 * \param axis_order A pointer to an array of axis order paraemter.
 *  The element of the array must be 0, 1, or 2 and must not overlap.
 * \param axis_sign A pointer to an array of axis sign paraemter.
 *  The element of the array must be 0 or 1. 0 means no change, 1 means
 *  invert the axis.
 */
int16_t AKS_Init(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         axis_order[3],
    const uint8_t         axis_sign[3]
);


/*!
 * Get device information.
 * \retval AKM_SUCCESS The operation has done successfully.
 * \retval Negative Something wrong with initialization.
 *  For example, #AKM_ERR_NOT_SUPPORT means the specified device is not
 *  presented.
 * \param stype Specify a type of sensor.
 * \param info A pointer to AKS_DEVICE_INFO struct (or struct array).
 * \param num As an input, specify the number of info struct array.
 *  As output, the number of info struct got from this system.
 */
int16_t AKS_GetDeviceInfo(
    const AKM_SENSOR_TYPE  stype,
    struct AKS_DEVICE_INFO *info,
    uint8_t                *num
);


/*!
 * Start device measurement.
 * \retval AKM_SUCCESS The operation has done successfully.
 * \retval Negative Something wrong with the operation.
 *  This function may return the following value.
 *  AKM_ERR_INVALID_ARG Interval_us value is invalid.
 *  AKM_ERR_NOT_SUPPORT The specified device is not presented.
 *  AKM_ERR_IO Could not start because of I/O error.
 *  AKM_ERR_BUSY The device is not ready to start yet.
 * \param stype Specify types of sensor to be started. This argument
 *  accepts a 'or' combination of #AKM_SENSOR_TYPE.
 * \param interval_us Specify the measurement interval in micro seconds unit.
 *  But note that actual measurement interval may differ from the specified
 *  value. This function try to set the closest value as posible as it can.
 *  When negative value is specified, device operate single shot measurement
 *   if it is supported. When single shot measurement has done, device will
 *  automatically stop (same state as when AKS_Stop is called).
 */
int16_t AKS_Start(
    const AKM_SENSOR_TYPE stype,
    const int32_t         interval_us
);


/*!
 * Stop device measurement.
 * \retval AKM_SUCCESS The operation has done successfully.
 * \param stype Specify types of sensor to be stopped. This argument
 *  accepts a 'or' combination of #AKM_SENSOR_TYPE.
 */
int16_t AKS_Stop(
    const AKM_SENSOR_TYPE stype
);


/*!
 * Check if the sensor is ready to read new data.
 * If a new data is not available yet, this function blocks the
 * process of caller until elapsed time which is specified with
 * timeout_us has passed.
 * \retval Negative It means something wrong with device.
 * \retval 0 It means that the device is not ready yet.
 * \retval Positive The return value shows the number of new data set
 *  which can be read. If new data was already obtained by calling #AKS_GetData,
 *  this function will return 0 (because the 'new' data is not ready yet).
 *  If the device is 3-axial sensor, a set of (x, y, z) data is counted
 *  as 1 (not 3).
 * \param stype Specify a type of sensor.
 * \param timeout_us When 0 or negative value is specified, this function
 *  will return immediately after checking the device.
 */
int16_t AKS_CheckDataReady(
    const AKM_SENSOR_TYPE stype,
    const int32_t         timeout_us
);


/*!
 * Get data from device.
 * This function does not warrant the obtained data is the newest or
 *  not obtained before. It means that when this function is called
 *  sequentially, the same data may be obtained several time.
 *  In order to avoid the duplicated data, it is necessary to call
 *  #AKS_CheckDataReady in advance and to check the number of new data.
 * \retval AKM_SUCCESS The operation has done successfully.
 * \retval Negative Something wrong with the operation.
 * \param stype Specify a type of sensor.
 * \param data A pointer to #AKM_SENSOR_DATA struct array.
 * \param num The number of data buffer as input. The number shows the length
 *  of data struct array. When this function finished successfully, the return
 *  number shows the number of data to be filled in the data array.
 *  Therefore, the returned number is equal or less than input number.
 *  This number does not show the number of new data, so some device may
 *  fill the buffer with only new data, but some may not.
 */
int16_t AKS_GetData(
    const AKM_SENSOR_TYPE  stype,
    struct AKM_SENSOR_DATA *data,
    uint8_t                *num
);


/*!
 * Do self-test operation.
 * This function will block the process of caller until operation is done.
 * \retval AKM_SUCCESS The operation has done successfully.
 * \retval Negative Something wrong with the operation.
 * \param stype Specify a type of sensor.
 * \param result A pointer to a status code or a test result.
 */
int16_t AKS_SelfTest(
    const AKM_SENSOR_TYPE stype,
    int32_t               *result
);

#endif /* INCLUDE_AKS_APIS_H */
