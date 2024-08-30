#include "AKH_APIs.h"
#include "AKL_APIs.h"
#include "AKM_CustomerSpec.h"
#include "AKS_APIs.h"
#include <math.h>
#include <stdio.h>

/*#define DEBUG*/
/*#define VERBOSE*/
#define PRINT_DETAIL

void print_version(void)
{
    AKH_Print("Debug: Entering print_version\n");

    struct AKS_DEVICE_INFO dev_info[4];
    struct AKL_LIBRARY_INFO lib_info;
    int16_t fret;
    uint8_t i;
    uint8_t num = 4;

    fret = AKS_GetDeviceInfo(AKM_ST_ALL_SENSORS, dev_info, &num);

    if (fret != AKM_SUCCESS) {
        AKH_Print("Woops! Device information couldn't be retrieved.\n");
    } else {
        for (i = 0; i < num; i++) {
            AKH_Print(" %s is detected.\n", dev_info[i].name);
        }
    }

    AKH_Print("\n");

    AKL_GetLibraryInfo(&lib_info);
    AKH_Print("AKM sample program for Non-OS package.\n");
    AKH_Print("  INFO: Algorithm code of Library: %d\n",
              lib_info.algocode);
    AKH_Print("  INFO: Version: %d.%d.%d.%d.%d\n",
              lib_info.major,
              lib_info.minor,
              lib_info.revision,
              lib_info.datecode,
              lib_info.variation);
#ifdef DEBUG
    AKH_Print("  DBG : AKM_CUSTOM_NUM_FORM=%d\n", AKM_CUSTOM_NUM_FORM);
    AKH_Print("  DBG : Parameter size=%d\n",
              AKL_GetParameterSize(AKM_CUSTOM_NUM_FORM));
    AKH_Print("  DBG : NV data size=%d\n",
              AKL_GetNVdataSize(AKM_CUSTOM_NUM_FORM));
#endif
    AKH_Print("\n");
}

void print_data(
    const AKM_SENSOR_TYPE stype,
    const int32_t data[6],
    const int32_t st,
    AKM_TIMESTAMP ts )
{
    AKH_Print("Debug: Entering print_data\n");

    if (data == NULL) {
        AKH_Print("Error: data pointer is NULL\n");
        return;
    }



    float fx, fy, fz;
    const char *name, *unit;
    int print_bias = 0;
    fx = data[0] / 65536.f;
    fy = data[1] / 65536.f;
    fz = data[2] / 65536.f;
    unit = "";

    switch (stype) {
    case AKM_ST_MAG:
        name = "MAG";
        unit = "uT";
        print_bias = 1;
        break;

    // case AKM_ST_ACC:
    //     name = "ACC";
    //     unit = "m/s^2";
    //     break;

    // case AKM_ST_GYR:
    //     name = "GYR";
    //     unit = "dps";
    //     print_bias = 1;
    //     break;

    default:
        name = "UNKNOWN";
        break;
    }

    AKH_Print("%.3f (%s) = %6.3f, %6.3f, %6.3f %s\n",
              (float)(ts / 1000.0),
              name,
              fx, fy, fz,
              unit);

#ifdef PRINT_DETAIL
    if (print_bias) {
        AKH_Print("Debug: Inside print_bias check\n");
            AKH_Print("  Bias(x,y,z) = %6.1f, %6.1f, %6.1f %s\n",
                      data[3] / 65536.f,
                      data[4] / 65536.f,
                      data[5] / 65536.f,
                      unit);
            AKH_Print("  R           = %6.1f %s\n",
                      sqrtf(fx * fx + fy * fy + fz * fz), unit);
        } else {
            AKH_Print("Error: data array size is less than expected for bias data\n");
    }
    

    AKH_Print("  Accuracy    = %d\n", st);
#endif
}

void print_ypr(
    const int32_t data[],
    AKM_TIMESTAMP ts)
{
    AKH_Print("%f (ypr) = %6.1f, %6.1f, %6.1f degree.\n",
              ts / 1000.0f,
              data[0] / 65536.f,
              data[1] / 65536.f,
              data[2] / 65536.f);
}


void print_quat(
    const int32_t quat[],
    AKM_TIMESTAMP ts)
{
    AKH_Print("Debug: Entering print_quat\n");

    if (quat == NULL) {
        AKH_Print("Error: quat pointer is NULL\n");
        return;
    }

    float q0 = quat[0] / 65536.f;
    float q1 = quat[1] / 65536.f;
    float q2 = quat[2] / 65536.f;
    float q3 = quat[3] / 65536.f;

    AKH_Print("Debug: q0=%.4f, q1=%.4f, q2=%.4f, q3=%.4f\n", q0, q1, q2, q3);
    AKH_Print("%.3f (quat) = %6.4f, %6.4f, %6.4f, %6.4f\n",
              (float)(ts / 1000.0),
              q0,
              q1,
              q2,
              q3);
}
