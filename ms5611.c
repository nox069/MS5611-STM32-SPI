/*
 * ms5611.c
 *
 *  Created on: Mar 28, 2025
 *      Author: torik
 */
#include "ms5611.h"

/* Chip Select */
static void selectChip(MS5611 *ms5611)
{
    HAL_GPIO_WritePin(ms5611->cs_port, ms5611->cs_pin, GPIO_PIN_RESET);
}

static void deselectChip(MS5611 *ms5611)
{
    HAL_GPIO_WritePin(ms5611->cs_port, ms5611->cs_pin, GPIO_PIN_SET);
}


/* Read PROM */
static uint16_t read_prom(MS5611 *ms5611, uint8_t reg)
{
	uint8_t data[2] = {0};
	uint8_t regis = MS5611_CMD_READ_PROM + (reg * 2);

	selectChip(ms5611);
	HAL_SPI_Transmit(ms5611->hspi, &regis, 1, 100);
	HAL_SPI_Receive(ms5611->hspi, data, 2, 100);
	deselectChip(ms5611);

	return (data[0] << 8) | data[1];
}

/*Initialize MS5611 */
void ms5611_init(MS5611 *ms5611, SPI_HandleTypeDef *SPI, GPIO_TypeDef *CS_Port, uint16_t CS_Pin)
{
	ms5611->hspi = SPI;
	ms5611->cs_port = CS_Port;
	ms5611->cs_pin = CS_Pin;

	selectChip(ms5611);
	uint8_t reg = 0x1E;
	HAL_SPI_Transmit(ms5611->hspi, &reg, 1, HAL_MAX_DELAY); // Reset
	deselectChip(ms5611);
	HAL_Delay(10);

	for(uint8_t i = 0; i < 8; i++)
	{
		ms5611->Calibration[i] = read_prom(ms5611,i); // Read PROM
	}
	HAL_Delay(10);
}

/* Read ADC */
static uint32_t read_adc(MS5611 *ms5611)
{
    uint8_t cmd = MS5611_CMD_READ_ADC;
    uint8_t rx_buf[3] = {0};

    selectChip(ms5611);
    HAL_SPI_Transmit(ms5611->hspi, &cmd, 1, 100);
    HAL_SPI_Receive(ms5611->hspi, rx_buf, 3, 100);
    deselectChip(ms5611);

    return ((uint32_t)rx_buf[0] << 16) | ((uint32_t)rx_buf[1] << 8) | rx_buf[2];
}

/* Measurement Function */
void get_pressure_temperature(MS5611 *ms5611, MS5611_OSR osr)
{

    uint8_t cmd = MS5611_CMD_CONVERT_D1 | osr; // Getting Raw Pressure

    selectChip(ms5611);
    HAL_SPI_Transmit(ms5611->hspi, &cmd, 1, 100);
    deselectChip(ms5611);

    HAL_Delay(9.1);
    ms5611->D1_pressure = read_adc(ms5611);


    cmd = MS5611_CMD_CONVERT_D2 | osr; 		   // Getting Raw Temperature

    selectChip(ms5611);
    HAL_SPI_Transmit(ms5611->hspi, &cmd, 1, 100);
    deselectChip(ms5611);

    HAL_Delay(9.1);
    ms5611->D2_temperature = read_adc(ms5611);


    int32_t dT = ms5611->D2_temperature - ((int32_t)ms5611->Calibration[5] << 8); // Temperature Main Calculation
    ms5611->TEMP = 2000 + ((dT * (int64_t)ms5611->Calibration[6]) >> 23);
    // Calculate offset and sensitivity
    ms5611->OFF = ((int64_t)ms5611->Calibration[2] << 16) + (((int64_t)ms5611->Calibration[4] * dT) >> 7);
    ms5611->SENS = ((int64_t)ms5611->Calibration[1] << 15) + (((int64_t)ms5611->Calibration[3] * dT) >> 8);
    if(ms5611->TEMP < 2000)
    {
            int32_t T2 = (dT * dT) >> 31; //Second Order Compensation for Better Accuracy
            ms5611->TEMP -= T2;
            int32_t temp_diff = ms5611->TEMP - 2000;
            ms5611->OFF -= (61 * temp_diff * temp_diff) >> 4;
            ms5611->SENS -= 2 * temp_diff * temp_diff;
    }



    ms5611->temperature = ms5611->TEMP / 100.0f;		// Final calculations
    //ms5611->pressure = (((ms5611->D1_pressure * ms5611->SENS) >> 21) - ms5611->OFF) / (128.0f * 100.0f);
    ms5611->pressure = (((((int64_t)ms5611->D1_pressure * ms5611->SENS) >> 21) - ms5611->OFF) / 16384.0f);
}
