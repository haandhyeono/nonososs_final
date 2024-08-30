#include "AKH_APIs.h"
#include "AKL_APIs.h"
#include "AKM_CustomerSpec.h"
#include "AKS_APIs.h"
#include "looper.h"
#include "print_util.h"
#include "akh_i2c.h"

#define GET_VEC_BUF_SIZE   6U
#define STATISTICS
#define PRINT_INTERVAL_MS  200U

#define FREQ_TO_INTERVAL_US(f)  (1000000 / (f))
#define FREQ_TO_INTERVAL_MS(f)  (1000 / (f))

/*! Timer tick counter.
 * Timer callback function ('interrupt') increments this variable
 * every 1 millisecond. This counter is set to 0 at start up.
 * This variable reset at the beginning of measurement loop.*/
static uint32_t tm_event;
/*! Magnetic event counter.
 * Timer callback function ('interrupt') increments this variable
 * every 1 millisecond.
 * This counter is reset when new data is obtained. */
static uint32_t mag_event;

static uint32_t acc_event;

static uint32_t gyr_event;

static uint32_t print_event;

static uint32_t stop_count = 0; // 

// UART 입력과 관련된 전역 변수 선언
extern volatile bool inputReady;  // UART 입력 완료 플래그
extern char inputBuffer[128];     // UART 입력 버퍼

/*!
 * \brief 
 *
 * \param prm
 *
 * \return 
 */
int16_t measurement_loop(struct AKL_SCL_PRMS *prm)
{
    int16_t fret;
    uint32_t interval_mag_ms;
    uint32_t interval_acc_ms;
    uint32_t interval_gyr_ms;
    uint16_t calc_flag;
    uint16_t isContinue = 1;
    int32_t data[GET_VEC_BUF_SIZE];
    int32_t st;
    uint32_t ch_count = 0;

    AKM_TIMESTAMP ts;
#ifdef STATISTICS
    uint32_t st_mag;
    uint32_t st_acc;
    uint32_t st_gyr;
#endif
    struct AKM_SENSOR_DATA sd_mag;
    struct AKM_SENSOR_DATA sd_acc;
    struct AKM_SENSOR_DATA sd_gyr;
    int32_t gyro_data[3];

    /* calculate interval from frequency */
    interval_mag_ms = (uint32_t)FREQ_TO_INTERVAL_MS(AKM_CUSTOM_MAG_FREQ);
  

    /* Start magnetometer with continuous measurement mode */
    if ((fret = AKS_Start(AKM_ST_MAG, FREQ_TO_INTERVAL_US(AKM_CUSTOM_MAG_FREQ))) != AKM_SUCCESS) {
        return fret;
    }

    /* Initialize event counters */
    tm_event = 0U;
    mag_event = 0U;
    acc_event = 0U;
    gyr_event = 0U;
    print_event = 0U;

#ifdef STATISTICS
    st_mag = 0U;
    st_acc = 0U;
    st_gyr = 0U;
#endif

    /* Reset calculation flag */
    calc_flag = 0U;

    /* Measurement loop */
    while (isContinue != 0U) {

        readInput();  
        processInput();  
        
        /* Take measurement when the measurement flag is true */
        if (mag_event >= interval_mag_ms) {
            uint8_t num = 1U;

            /* Reset the flag */
            mag_event = 0U;
            sd_mag.u.v[0] = 0;
            sd_mag.u.v[1] = 0;
            sd_mag.u.v[2] = 0;

            AKS_GetData(AKM_ST_MAG, &sd_mag, &num);
        }

        /* Calculate fusion if needed */
        if (calc_flag != 0U) {
            fret = AKL_CalcFusion(prm);

            if (fret != AKM_SUCCESS) {
                /* Handle error if needed */
            }

            calc_flag = 0U;
        }

        /* Output result periodically */
        if (print_event >= PRINT_INTERVAL_MS) {
            print_event = 0U;
#ifdef STATISTICS
          
#endif
            // Example for printing sensor data
            // print_data(AKM_ST_GYR, gyro_data, 0, ts);
        } else {
            /* Wait for next timer event */
            AKH_DelayMilli(1);
        }
    }

    /* Stop magnetometer */
    AKS_Stop(AKM_ST_MAG);

    return fret;
}


/*!
 * \brief This is timer callback function.
 * This function is called every 1 millisecond to count the current time.
 */
void interrupt(void)
{
    tm_event++;
    mag_event++;
    acc_event++;
    gyr_event++;
    print_event++;
}
