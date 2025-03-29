# MS5611-STM32-SPI

# MS5611 SPI Driver for STM32

A lightweight driver for the MS5611 barometric pressure and temperature sensor, designed for STM32 microcontrollers using the HAL SPI library. This library provides simple functions to read calibrated temperature and pressure data from the MS5611 sensor over SPI.

---

## Features
- **SPI Communication**: Full support for hardware SPI.
- **Factory Calibration**: Automatically reads PROM calibration data.
- **Oversampling Control**: Adjustable oversampling rates for precision/speed trade-offs.
- **Temperature Compensation**: Second-order compensation for accurate readings below 2000°C.
- **STM32 HAL Compatible**: Built for STM32CubeIDE/HAL libraries.

---

## Hardware Setup
### Wiring
| MS5611 Pin | STM32 Pin       |
|------------|-----------------|
| VCC        | 3.3V           |
| GND        | GND            |
| SCL (CLK)  | SPI Clock (SCK) |
| SDA (MOSI) | SPI MOSI        |
| CS         | GPIO (e.g., PA4)|

- Ensure the PS pin on the MS5611 is grounded for SPI mode.
- Use 3.3V logic level (MS5611 is not 5V tolerant!).

---

## Installation
1. **Add Files to Project**:
   - Copy `ms5611.c` and `ms5611.h` to your project's `Core/Src` and `Core/Inc` folders.
2. **Include Header**:
   ```c
   #include "ms5611.h"
3. Configure SPI:
1. Use STM32CubeMX to enable SPI in full-duplex master mode.
2. Set the correct clock speed (≤ 10 MHz for MS5611).
   
## Usage
1. Initialize the Sensor:
   ```c
   MS5611 sensor;
   SPI_HandleTypeDef hspi2; 
     int main(void) 
     {
       // STM32 HAL initialization here
       ms5611_init(&sensor, &hspi2, GPIOA, GPIO_PIN_4); // SPI, CS Port, CS Pin
     }

2. Read Data:
    ```c  
        // Read with OSR_4096 (9.1ms conversion time)
     get_pressure_temperature(&sensor, OSR_4096);
     
     // Access results
     float temperature = sensor.temperature; // °C
     float pressure = sensor.pressure;       // Pa

## Example Code:

#include "ms5611.h"
MS5611 sensor;
     
      ```c
      
                        #include "ms5611.h"
                        MS5611 sensor;      
                        int main(void) 
                        {
                              HAL_Init();
                              SystemClock_Config();
                              MX_SPI2_Init(); // Your SPI init function
                        
                              ms5611_init(&sensor, &hspi2, GPIOA, GPIO_PIN_4);
                        
                          while (1) 
                             {
                               get_pressure_temperature(&sensor, OSR_4096);
                               printf("Temp: %.2f°C\tPressure: %.2f mBar\n", sensor.temperature, sensor.pressure);
                               HAL_Delay(100);
                             }
                       }

## Configuration Options
Oversampling Rates
Enum	   | Conversion | Time	Resolution
OSR_256	|  0.6 ms	 |   Low
OSR_4096	|  9.1 ms	 |   Ultra High
     
      ```c
      
         get_pressure_temperature(&sensor, OSR_2048); // Balance speed/accuracy

## Troubleshooting
Garbage Values:

1. Verify SPI clock polarity/phase (Mode 0 for MS5611).

2. Check CS pin initialization and wiring.

3. Zero Calibration Data:

4. Ensure the sensor's reset command succeeds (10ms delay after init).

SPI Errors:

1. Confirm the SPI peripheral is enabled in CubeMX.

2. Use a logic analyzer to probe MOSI/CLK/CS signals.

