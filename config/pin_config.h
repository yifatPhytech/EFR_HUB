#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// $[CMU]
// [CMU]$

// $[LFXO]
// [LFXO]$

// $[PRS.ASYNCH0]
// [PRS.ASYNCH0]$

// $[PRS.ASYNCH1]
// [PRS.ASYNCH1]$

// $[PRS.ASYNCH2]
// [PRS.ASYNCH2]$

// $[PRS.ASYNCH3]
// [PRS.ASYNCH3]$

// $[PRS.ASYNCH4]
// [PRS.ASYNCH4]$

// $[PRS.ASYNCH5]
// [PRS.ASYNCH5]$

// $[PRS.ASYNCH6]
// [PRS.ASYNCH6]$

// $[PRS.ASYNCH7]
// [PRS.ASYNCH7]$

// $[PRS.ASYNCH8]
// [PRS.ASYNCH8]$

// $[PRS.ASYNCH9]
// [PRS.ASYNCH9]$

// $[PRS.ASYNCH10]
// [PRS.ASYNCH10]$

// $[PRS.ASYNCH11]
// [PRS.ASYNCH11]$

// $[PRS.SYNCH0]
// [PRS.SYNCH0]$

// $[PRS.SYNCH1]
// [PRS.SYNCH1]$

// $[PRS.SYNCH2]
// [PRS.SYNCH2]$

// $[PRS.SYNCH3]
// [PRS.SYNCH3]$

// $[GPIO]
// [GPIO]$

// $[TIMER0]
// [TIMER0]$

// $[TIMER1]
// [TIMER1]$

// $[TIMER2]
// [TIMER2]$

// $[TIMER3]
// [TIMER3]$

// $[TIMER4]
// [TIMER4]$

// $[USART0]
// [USART0]$

// $[I2C1]
// [I2C1]$

// $[EUSART1]
// EUSART1 RX on PA10
#ifndef EUSART1_RX_PORT                         
#define EUSART1_RX_PORT                          gpioPortA
#endif
#ifndef EUSART1_RX_PIN                          
#define EUSART1_RX_PIN                           10
#endif

// EUSART1 TX on PA09
#ifndef EUSART1_TX_PORT                         
#define EUSART1_TX_PORT                          gpioPortA
#endif
#ifndef EUSART1_TX_PIN                          
#define EUSART1_TX_PIN                           9
#endif

// [EUSART1]$

// $[EUSART2]
// [EUSART2]$

// $[LCD]
// [LCD]$

// $[KEYSCAN]
// [KEYSCAN]$

// $[LETIMER0]
// [LETIMER0]$

// $[IADC0]
// [IADC0]$

// $[ACMP0]
// [ACMP0]$

// $[ACMP1]
// [ACMP1]$

// $[VDAC0]
// [VDAC0]$

// $[PCNT0]
// [PCNT0]$

// $[LESENSE]
// [LESENSE]$

// $[HFXO0]
// [HFXO0]$

// $[I2C0]
// I2C0 SCL on PA08
#ifndef I2C0_SCL_PORT                           
#define I2C0_SCL_PORT                            gpioPortA
#endif
#ifndef I2C0_SCL_PIN                            
#define I2C0_SCL_PIN                             8
#endif

// I2C0 SDA on PA07
#ifndef I2C0_SDA_PORT                           
#define I2C0_SDA_PORT                            gpioPortA
#endif
#ifndef I2C0_SDA_PIN                            
#define I2C0_SDA_PIN                             7
#endif

// [I2C0]$

// $[EUSART0]
// [EUSART0]$

// $[PTI]
// [PTI]$

// $[MODEM]
// [MODEM]$

// $[CUSTOM_PIN_NAME]
#ifndef EFR_PA5_BATT_MONITOR_ENA_PORT           
#define EFR_PA5_BATT_MONITOR_ENA_PORT            gpioPortA
#endif
#ifndef EFR_PA5_BATT_MONITOR_ENA_PIN            
#define EFR_PA5_BATT_MONITOR_ENA_PIN             5
#endif

#ifndef EFR_PA6_EXTBATT_MONITOR_ENA_PORT        
#define EFR_PA6_EXTBATT_MONITOR_ENA_PORT         gpioPortA
#endif
#ifndef EFR_PA6_EXTBATT_MONITOR_ENA_PIN         
#define EFR_PA6_EXTBATT_MONITOR_ENA_PIN          6
#endif

#ifndef ON_OFF_PORT                             
#define ON_OFF_PORT                              gpioPortC
#endif
#ifndef ON_OFF_PIN                              
#define ON_OFF_PIN                               4
#endif

#ifndef BLE_PORT                                
#define BLE_PORT                                 gpioPortC
#endif
#ifndef BLE_PIN                                 
#define BLE_PIN                                  5
#endif

// [CUSTOM_PIN_NAME]$

#endif // PIN_CONFIG_H

