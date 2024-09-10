/*
 * adc_reader.h
 *
 * Created on: Sep 26, 2023
 * Author: Itay
 *
 * IADC:
 * - Scan Mode is for automatically sampling multiple channels in sequence.
 * - Single Mode is for sampling just one channel at a time.
 *
 * How to use:
 *   uint32_t battery_voltage = readADCChannel(ADC0_SENS0_PC0_CHANNEL_0);
 *   battery_voltage = readADCChannel(ADC0_4TO20MA_PC1_CHANNEL_1);
 *   battery_voltage = readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);
 *   battery_voltage = readADCChannel(ADC0_EXTERNAL_BATT_PC3_CHANNEL_3);
 *
 *   or:
 *
 *   IADC_Result_t* adcResults = readAllADCChannelsInOneHit();
 *   // Extracting the results from the adcResults array.
 *   uint32_t SENS0 = (uint32_t)adcResults[0].data;
 *   uint32_t 4TO20MA = (uint32_t)adcResults[1].data;
 *   uint32_t INTERNAL_BATT = (uint32_t)adcResults[2].data;
 *   uint32_t EXTERNAL_BATT = (uint32_t)adcResults[3].data;
 *
 *   // Access the results for all channels
 *   for (uint8_t i = 0; i < TOTAL_ADC_CHANNELS; i++) {
 *     printf("Channel %u: Raw ADC Value = %lu\r\n", adcResults[i].id, (uint32_t)adcResults[i].data);
 *   }
 */

#ifndef LIBRARIES_106_ADC_106_ADC_READER_H_
#define LIBRARIES_106_ADC_106_ADC_READER_H_

#include <em_iadc.h>
#include <stdint.h>


// Total ADC channels
#define TOTAL_ADC_CHANNELS 4

// ADC channel definitions
#define ADC0_SENS0_PC0_CHANNEL_0                0
#define ADC0_4TO20MA_PC1_CHANNEL_1              1
#define ADC0_INTERNAL_BATT_PC2_CHANNEL_2        2
#define ADC0_EXTERNAL_BATT_PC3_CHANNEL_3        3
#define ADC0_EFRADIO_AVDD_CHANNEL               iadcPosInputAvdd

// GPIO definitions for battery monitoring
#define EFR_PA5_BATT_MONITOR_ENA_port           gpioPortA
#define EFR_PA5_BATT_MONITOR_ENA_pin            5

// GPIO definitions for external battery monitoring
#define EFR_PA6_EXTBATT_MONITOR_ENA_port        gpioPortA
#define EFR_PA6_EXTBATT_MONITOR_ENA_pin         6

// GPIO definitions for enabling/disabling external voltage source
#define EFR_PC8_EN_4_20_S0_port                 gpioPortC
#define EFR_PC8_EN_4_20_S0_pin                  8  // Forward

#define EFR_PC9_LRLY_S1_port                    gpioPortC
#define EFR_PC9_LRLY_S1_pin                     9  // Backward

// Function prototypes
void initADC(void);
uint32_t readADCChannel(uint8_t channel);
IADC_Result_t* readAllADCChannelsInOneHit(void);

#endif /* LIBRARIES_106_ADC_106_ADC_READER_H_ */
