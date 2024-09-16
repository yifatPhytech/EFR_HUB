
#ifndef PROTOCOL_BL_DEF_H
#define PROTOCOL_BL_DEF_H
#include "libraries/Hub_Definition/hub_define.h"


typedef struct _ProtocolBootloaderHeader
{
	uint8_t		m_HeaderID;
	uint8_t 	m_Index;
	uint8_t		m_size;
	uint8_t		m_cs;
	uint32_t 	m_ID;
} PrtlBLHdr;

//typedef struct _ProtocolBootloader
//{
//	PrtlBLHdr	m_Header;
//	uint8_t		m_Buffer[MAX_EZRr_BUFFER_SIZE - sizeof(PrtlBLHdr)];
//} PrtlBL;

typedef struct _Sen_Bl_Msg
{
	uint8_t		m_Header;
	uint8_t 	m_size;
	uint8_t		m_index;
	uint8_t		m_result;
} Sen_Bl_Msg;


#endif //PROTOCOL_BL_DEF_H
