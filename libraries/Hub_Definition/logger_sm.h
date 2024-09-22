#ifndef LOGGER_SM
#define LOGGER_SM

void LoggerStateMachine();
void DefineRadio_PWR_LVL2LGR(uint8_t rssi);
bool IsLgrSmSleep();
void InitLoggerSM();
void PrintCurrentLgrState();

#endif
