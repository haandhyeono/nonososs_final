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
#include "akm_extra.h"
#include "AKH_APIs.h"
#include "AKL_APIs.h"
#include "AKM_CustomerSpec.h"
#include "AKS_APIs.h"
#include "looper.h"
#include "print_util.h"
#include <chrono>

using namespace std::chrono;





/*! Initialize a board (or hardware environment) at first.
 * Then initialize sensor. Sensor axis swapping is done in sensor abstruction
 * layer (which API name starts with 'AKS_'). You need to set correct parameter
 * when AKS_Init is called.
 * \return When function succeeds, #AKM_SUCCESS is returned. */
static int16_t hw_init(void)
{
    int16_t fret;
    uint8_t axis_order[3];
    uint8_t axis_sign[3];
    const AKM_DEVICES dev[] = {CONFIG_SLOT1, CONFIG_SLOT2, CONFIG_SLOT3};

    /* Initialize hardware */
    AKH_Init();
    
    fret = AKS_Config(3, dev);

    if (fret <= 0) {
        AKH_Print("AKS_Config failed...\n");
        return AKM_ERROR;
    }

    /* Initialize magnetic sensor */
    axis_order[0] = (uint8_t)AKM_CUSTOM_MAG_AXIS_ORDER_X;
    axis_order[1] = (uint8_t)AKM_CUSTOM_MAG_AXIS_ORDER_Y;
    axis_order[2] = (uint8_t)AKM_CUSTOM_MAG_AXIS_ORDER_Z;
    axis_sign[0] = (uint8_t)AKM_CUSTOM_MAG_AXIS_SIGN_X;
    axis_sign[1] = (uint8_t)AKM_CUSTOM_MAG_AXIS_SIGN_Y;
    axis_sign[2] = (uint8_t)AKM_CUSTOM_MAG_AXIS_SIGN_Z;

    fret = AKS_Init(AKM_ST_MAG, axis_order, axis_sign);

    if (fret != AKM_SUCCESS) {
        AKH_Print("AKS_Init (MAG) failed...\n");
        return fret;
    }

    axis_order[0] = (uint8_t)AKM_CUSTOM_ACC_AXIS_ORDER_X;
    axis_order[1] = (uint8_t)AKM_CUSTOM_ACC_AXIS_ORDER_Y;
    axis_order[2] = (uint8_t)AKM_CUSTOM_ACC_AXIS_ORDER_Z;
    axis_sign[0] = (uint8_t)AKM_CUSTOM_ACC_AXIS_SIGN_X;
    axis_sign[1] = (uint8_t)AKM_CUSTOM_ACC_AXIS_SIGN_Y;
    axis_sign[2] = (uint8_t)AKM_CUSTOM_ACC_AXIS_SIGN_Z;

    fret = AKS_Init(AKM_ST_ACC, axis_order, axis_sign);

    if (fret != AKM_SUCCESS) {
        if (fret != AKM_ERR_NOT_SUPPORT) {
            AKH_Print("AKS_Init_ACC (%d) failed...\n", fret);
            return fret;
        }
    }

    axis_order[0] = (uint8_t)AKM_CUSTOM_GYR_AXIS_ORDER_X;
    axis_order[1] = (uint8_t)AKM_CUSTOM_GYR_AXIS_ORDER_Y;
    axis_order[2] = (uint8_t)AKM_CUSTOM_GYR_AXIS_ORDER_Z;
    axis_sign[0] = (uint8_t)AKM_CUSTOM_GYR_AXIS_SIGN_X;
    axis_sign[1] = (uint8_t)AKM_CUSTOM_GYR_AXIS_SIGN_Y;
    axis_sign[2] = (uint8_t)AKM_CUSTOM_GYR_AXIS_SIGN_Z;

    fret = AKS_Init(AKM_ST_GYR, axis_order, axis_sign);

    if (fret != AKM_SUCCESS) {
        if (fret != AKM_ERR_NOT_SUPPORT) {
            AKH_Print("AKS_Init_GYR (%d) failed...\n", fret);
            return fret;
        }
    }

    return AKM_SUCCESS;
}

/*! Library initialize function.
 * This function allocate new memory area for #AKL_SCL_PRMS.
 * When this function succeeds, #AKL_SCL_PRMS parameter is ready to
 * start measurement. But please note that this function does
 * not restore previously saved data, for example, offset and its
 * accuracy etc.\n
 * When this function fails, \c prm is cleaned up internally,
 * so you don't need to \c 'free' outside of this function.
 * \return When function succeeds, #AKM_SUCCESS is returned.
 * \param prm A pointer of pointer to #AKL_SCL_PRMS struct. */
static int16_t library_init(struct AKL_SCL_PRMS **prm)
{
    uint32_t                      prmSz;
    int16_t                       fret;

    /* alloc library data parameter */
    prmSz = AKL_GetParameterSize((uint8_t)AKM_CUSTOM_NUM_FORM);

    if (prmSz == 0U) {
        *prm = NULL;
        fret = AKM_ERROR;
        goto EXIT_LIBRARY_INIT;
    }

    *prm = (struct AKL_SCL_PRMS *)malloc((size_t)prmSz);

    if (*prm == NULL) {
        fret = AKM_ERROR;
        goto EXIT_LIBRARY_INIT;
    }

    memset(*prm, 0, (size_t)prmSz);

    /* Initialize AKM library. */
    /* The 4th argument (device type) is not used in OSS library.
     * So, any device number is O.K. */
    fret = AKL_Init(*prm, NULL, (uint8_t)AKM_CUSTOM_NUM_FORM, AKM_MAGNETOMETER_AK8963);

    if (fret != AKM_SUCCESS) {
        goto EXIT_LIBRARY_INIT;
    }

    return AKM_SUCCESS;

EXIT_LIBRARY_INIT:

    if (*prm != NULL) {
        free(*prm);
        *prm = NULL;
    }

    return fret;
}

