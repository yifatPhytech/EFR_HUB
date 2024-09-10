#ifndef RF_PARSER
#define RF_PARSER

bool ParseSensorMsg();
bool ParseLoggerMsg();
void ResetAfterReadRow();
bool IsNewRxData();

#endif //RF_PARSER
