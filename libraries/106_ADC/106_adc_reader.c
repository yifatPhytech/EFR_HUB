/*
 * adc_reader.c
 *
 *  Created on: Sep 26, 2023
 *      Author: itay
 */
#include <em_device.h>
#include <em_gpio.h>
#include <em_cmu.h>
#include <libraries/106_ADC/106_adc_reader.h>
#include <stdio.h>  // for printf function


#define IADC_INPUT_0_BUS          ABUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_ABUSALLOC_AODD0_ADC0

#define CLK_SRC_ADC_FREQ        40000000  // CLK_SRC_ADC - 40 MHz max
#define CLK_ADC_FREQ            10000000  // CLK_ADC - 10 MHz max in normal mode
#define HFXO_FREQ               38400000

#define VREF                    1210      // Reference voltage for ADC in millivolts
#define ADC_RESOLUTION          4095      // 2^12 - 1 (12bit)

//static void enableChannel(uint8_t channel);
//static void disableChannel(uint8_t channel);


void initADC(void)
{
  // Initialization structures for ADC configuration
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitScan_t initScan = IADC_INITSCAN_DEFAULT;
  IADC_ScanTable_t scanTable = IADC_SCANTABLE_DEFAULT;

  // Enable the clock for the IADC0 module
  CMU_ClockEnable(cmuClock_IADC0, true);

  // GPIO Configuration
  // Enable the clock for the GPIO module
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Enable 4-20mA sensor
  GPIO_PinModeSet(EFR_PC8_EN_4_20_S0_port, EFR_PC8_EN_4_20_S0_pin, gpioModePushPull, 1);

  // Configure PC0 to PC3 as analog inputs by disabling their digital functionality
  GPIO_PinModeSet(gpioPortC, 0, gpioModeDisabled, 0);  // Configure PC0 as analog input
  GPIO_PinModeSet(gpioPortC, 1, gpioModeDisabled, 0);  // Configure PC1 as analog input
  GPIO_PinModeSet(gpioPortC, 2, gpioModeDisabled, 0);  // Configure PC2 as analog input
  GPIO_PinModeSet(gpioPortC, 3, gpioModeDisabled, 0);  // Configure PC3 as analog input

  // Configure PA5 as push-pull for internal battery monitor voltage
  // GPIO_PinModeSet(EFR_PA5_BATT_MONITOR_ENA_port, EFR_PA5_BATT_MONITOR_ENA_pin, gpioModePushPull, 0);

  // Configure PA6 as push-pull for external battery monitor voltage
  GPIO_PinModeSet(EFR_PA6_EXTBATT_MONITOR_ENA_port, EFR_PA6_EXTBATT_MONITOR_ENA_pin, gpioModePushPull, 0);

  // Set the clock source for the ADC
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_EM01GRPACLK);

  // Configure ADC warm-up mode
  init.warmup = iadcWarmupNormal;             // the ADC's internal circuits are powered down when not in use.
  // init.warmup = iadcWarmupKeepWarm;        // the ADC's internal circuits remain powered even when not in use.

  // Calculate and set the source clock prescaler and timer cycles for the ADC
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);
  init.timerCycles = CMU_ClockFreqGet(cmuClock_IADCCLK)/1000;

  // Configure various ADC settings like reference voltage, oversampling, gain, etc.
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
  initAllConfigs.configs[0].vRef = VREF;
  initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed2x;
  initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain0P5x;
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0, CLK_ADC_FREQ, 0, iadcCfgModeNormal, init.srcClkPrescale);

  // Configure ADC scan mode settings
  initScan.triggerSelect = iadcTriggerSelTimer;
  initScan.dataValidLevel = iadcFifoCfgDvl1;
  initScan.showId = true;

  // Set up the scan table to read from PC0 to PC3
  for (int i = 0; i < 4; i++) {
      scanTable.entries[i].posInput = iadcPosInputPortCPin0 + i;
      scanTable.entries[i].negInput = iadcNegInputGnd;
      scanTable.entries[i].includeInScan = true;
  }

  // Initialize the ADC with the configurations set above
  IADC_init(IADC0, &init, &initAllConfigs);
  IADC_initScan(IADC0, &initScan, &scanTable);
  IADC_command(IADC0, iadcCmdEnableTimer);

  // Allocate the analog bus for ADC0 inputs
  GPIO->CDBUSALLOC |= (GPIO_CDBUSALLOC_CDEVEN0_ADC0 | GPIO_CDBUSALLOC_CDODD0_ADC0); // For even and odd-numbered pins of Port C and Port D
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;                                  // For internal and Port A analog routes
}

