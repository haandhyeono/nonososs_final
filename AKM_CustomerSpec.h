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
 * \file AKM_CustomerSpec.h
 * \brief Customer's specification.
 */
#ifndef INCLUDE_AKM_CUSTOMERSPEC_H
#define INCLUDE_AKM_CUSTOMERSPEC_H


/*! \defgroup CUSTOMER_SPEC Customer specific parameters.
 * SmartCompass library and NanoMagSensor library parameters. <b>
 * Please change these parameters according to the directions from AKM.</b>
 @{*/
/*! A string of licenser. Don't change this string. */
#define AKM_CUSTOM_LICENSER  "ASAHIKASEI"
/*! A string of licensee. This string should be changed. */
#define AKM_CUSTOM_LICENSEE  "ECOMPASS"

/*! The number of formation. */
#define AKM_CUSTOM_NUM_FORM  1

/*! Measurement frequency */
#define AKM_CUSTOM_MAG_FREQ  10
#define AKM_CUSTOM_ACC_FREQ  50
#define AKM_CUSTOM_GYR_FREQ  100

/*! \defgroup CSPEC_AXIS The axis conversion
 * Axis conversion parameters.
 @{*/
#define AKM_CUSTOM_MAG_AXIS_ORDER_X  0
#define AKM_CUSTOM_MAG_AXIS_ORDER_Y  1
#define AKM_CUSTOM_MAG_AXIS_ORDER_Z  2
#define AKM_CUSTOM_MAG_AXIS_SIGN_X   0
#define AKM_CUSTOM_MAG_AXIS_SIGN_Y   0
#define AKM_CUSTOM_MAG_AXIS_SIGN_Z   0

#define AKM_CUSTOM_ACC_AXIS_ORDER_X  0
#define AKM_CUSTOM_ACC_AXIS_ORDER_Y  1
#define AKM_CUSTOM_ACC_AXIS_ORDER_Z  2
#define AKM_CUSTOM_ACC_AXIS_SIGN_X   0
#define AKM_CUSTOM_ACC_AXIS_SIGN_Y   0
#define AKM_CUSTOM_ACC_AXIS_SIGN_Z   0

#define AKM_CUSTOM_GYR_AXIS_ORDER_X  0
#define AKM_CUSTOM_GYR_AXIS_ORDER_Y  1
#define AKM_CUSTOM_GYR_AXIS_ORDER_Z  2
#define AKM_CUSTOM_GYR_AXIS_SIGN_X   0
#define AKM_CUSTOM_GYR_AXIS_SIGN_Y   0
#define AKM_CUSTOM_GYR_AXIS_SIGN_Z   0
/*@}*/

/*@}*/
#endif /* INCLUDE_AKM_CUSTOMERSPEC_H */
