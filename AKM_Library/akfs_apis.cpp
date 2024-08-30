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
#include "AKL_APIs.h"     /* API decleration */
#include "akfs_compass.h" /* Definition of struct */
#include "akfs_measure.h"
#include "AKH_APIs.h"


/*! Identify the nv data. */
#define AKL_NV_MAGIC_NUMBER  (uint32_t)(0xcafecafe)
/*! 1G (= 9.8 m/s^2) in Q16 format. i.e. (9.80665f * 65536) */
#define ACC_1G_IN_Q16        (642689)

/*! Convert from Q16 to floating format. */
#define Q16_TO_FLOAT(x)  (AKFLOAT)((x) / 65536.0f)
/*! Convert from floating format to Q16. */
#define FLOAT_TO_Q16(x)  (int32_t)((x) * 65536)

/*! Convert from AKSC to SI unit (m/s^2) in Q16 format. */
#define ACC_CONVERT_TO_Q16(x)    (int32_t)(((x) * ACC_1G_IN_Q16) / 720)
/*! Convert from SI unit (m/s^2) to AKSC format. */
#define ACC_CONVERT_FROM_Q16(x)  (int32_t)(((x) * 720) / ACC_1G_IN_Q16)

/******************************************************************************/
/***** AKM static functions ***************************************************/
static uint32_t byte_allign(const uint32_t sz)
{
    if (0 >= sz) {
        return (uint16_t)0;
    }

    /* Another method.
     int32_t rem = sz % 4;
     return (rem ? (sz + (4 - rem)) : (sz));
     */
    return ((sz & 0x3) ? ((sz & ~(0x3)) + 0x4) : sz);
}

/*****************************************************************************/
static int16_t akl_setv_mag(
    struct AKL_SCL_PRMS          *mem,
    const struct AKM_SENSOR_DATA *data)
{
    AKFLOAT mag[3];
    int     i;
#ifdef AKL_ARGUMENT_CHECK
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (data == NULL) {
        return AKM_ERR_INVALID_ARG;
    }
#endif

    for (i = 0; i < 3; i++) {
        mag[i] = Q16_TO_FLOAT(data->u.v[i]);
    }

    return AKFS_Set_MAGNETIC_FIELD(mem, mag, data->timestamp);
}

/**************************************/
static int16_t akl_setv_acc(
    struct AKL_SCL_PRMS          *mem,
    const struct AKM_SENSOR_DATA *data)
{
    AKFLOAT acc[3];
    int     i;
#ifdef AKL_ARGUMENT_CHECK
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (data == NULL) {
        return AKM_ERR_INVALID_ARG;
    }
#endif

    for (i = 0; i < 3; i++) {
        acc[i] = Q16_TO_FLOAT(data->u.v[i]);
    }

    return AKFS_Set_ACCELEROMETER(mem, acc, data->timestamp);
}

/**************************************/
static int16_t akl_setv_gyr(
    struct AKL_SCL_PRMS          *mem,
    const struct AKM_SENSOR_DATA *data)
{
    return AKM_ERR_NOT_SUPPORT;
}

