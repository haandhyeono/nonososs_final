#include "mbed.h"
#include "AKH_APIs.h"

#ifdef AKH_USE_SPI
#include "akh_spi.h"
#else
#include "akh_i2c.h"
#endif

/* UART 정의 및 입력 처리 관련 변수 */
#ifdef ACSP_DEFINE
// ACSP 보드의 경우
BufferedSerial uart(PA_2, PA_3, 921600);
#else
// Arduino 센서 보드의 경우
BufferedSerial uart(USBTX, USBRX, 115200);
#endif

char inputBuffer[128]; // 입력 버퍼
volatile bool inputReady = false; // 입력이 완료되었음을 나타내는 플래그
volatile bool paused = false; // 프로그램이 일시 정지 상태인지 나타내는 플래그

/* Reset pin definition */
//DigitalOut rstn(PB_0);
void readInput() {
    char c;
    static int index = 0;

    while (uart.readable() && uart.read(&c, 1)) {
        if (c == '\n' || c == '\r') {
            inputBuffer[index] = '\0'; 
            inputReady = true; 
            index = 0; 
        } else {
            inputBuffer[index++] = c;
            if (index >= sizeof(inputBuffer) - 1) {
                index = sizeof(inputBuffer) - 1; 
            }
        }
    }
}


void processInput() {
    if (inputReady) {
        inputReady = false;

        if (strcmp(inputBuffer, "q") == 0) {
            paused = true;
            uart.write("Program paused. Press 'r' to resume.\n", 36);
            while (paused) {
                readInput();
                inputReady = false;
                if (strcmp(inputBuffer, "r") == 0 && paused)
                {
                    paused = false;
                    uart.write("Program resumed.\n", 18);
                    break;
                }
            }
        } 
        else if (strcmp(inputBuffer, "z") == 0) {
            AKH_Print("Exiting program...\n");
            exit(EXIT_SUCCESS);  
        }

        memset(inputBuffer, 0, sizeof(inputBuffer)); 
    }
}

/* user event interface */
#ifndef ACSP_DEFINE
InterruptIn user_btn(BUTTON1);  // Updated to BUTTON1 from USER_BUTTON
bool user_int;
#endif

/* interrupt based timestamp */
#ifdef ACSP_DEFINE
// In case of ACSP Board
InterruptIn mag_int(PC_3);
#else
// For Arduino Sensor Board
InterruptIn mag_int(D7);
InterruptIn acc_int(D5);
InterruptIn gyr_int(D3);
#endif

/* get timestamp */
Timer ts;

/* Heart beat LED */
Ticker tk;
#ifdef ACSP_DEFINE
DigitalOut led1(PA_0);
#else
DigitalOut led1(LED1);
#endif

void pressed()
{
#ifndef ACSP_DEFINE
    user_int = true;
#endif
}

void heart_beat() {
    led1 = !led1;
}

/******************************************************************************/
/***** AKH public APIs ********************************************************/
void AKH_Init(void)
{
    /* Reset-pin configuration */
    //rstn = 1;

    /* User input configuration */
#ifndef ACSP_DEFINE
    user_int = false;
    user_btn.fall(&pressed);
#endif

#ifndef ACSP_DEFINE
    /* drdy configuration */
    mag_int.disable_irq();
    acc_int.disable_irq();
    gyr_int.disable_irq();
#endif

    /* Initialize serial bus */
#ifdef AKH_USE_SPI
    AKH_SPI_Init();
#else
    AKH_I2C_Init();
#endif
    /* USART configuration */
    uart.write("Hello ACSP!!!\n", 14);

    /* start timer */
    ts.start();

    /* start heart beat */
    tk.attach(&heart_beat, 500ms);  // Updated to use chrono duration

    /* wait for settle down */
    ThisThread::sleep_for(100ms);  // Updated to use chrono duration
}

int16_t AKH_TxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t address,
    const uint8_t *data,
    const uint16_t numberOfBytesToWrite)
{
#ifdef AKH_USE_SPI
    return AKH_SPI_TxData(stype, address, data, numberOfBytesToWrite);
#else
    return AKH_I2C_TxData(stype, address, data, numberOfBytesToWrite);
#endif
}

int16_t AKH_RxData(
    const AKM_SENSOR_TYPE stype,
    const uint8_t address,
    uint8_t *data,
    const uint16_t numberOfBytesToRead)
{
#ifdef AKH_USE_SPI
    return AKH_SPI_RxData(stype, address, data, numberOfBytesToRead);
#else
    return AKH_I2C_RxData(stype, address, data, numberOfBytesToRead);
#endif
}

int16_t AKH_Reset(const AKM_SENSOR_TYPE stype)
{
    //rstn = 0;
    //wait_us(100);
    //rstn = 1;

    /* reset timer */
    ts.reset();

    return AKM_SUCCESS;
}

void AKH_DelayMicro(const uint16_t us)
{
    wait_us(us);
}

void AKH_DelayMilli(const uint16_t ms)
{
    ThisThread::sleep_for(std::chrono::milliseconds(ms));  // Updated to use chrono duration
}

int16_t AKH_SetIRQHandler(
    const AKH_IRQ stype,
    IRQ_CALLBACK func)
{
#ifndef ACSP_DEFINE
    switch (stype) {
    case IRQ_MAG_1:
        if (func == NULL) {
            mag_int.disable_irq();
        } else {
            mag_int.rise(func);
            mag_int.enable_irq();
        }
        break;

    case IRQ_ACC_1:
        if (func == NULL) {
            acc_int.disable_irq();
        } else {
            acc_int.rise(func);
            acc_int.enable_irq();
        }
        break;

    case IRQ_GYR_1:
        if (func == NULL) {
            gyr_int.disable_irq();
        } else {
            gyr_int.rise(func);
            gyr_int.enable_irq();
        }
        break;

    default:
        return AKM_ERR_NOT_SUPPORT;
    }
#endif

    return AKM_SUCCESS;
}

AKM_TIMESTAMP AKH_GetTimestamp(void)
{
#ifdef AKM_TIMESTAMP_NANOSECOND
    return (AKM_TIMESTAMP)((unsigned int)ts.elapsed_time().count() * (AKM_TIMESTAMP)1000);  // Updated to use chrono elapsed_time
#else
    return (AKM_TIMESTAMP)(0x7FFFFFFF & ts.elapsed_time().count());  // Updated to use chrono elapsed_time
#endif
}

int16_t AKH_SaveParameter(
    const uint8_t *param,
    const uint16_t nBytes)
{
    return AKM_ERR_NOT_SUPPORT;
}

int16_t AKH_LoadParameter(
    uint8_t *param,
    const uint16_t nBytes)
{
    return AKM_ERR_NOT_SUPPORT;
}

void AKH_Print(
    const char *format,
    ...)
{
    va_list va;
    char buf[1024];  // 버퍼 크기 확장

    va_start(va, format);
    vsnprintf(buf, sizeof(buf), format, va);
    va_end(va);

    uart.write(buf, strlen(buf));  // BufferedSerial write 사용
}

int16_t AKH_CheckUserInput(uint32_t *code)
{
#ifdef ACSP_DEFINE
    return AKM_ERR_TIMEOUT;
#else

    if (user_int) {
        user_int = false;
        return AKM_SUCCESS;
    }

    return AKM_ERR_TIMEOUT;
#endif
}
