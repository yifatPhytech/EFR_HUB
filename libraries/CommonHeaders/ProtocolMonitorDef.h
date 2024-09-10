
#ifndef PROTOCOL_MNTR_DEF_H
#define PROTOCOL_MNTR_DEF_H

#pragma pack (1)
typedef struct _ProtocolMntrHeader
{
	uint8_t m_Header;
	uint8_t	m_size;
	uint8_t	m_buffer[16];
} ProtocolMntrHeader;
/*
//sensor<-->monitor protocol
//0xA1 / 0xA6
typedef struct _PayloadStage1
{
	uint8_t		m_version[4];
	uint16_t	m_battery;
	uint16_t	m_rssi;
	uint64_t	m_MacAdr;
} PayloadStage1;
*/
typedef struct _Payload_Stage_1
{

	uint8_t		m_version[4];
	uint16_t	m_battery;
	uint16_t	m_rssi;
	uint64_t	m_MacAdr;
} Payload_Stage_1;
//0xA2
typedef struct _PayloadAckStage1
{
	uint16_t	m_batteryEcho;
	uint8_t		m_type;
} PayloadAckStage1;

//0xA3
typedef struct _PayloadStage2
{
//	uint8_t 	m_Header;
//	uint8_t		m_size;
	int16_t		m_data;
	uint16_t	m_typeEcho;
} PayloadStage2;

//0xA4
typedef struct _PayloadAckStage2
{
	uint32_t 	m_ID;
	uint8_t		m_type;
	int16_t		m_clbr;
} PayloadAckStage2;

//0xA5
typedef struct _PayloadStage3
{
//	uint8_t 	m_Header;
//	uint8_t		m_size;
	uint32_t	m_IDEcho;
	uint8_t		m_type;
} PayloadStage3;

//hub<-->monitor protocol
//0xA7
typedef struct _PayloadAckStage_1
{
	uint32_t	m_ID;
	uint16_t	m_batteryEcho;
} PayloadAckStage_1;

//0xA8
typedef struct _PayloadStage_2
{
	uint32_t 	m_ID;
} PayloadStage_2;

typedef struct _PayloadAckSwUpd
{
	uint8_t 	m_index;
	uint8_t		m_ACK;
} PayloadAckSwUpd;

typedef struct _Sen_Monitor_Msg
{
//	ProtocolMntrHeader Header;
	union
	{
		/*PayloadStage1*/Payload_Stage_1		stage1Payload;
		PayloadAckStage1	stage1AckPayload;
		PayloadStage2		stage2Payload;
		PayloadAckStage2	stage2AckPayload;
		PayloadStage3		stage3Payload;
		PayloadAckSwUpd		AckUpdate;
	};
//	uint8_t 	m_Header;
//	uint8_t		m_size;
}Sen_Monitor_Msg;

typedef struct _Hub_Monitor_Msg
{
	ProtocolMntrHeader Header;
	union
	{
		Payload_Stage_1		stage1Payload;
		PayloadAckStage_1	stage1AckPayload;
		PayloadStage_2		stage2Payload;
	};
}Hub_Monitor_Msg;
#endif //PROTOCOL_MNTR_DEF_H