/*****************************************************************************/
static int16_t akl_getv_mag(
    const struct AKL_SCL_PRMS *mem,
    int32_t                   data[6],
    int32_t                   *status,
    AKM_TIMESTAMP             *timestamp)
{
    int i;
#ifdef AKL_ARGUMENT_CHECK
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (data == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (status == NULL) {
        return AKM_ERR_INVALID_ARG;
    }
#endif

    /* Convert from SmartCompass to Q16 */
    for (i = 0; i < 3; i++) {
        data[i] = FLOAT_TO_Q16(mem->fv_hvec.v[i]);
        data[i + 3] = FLOAT_TO_Q16(mem->fv_ho.v[i]);
    }

    *status = (int32_t)mem->i16_hstatus;
    *timestamp = mem->m_ts_hvec;

    return AKM_SUCCESS;
}

/**************************************/
static int16_t akl_getv_acc(
    const struct AKL_SCL_PRMS *mem,
    int32_t                   data[3],
    int32_t                   *status,
    AKM_TIMESTAMP             *timestamp)
{
    int i;
#ifdef AKL_ARGUMENT_CHECK
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (data == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (status == NULL) {
        return AKM_ERR_INVALID_ARG;
    }
#endif

    for (i = 0; i < 3; i++) {
        data[i] = FLOAT_TO_Q16(mem->fv_avec.v[i]);
    }

    *status = (int32_t)(3);
    *timestamp = mem->m_ts_avec;

    return AKM_SUCCESS;
}

/**************************************/
static int16_t akl_getv_gyr(
    const struct AKL_SCL_PRMS *mem,
    int32_t                   data[6],
    int32_t                   *status,
    AKM_TIMESTAMP             *timestamp)
{
    return AKM_ERR_NOT_SUPPORT;
}

/**************************************/
static int16_t akl_getv_ori(
    const struct AKL_SCL_PRMS *mem,
    int32_t                   data[3],
    int32_t                   *status,
    AKM_TIMESTAMP             *timestamp)
{
    /* Convert unit */
    /* from Q6 to Q16 */
    data[0] = FLOAT_TO_Q16(mem->f_azimuth);
    data[1] = FLOAT_TO_Q16(mem->f_pitch);
    data[2] = FLOAT_TO_Q16(mem->f_roll);

    *status = (int32_t)(3);

    /* TODO: hvec or avec, which one is much better? */
    *timestamp = mem->m_ts_avec;

    return AKM_SUCCESS;
}

/**************************************/
int16_t akl_getv_quat(
    const struct AKL_SCL_PRMS *mem,
    int32_t                   data[4],
    int32_t                   *status,
    AKM_TIMESTAMP             *timestamp)
{
    return AKM_ERR_NOT_SUPPORT;
}

/******************************************************************************/
/***** AKM public APIs ********************************************************/
/***** Function manual is described in header file. ***************************/
uint32_t AKL_GetParameterSize(const uint8_t max_form)
{
    /* form is not used */
    return byte_allign(sizeof(struct AKL_SCL_PRMS));
}

/*****************************************************************************/
uint32_t AKL_GetNVdataSize(const uint8_t max_form)
{
    /* form is not used */
    return byte_allign(sizeof(struct AKL_NV_PRMS));
}

/*****************************************************************************/
int16_t AKL_Init(
    struct AKL_SCL_PRMS                 *mem,
    const struct AKL_CERTIFICATION_INFO *cert,
    const uint8_t                       max_form,
    AKM_DEVICES                         device)
{
#ifdef AKL_ARGUMENT_CHECK
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }
    /* "cert" can be NULL, because this parameter is used
     * in genuine library.
     */
#endif
    /* Clear all data. */
    AKFS_InitPRMS(mem);

    /* Set NV data pointer */
    mem->ps_nv = (struct AKL_NV_PRMS *)(
            (long)mem
            + byte_allign(sizeof(struct AKL_SCL_PRMS)));

    return AKM_SUCCESS;
}

/*****************************************************************************/
int16_t AKL_StartMeasurement(
    struct AKL_SCL_PRMS *mem,
    uint8_t             *nv_data)
{
    struct AKL_NV_PRMS *p_nv;
    struct AKL_NV_PRMS *p_pr;

#ifdef AKL_ARGUMENT_CHECK
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }
#endif
    p_nv = (struct AKL_NV_PRMS *)nv_data;
    p_pr = mem->ps_nv;

    if (p_nv == NULL) {
        /* If parameters couldn't be got, set default value. */
        AKFS_SetDefaultNV(p_pr);
    } else {
        if (p_nv->magic == AKL_NV_MAGIC_NUMBER) {
            /* Copy NV data to mem struct. */
            *p_pr = *p_nv;
        } else {
            AKFS_SetDefaultNV(p_pr);
        }
    }

    /* Init library functions. */
    return AKFS_InitMeasure(mem);
}

/*****************************************************************************/
int16_t AKL_StopMeasurement(
    const struct AKL_SCL_PRMS *mem,
    uint8_t                   *nv_data)
{
    struct AKL_NV_PRMS *p_nv;
    struct AKL_NV_PRMS *p_pr;

#ifdef AKL_ARGUMENT_CHECK
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }
#endif
    p_nv = (struct AKL_NV_PRMS *)nv_data;
    p_pr = mem->ps_nv;

    if (p_nv != NULL) {
        /* Copy mem data to NV buffer. */
        *p_nv = *p_pr;
        p_nv->magic = AKL_NV_MAGIC_NUMBER;
    }

    return AKM_SUCCESS;
}

