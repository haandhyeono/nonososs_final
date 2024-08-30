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
 * \file AKH_APIs.h
 * \brief Hardware abstruction layer.
 */
#ifndef INCLUDE_AKH_APIS_H
#define INCLUDE_AKH_APIS_H

#include "AKM_Common.h"

#ifndef UART_COMMUNICATION_H
#define UART_COMMUNICATION_H

#include "mbed.h"

// UART 설정 (디버그 정보 출력)
extern BufferedSerial uart;

void processInput();

void readInput();

void processSensorData();

#endif // UART_COMMUNICATION_H


/**
 * The list of IRQ which is supported by this HAL.
 */
typedef enum _AKH_IRQ {
    IRQ_USER = 1,
    IRQ_MAG_1,
    IRQ_MAG_2,
    IRQ_ACC_1,
    IRQ_ACC_2,
    IRQ_GYR_1,
    IRQ_GYR_2
} AKH_IRQ;

typedef void (*IRQ_CALLBACK)(
    void
);

/******************************************************************************
 * API decleration.
 ******************************************************************************/
/*!
 * Initialize system. This function must be called before any other function
 * that is provided by the driver is called.
 *
 * \return No return value is reserved.
 */
void AKH_Init(
    void
);


/*!
 * Write data to a device via serial interface. When more than one byte of
 * data is specified, the data is written in contiguous locations starting
 * at an address specified in address.
 * \retval #AKM_SUCCESS When operation is succeeded.
 * \retval negative-value When operation failed.
 * \param stype Specify sensor type.
 * \param address Register address to write.
 * \param data A pointer to a data buffer to be written.
 * \param numberOfBytesToWrite The number of byte to be written.
 */
int16_t AKH_TxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         address,
    const uint8_t         *data,
    const uint16_t        numberOfBytesToWrite
);


/*!
 * Acquire data from the device via serial interface.
 * \retval #AKM_SUCCESS When operation is succeeded.
 * \retval negative-value When operation failed.
 * \param stype Specify sensor type.
 * \param address Register address to read.
 * \param data a Pointer to the data to be read.
 * \param numberOfBytesToRead The number of byte to be read.
 */
int16_t AKH_RxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t         address,
    uint8_t               *data,
    const uint16_t        numberOfBytesToRead
);

/*!
 * Reset device. This function resets the devicy physically. Sometimes it is
 * called as 'hard reset'. If the device does not support hard rest operation
 * or RESET signal is not connected to GPIO pins, this function returns
 * #AKM_ERR_NOT_SUPPORT.
 * \retval #AKM_SUCCESS When operation is succeeded.
 * \retval negative-value When operation failed.
 * \param stype Specify sensor type.
 */
int16_t AKH_Reset(
    const AKM_SENSOR_TYPE stype
);

/*!
 * Suspend execution.
 * \return No return value is reserved.
 * \param us Duration time in micro-seconds unit.
 */
void AKH_DelayMicro(
    const uint16_t us
);

/*!
 * Suspend execution.
 * \return No return value is reserved.
 * \param ms Duration time in milli-seconds unit.
 */
void AKH_DelayMilli(
    const uint16_t ms
);

/*!
 * Set IRQ handler for each sensor.
 * \return When specified sensor does not supported, the return value is
 * #AKM_ERR_NOT_SUPPORT
 * \param stype Specify sensor type.
 * \param func Callback function. If this parameter is NULL, IRQ is disabled.
 */
int16_t AKH_SetIRQHandler(
    const AKH_IRQ irq,
    IRQ_CALLBACK  func
);

/*!
 * Get timestamp
 * \return Current time. The unit depends on definition.
 */
AKM_TIMESTAMP AKH_GetTimestamp(
    void
);

/*!
 * Save data to non-volatile storage.
 * \retval #AKM_SUCCESS When operation is succeeded.
 * \retval #AKM_ERR_NOT_SUPPORT When this platform does not support non-volatile
 * storage.
 * \param param A pointer to a data buffer to be saved.
 * \param nBytes The lenght of \c param.
 */
int16_t AKH_SaveParameter(
    const uint8_t  *param,
    const uint16_t nBytes
);

/*!
 * Load data from non-volatile storage.
 * \retval #AKM_SUCCESS When operation is succeeded.
 * \retval #AKM_ERR_NOT_SUPPORT When this platform does not support non-volatile
 * storage.
 * \param param A pointer to a data buffer which loaded data is stored.
 * \param nBytes The lenght of \c param.
 */
int16_t AKH_LoadParameter(
    uint8_t        *param,
    const uint16_t nBytes
);

/*!
 * Print function.
 * \return No return value is reserved yet.
 * \param format A pointer to a format string.
 */
void AKH_Print(
    const char *format,
    ...
);

/*!
 * Check whether user input (or any staus changed signal) is occured.
 * This function does not block current process (or thread).
 * It means this function return the result immediately.
 * \retval AKM_SUCCESS When user input is detected.
 * \retval AKM_ERR_TIMEOUT No user input is detected.
 * \retval negative-value Other error happend.
 * \param code This variable is used to determine what type of input
 * was happend. For example, this function detects keyboard input,
 * this function may return the pressed key code.
 * \c NULL is acceptable for this parameter, but you cannot get the event
 * code by any other method.
 */
int16_t AKH_CheckUserInput(
    uint32_t *code
);

/*
 * TODO: to be implemented function list
 *
 * int16_t AKH_SetTimer()
 * int16_t AKH_SetCallback()
 */
#endif /* INCLUDE_AKH_APIS_H */