uint32_t readADCChannel(uint8_t channel)
{
  if (channel > 3) return 0; // Only channels 0 to 3 are valid

  IADC_Result_t sample;
  uint32_t vadc = 0;
  uint32_t vout = 0; // Output voltage, named vout to generalize vin and vbat
  uint32_t vref = VREF; // Default VREF

  // Start scan
  IADC_command(IADC0, iadcCmdStartScan);

  do
    {
      while (IADC_getScanFifoCnt(IADC0) == 0);         // Wait for the conversion to complete
      sample = IADC_pullScanFifoResult(IADC0);         // Read the scan result
    }
  while (sample.id != channel);                        // Continue reading until the result from the desired channel is obtained

  // Check the channel number and apply the appropriate formula
  if (channel == 0 || channel == 1)
    {
      vref = 1210; // VREF = 1210
      vadc = (vref * (uint32_t)sample.data) / ADC_RESOLUTION;     // vadc in millivolts
      vout = vadc;                                            // vbat in millivolts
    }
  else if (channel == 2)
    {
      vref = 1210; // VREF = 1210
      vadc = (vref * (uint32_t)sample.data) / ADC_RESOLUTION;     // vadc in millivolts
      vout = (uint32_t)((35 * vadc) / 13.0);                      // Corrected vin in millivolts
    }
  else if (channel == 3)
    {
      vref = 2500; // VREF = 1210
      vadc = (vref * (uint32_t)sample.data) / ADC_RESOLUTION;     // vadc in millivolts
      vout = (239 * vadc) / 39;                                   // vin in millivolts
    }

  // Print the results for debugging
  printf("Channel: %u, Raw ADC Value: %lu, vadc: %lu, vout: %lu\r\n", channel, (uint32_t)sample.data, vadc, vout);

  return vout;
}

IADC_Result_t* readAllADCChannelsInOneHit(void)
{
  static IADC_Result_t results[TOTAL_ADC_CHANNELS]; // Static array
  uint32_t vadc = 0;
  uint32_t vref = VREF; // Default VREF

  // Start scan
  IADC_command(IADC0, iadcCmdStartScan);

  for (uint8_t channel = 0; channel < TOTAL_ADC_CHANNELS; channel++)
    {
      while (IADC_getScanFifoCnt(IADC0) == 0);         // Wait for the conversion to complete
      results[channel] = IADC_pullScanFifoResult(IADC0); // Read the scan result and store directly in results array

      // Check the channel number and apply the appropriate formula
      if (results[channel].id == 0 || results[channel].id == 1)
        {
          vref = 1210; // VREF = 1210
          vadc = (vref * (uint32_t)results[channel].data) / ADC_RESOLUTION;     // vadc in millivolts
          results[channel].data = vadc;                                      // vbat in millivolts
        }
      else if (results[channel].id == 2)
        {
          vref = 1210; // VREF = 1210
          vadc = (vref * (uint32_t)results[channel].data) / ADC_RESOLUTION;     // vadc in millivolts
          results[channel].data = (uint32_t)((35 * vadc) / 13.0);                // Corrected vin in millivolts
        }
      else if (results[channel].id == 3)
        {
          vref = 2500; // VREF = 1210
          vadc = (vref * (uint32_t)results[channel].data) / ADC_RESOLUTION;     // vadc in millivolts
          results[channel].data = (239 * vadc) / 39;                             // vin in millivolts
        }

      // Print the results for debugging
      printf("Channel: %u, Raw ADC Value: %lu, vadc: %lu, vout: %lu\r\n", results[channel].id, (uint32_t)results[channel].data, vadc, results[channel].data);
    }

  return results;
}


//static void enableChannel(uint8_t channel)
//{
//  if (channel == 2) GPIO_PinOutClear(EFR_PA5_BATT_MONITOR_ENA_port, EFR_PA5_BATT_MONITOR_ENA_pin);       // enable CH2
//  else if (channel == 3) GPIO_PinOutClear(EFR_PA6_EXTBATT_MONITOR_ENA_port, EFR_PA6_EXTBATT_MONITOR_ENA_pin); // enable CH3
//}
//
//static void disableChannel(uint8_t channel)
//{
//  if (channel == 2) GPIO_PinOutSet(EFR_PA5_BATT_MONITOR_ENA_port, EFR_PA5_BATT_MONITOR_ENA_pin);              // disable CH2
//  else if (channel == 3) GPIO_PinOutSet(EFR_PA6_EXTBATT_MONITOR_ENA_port, EFR_PA6_EXTBATT_MONITOR_ENA_pin);   // disable CH3
//}