/*****************************************************************************/
int16_t AKL_SetVector(
    struct AKL_SCL_PRMS          *mem,
    const struct AKM_SENSOR_DATA data[],
    const uint8_t                num)
{
    uint8_t i;
    int16_t ret;

#ifdef AKL_ARGUMENT_CHECK
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (data == NULL) {
        return AKM_ERR_INVALID_ARG;
    }
#endif

    for (i = 0; i < num; i++) {
        switch (data[i].stype) {
        case AKM_ST_MAG:
            ret = akl_setv_mag(mem, &data[i]);
            break;

        case AKM_ST_ACC:
            ret = akl_setv_acc(mem, &data[i]);
            break;

        case AKM_ST_GYR:
            ret = akl_setv_gyr(mem, &data[i]);
            break;

        default:
            ret = AKM_ERR_NOT_SUPPORT;
            break;
        }

        if (ret != AKM_SUCCESS) {
             AKH_Print("AKL_SetVector: Error in setting vector at index %d\n", i);
            return ret;
        }
    }

    return AKM_SUCCESS;
}

/*****************************************************************************/
int16_t AKL_CalcFusion(struct AKL_SCL_PRMS *mem)
{
    int16_t ret;
#ifdef AKL_ARGUMENT_CHECK
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }
#endif
    /* Azimuth calculation */
    /* hvbuf[in] : Android coordinate, sensitivity adjusted, */
    /*             offset subtracted. */
    /* avbuf[in] : Android coordinate, sensitivity adjusted, */
    /*             offset subtracted. */
    /* azimuth[out]: Android coordinate and unit (degree). */
    /* pitch  [out]: Android coordinate and unit (degree). */
    /* roll   [out]: Android coordinate and unit (degree). */
    ret = AKFS_Direction(
            AKFS_HDATA_SIZE,
            mem->fva_hvbuf,
            AKFS_HNAVE_D,
            AKFS_ADATA_SIZE,
            mem->fva_avbuf,
            AKFS_ANAVE_D,
            &mem->f_azimuth,
            &mem->f_pitch,
            &mem->f_roll
        );

    if (ret == AKFS_ERROR) {
        return AKM_ERROR;
    }

    return AKM_SUCCESS;
}

/*****************************************************************************/
int16_t AKL_GetVector(
    const AKM_VECTOR_TYPE       vtype,
    const struct AKL_SCL_PRMS   *mem,
    int32_t                     *data,
    uint8_t                     size,
    int32_t                     *status,
    AKM_TIMESTAMP               *timestamp)
{
    // 포인터 NULL 체크 추가
    if (mem == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (data == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (status == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    if (timestamp == NULL) {
        return AKM_ERR_INVALID_ARG;
    }

    // 벡터 타입에 따른 크기 체크
    switch (vtype) {
    case AKM_VT_MAG:
        if (AKM_VT_MAG_SIZE > size) {
            return AKM_ERR_INVALID_ARG;
        }
        return akl_getv_mag(mem, data, status, timestamp);

    case AKM_VT_ACC:
        if (AKM_VT_ACC_SIZE > size) {
            return AKM_ERR_INVALID_ARG;
        }
        return akl_getv_acc(mem, data, status, timestamp);

    case AKM_VT_GYR:
        if (AKM_VT_GYR_SIZE > size) {
            return AKM_ERR_INVALID_ARG;
        }
        return akl_getv_gyr(mem, data, status, timestamp);

    case AKM_VT_ORI:
        if (AKM_VT_ORI_SIZE > size) {
            return AKM_ERR_INVALID_ARG;
        }
        return akl_getv_ori(mem, data, status, timestamp);

    case AKM_VT_QUAT:
        if (AKM_VT_QUAT_SIZE > size) {
            return AKM_ERR_INVALID_ARG;
        }
        return akl_getv_quat(mem, data, status, timestamp);

    default:
        return AKM_ERR_NOT_SUPPORT;
    }
}
/*****************************************************************************/
void AKL_GetLibraryInfo(struct AKL_LIBRARY_INFO *info)
{
    info->algocode = 0;
    info->major = 0;
    info->minor = 0;
    info->variation = 0;
    info->revision = 0;
    info->datecode = 0;
}

/*****************************************************************************/
void AKL_ForceReCalibration(struct AKL_SCL_PRMS *mem)
{
    mem->i16_hstatus = 0;
}

/*****************************************************************************/
int16_t AKL_ChangeFormation(
    struct AKL_SCL_PRMS *mem,
    const uint8_t       formNumber)
{
    return AKM_ERR_NOT_SUPPORT;
}

/*****************************************************************************/
int16_t AKL_SetPDC(
    const struct AKL_SCL_PRMS *mem,
    const uint8_t             pdc[AKL_PDC_SIZE],
    const uint8_t             formNumber)
{
    return AKM_ERR_NOT_SUPPORT;
}