/*! This function restore previous parameter from non-volatile
 * area (for example, EEPROM on MCU, a file on filesystem).
 * If previous parameter is not restored correctly (may be it's
 * collapsed), a default parameter is set.
 * When this function succeeds, it is completely ready to start
 * measurement.
 * \return When function succeeds, #AKM_SUCCESS is returned.
 * \param prm A pointer to #AKL_SCL_PRMS struct. */
static int16_t load_and_start(struct AKL_SCL_PRMS *prm)
{
    uint8_t  *nv_data;
    uint16_t nvSz;
    int16_t  fret;

    /* alloc nv data area */
    nvSz = AKL_GetNVdataSize((uint8_t)AKM_CUSTOM_NUM_FORM);

    if (nvSz == 0U) {
        nv_data = NULL;
        fret = AKM_ERROR;
        goto LOAD_QUIT;
    }

    nv_data = (uint8_t *)malloc((size_t)nvSz);

    /* load data from storage */
    if (nv_data != NULL) {
        fret = AKH_LoadParameter(nv_data, nvSz);

        if (fret != AKM_SUCCESS) {
            free(nv_data);
            nv_data = NULL;
        }
    }

    /* AKL can accept NULL pointer for nv_data */
    fret = AKL_StartMeasurement(prm, nv_data);

    if (fret != AKM_SUCCESS) {
        goto LOAD_QUIT;
    }

    /* If any compensation data is available,
     * set the parameter.
     */
    fret = set_compensation_info(prm);

    if (fret != AKM_SUCCESS) {
        goto LOAD_QUIT;
    }

LOAD_QUIT:

    if (nv_data != NULL) {
        free(nv_data);
    }

    return fret;
}

/*! This function save parameter to non-volatile area for next
 * measurement. If this function fails, next measurement will
 * start with a default parameters.
 * \return When function succeeds, #AKM_SUCCESS is returned.
 * \param prm A pointer to #AKL_SCL_PRMS struct. */
static int16_t stop_and_save(struct AKL_SCL_PRMS *prm)
{
    uint8_t  *nv_data;
    uint16_t nvSz;
    int16_t  fret;

    /* alloc nv data area */
    nvSz = AKL_GetNVdataSize((uint8_t)AKM_CUSTOM_NUM_FORM);

    if (nvSz == 0U) {
        nv_data = NULL;
        fret = AKM_ERROR;
        goto SAVE_QUIT;
    }

    nv_data = (uint8_t *)malloc((size_t)nvSz);

    /* AKL can accept NULL pointer for nv_data */
    fret = AKL_StopMeasurement(prm, nv_data);

    if (fret != AKM_SUCCESS) {
        goto SAVE_QUIT;
    }

    /* store data to storage */
    if (nv_data != NULL) {
        fret = AKH_SaveParameter(nv_data, nvSz);
    }

SAVE_QUIT:

    if (nv_data != NULL) {
        free(nv_data);
    }

    return fret;
}

int main(void)
{
    int16_t fret;
    struct  AKL_SCL_PRMS *prm = NULL;
    int32_t test_result;
    Ticker  one_milli_timer;

    /* Initialize hardware */
    fret = hw_init();

    if (fret != AKM_SUCCESS) {
        AKH_Print("hw_init failed (%d)\n", fret);
        goto MAIN_QUIT;
    }

    /* setup 1 millisecond timer */
    one_milli_timer.attach(&interrupt, milliseconds(1));

    /* Execute self test */
    fret = AKS_SelfTest(AKM_ST_MAG, &test_result);

    if (fret != AKM_SUCCESS) {
        AKH_Print("Test failed with code: 0x%08X\n", test_result);
        goto MAIN_QUIT;
    }

    /* Initialize AKM library. */
    fret = library_init(&prm);

    if (fret != AKM_SUCCESS) {
        AKH_Print("library_init failed (%d)\n", fret);
        goto MAIN_QUIT;
    }

    print_version();
    AKH_Print("Ready to measure.\n");

    while (1) {
#ifdef STOP_ON_START
        /* Process user input command */
        while (AKH_CheckUserInput(NULL) == AKM_ERR_TIMEOUT) {
            AKH_DelayMilli(250U);
        }
#endif
        /* reset sensor */
        AKH_Reset(AKM_ST_ALL_SENSORS);

        /* library prepare */
        fret = load_and_start(prm);

        if (fret != AKM_SUCCESS) {
            AKH_Print("load_and_start failed.");
            break;
        }

        /* start loop */
        fret = measurement_loop(prm);

        if (fret != AKM_SUCCESS) {
            AKH_Print("looper failed.");
            break;
        }

        /* finish measurement */
        fret = stop_and_save(prm);

        if (fret != AKM_SUCCESS) {
            AKH_Print("stop_and_save failed.");
            break;
        }
    }

MAIN_QUIT:

    if (prm != NULL) {
        free(prm);
    }

    /* Farewell message */
    AKH_Print("\nBye (%d).\n", fret);

    return 0;
}
