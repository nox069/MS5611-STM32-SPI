#ifndef MS5611_SPI_H
#define MS5611_SPI_H

#include "stm32f4xx_hal.h" // Adjust for your STM32 series, e.g., stm32f1xx_hal.h
#include "main.h"
// MS5611 command set (from datasheet and Arduino reference)
#define MS5611_CMD_READ_ADC       0x00
#define MS5611_CMD_READ_PROM      0xA0
#define MS5611_CMD_RESET          0x1E
#define MS5611_CMD_CONVERT_D1     0x40
#define MS5611_CMD_CONVERT_D2     0x50

// Status and default values
#define MS5611_READ_OK            0
#define MS5611_NOT_READ           -999.0f

// Oversampling rate options
typedef enum {
    OSR_256  = 0x00,  // Conversion time 0.6ms
    OSR_512  = 0x02,  // 1.2ms
    OSR_1024 = 0x04,  // 2.3ms
    OSR_2048 = 0x06,  // 4.6ms
    OSR_4096 = 0x08   // 9.1ms (default)
} MS5611_OSR;

// Sensor structure
typedef struct {
    SPI_HandleTypeDef *hspi;    // SPI handle (e.g., &hspi2)
    GPIO_TypeDef *cs_port;      // Chip select GPIO port (e.g., GPIOA)
    uint16_t cs_pin;            // Chip select GPIO pin (e.g., GPIO_PIN_4)

    float temperature;          // Temperature in 0.01°C units
    float pressure;             // Pressure in 0.01 mBar units
    uint16_t  Calibration[8];                 // Calibration coefficients

    uint32_t D1_pressure;
    uint32_t D2_temperature;

    // Compensation values
    int32_t TEMP;
    int64_t OFF;
    int64_t SENS;
} MS5611;

void ms5611_init(MS5611 *ms5611, SPI_HandleTypeDef *SPI, GPIO_TypeDef *CS_Port, uint16_t CS_Pin);
void get_pressure_temperature(MS5611 *ms5611, MS5611_OSR osr);


#endif // MS5611_SPI_H
