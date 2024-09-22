#ifndef SENSOR_SM
#define SENSOR_SM

void SensorStateMachine();
void InitSensorSM();
bool IsSnsSmSleep();
bool ShouldListen();
void PrintCurrentSnsState();

#endif
