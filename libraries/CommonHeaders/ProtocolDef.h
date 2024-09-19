
#ifndef PROTOCOL_DEF_H
#define PROTOCOL_DEF_H
#include <stdint.h>

//#define MAX_PAYLOAD_LEN		48
#define MAX_PAYLOAD_LEN_NEW	64 - sizeof(PrtlHdr) - 1		//48
#define LENGTH_SNS_DATA_MSG	8
#define LENGTH_SNS_OFF_MSG	6
#define LENGTH_SNS_PRM_MSG	12
#define LENGTH_MULTY_SNS_DATA_MSG	30
#define MAX_HSTR_CNT	12//5

#pragma pack(push,1)
typedef struct _ProtocolHeader
{
	uint32_t 	m_ID;
	uint32_t 	m_addressee;
	uint8_t 	m_Header;
	uint8_t		m_size;
} PrtlHdr;

typedef struct _PayloadSenDataNHstr
{
	uint8_t		m_nDataCnt;
	int16_t		m_data[MAX_HSTR_CNT+1];
} PayloadSenData_n_Hstr;

typedef struct _PayloadMultiSenDataNHstr
{
	uint8_t		m_nDataCnt;
	uint8_t		m_nHstrIndex;
	int16_t		m_data[24];
} PayloadMultiSenData_n_Hstr;

//
typedef struct _PayloadSenPrmNew
{
	uint32_t	m_version;
	uint16_t	m_battery;
	uint8_t		m_type;
	uint8_t		m_slot;
} PayloadSenPrmNew;

typedef struct _PayloadSen2Lgr
{
	uint16_t	m_battery;
	uint8_t		m_type;
	uint8_t		m_nDataCnt;
	uint8_t		m_Gnrl1;
	uint8_t		m_Gnrl2;
	int16_t		m_data[20];
} PayloadSen2Lgr;


typedef struct _PayloadHubAckExt
{
	uint16_t		m_sec2cnct;
	uint8_t			m_slot;
	uint8_t 		m_indexEcho;
	uint8_t			m_sec4Burst;
	uint8_t			m_HeardRssi;
} PayloadHubAckExt;

typedef struct _PayloadHubDataNew
{
	uint8_t		m_data[MAX_PAYLOAD_LEN_NEW];
} PayloadHubDataNew;


#define HUB_DATA_CONST_LENGTH	3 //(sizeof m_battery + m_index)

//0xCB
typedef struct _PayloadRecAck
{
	uint8_t		m_slotMin;
	uint8_t		m_slotSec;
	uint8_t		m_min;
	uint8_t		m_sec;
} PayloadRecAck;

typedef struct _PayloadHubPrmsExt
{
	 float		m_lat;
	 float		m_lon;
	 uint32_t	m_version;
	 uint16_t	m_battery;
} PayloadHubPrmsExt;

typedef struct _Sen_Hub_Rec_Msg
{
	PrtlHdr Header;
	union
	{
		PayloadSenData_n_Hstr	DataHstPayload;
		PayloadMultiSenData_n_Hstr	MultiDataPayload;
		PayloadSenPrmNew	PrmPayload;
		PayloadHubAckExt	AckExtPayload;
		PayloadHubPrmsExt	HubPrmsExtPayload;
		PayloadRecAck		RecAckPayload;
		PayloadHubDataNew	HubDataPayload;
		PayloadSen2Lgr		SenLgrPayload;
	};
}Sen_Hub_Rec_Msg;

#pragma pack(pop)

#endif //PROTOCOL_DEF_H
