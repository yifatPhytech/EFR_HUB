/*
 * define.h
 *
 *  Created on: Feb 26, 2019
 *      Author: Yifat
 */

#ifndef GLOBAL_DEFINE_H_
#define GLOBAL_DEFINE_H_

//#define IRG_WITH_CON
/// Configuration data for EzRadio plugin manager.
#define EZRADIODRV_INIT_DEFAULT                                                 \
  {                                                                             \
    EZRADIODRV_TRANSMIT_PLUGIN_INIT_DEFAULT      /* Tx plugin init */           \
    EZRADIODRV_RECEIVE_PLUGIN_INIT_DEFAULT       /* Rx plugin init */           \
    EZRADIODRV_CRC_ERROR_PLUGIN_INIT_DEFAULT     /* CRC error plugin init */    \
    EZRADIODRV_AUTO_ACK_PLUGIN_INIT_DEFAULT      /* Auto-ack plugin init */     \
    EZRADIODRV_UNMOD_CARRIER_PLUGIN_INIT_DEFAULT /* CW plugin init */           \
    EZRADIODRV_PN9_PLUGIN_INIT_DEFAULT           /* PN9 plugin init */          \
    EZRADIODRV_DIRECT_TRANSMIT_PLUGIN_INIT_DEFAULT /* Direct Tx plugin init */  \
      EZRADIODRV_DIRECT_RECEIVE_PLUGIN_INIT_DEFAULT /* Direct Rx plugin init */ \
  }
//typedef enum _ConfigStage
//{
//	CONFIG_STAGE_1,
//	CONFIG_STAGE_2,
//}ConfigStage;
/*
typedef enum _Headers
{
	HEADER_SEN_LOST = 		0xB0,
	HEADER_SEN_LOST_ACK = 	0xB1,
	HEADER_MSR_ONLY_INT = 	0xB2,
	HEADER_MSR_URGENT= 		0x10,
	HEADER_MSR_URGENT_ACK = 0x11,
	HEADER_SEN_LGR= 		0x12,
	HEADER_SEN_LGR_ACK =	0x13,
	HEADER_SEN_LGR_HST =	0x14,
	HEADER_SEN_LGR_HST_ACK =	0x15,
//	HEADER_SEN_OFF_ACK =	0xB3,
	HEADER_MSR_MLT_ONLY = 	0xB4,
	HEADER_MSR_ACK_GET_PRM = 	0xB5,
	HEADER_SEN_PRM = 		0xB6,
	HEADER_SEN_PRM_ACK = 	0xB7,
	HEADER_MSR_ONLY	=		0xB8,
	HEADER_MSR_ONLY_ACK	=	0xB9,
	HEADER_SEN_CHECK_4_UPDATE =	0xBA,
	HEADER_SEN_UPDATE_STRT_PKT = 	0xBB,
	HEADER_SEN_UPDATE = 	0xBC,
	HEADER_SEN_UPDATE_ACK = 0xBD,
	HEADER_SEN_FCTR_RST = 	0xBE,
	HEADER_SEN_FCTR_RST_ACK = 	0xBF,
	HEADER_MON_GETID = 		0xA1,
	HEADER_MON_GETID_ACK = 	0xA2,
	HEADER_MON_MSR = 		0xA3,
	HEADER_MON_MSR_ACK = 	0xA4,
	HEADER_MON_ID_OK = 		0xA5,
	HEADER_GETID = 			0xA6,
	HEADER_GETID_ACK = 		0xA7,
	HEADER_ID_OK = 			0xA8,
	HEADER_SND_DATA = 		0xC1,
	HEADER_SND_DATA_ALERT = 0xC9,
	HEADER_SND_DATA_ACK = 	0xC2,
	HEADER_HUB_PRM =	 	0xC3,
	HEADER_HUB_PRM_ACK = 	0xC4,
	HEADER_HUB_SNS_PRM =	 0xC5,
	HEADER_HUB_SNS_PRM_ACK = 0xC6,
	HEADER_HUB_CHANGE_SLOT = 0xC7,
	HEADER_HUB_CHANGE_SLOT_ACK = 0xC8,
	HEADER_HUB_CHECK_4_UPDATE =	0xCA,
	HEADER_HUB_UPDATE_START_PKT =	0xCB,
	HEADER_HUB_UPDATE =		0xCC,
	HEADER_HUB_UPDATE_ACK =	0xCD,
	HEADER_HUB_STOP =		0xCE,
	HEADER_HUB_STOP_ACK =	0xCF,
	HEADER_TEST_RF	=		0xD1,
	HEADER_TEST_RF_ACK	= 	0xD2,
	HEADER_START_TECH_MODE=	0xD3,
	HEADER_START_TECH_MODE_ACK	= 	0xD4,
}Headers;*/

typedef enum _ExtraDef
{
	DEF_NONE = 0,
	DEF_HOT = 1,
	DEF_RESET = 2,
	DEF_DBL = 4,
	DEF_HST = 8,
}ExtraDef;

typedef enum _MsgType
{
	TYPE_DATA_N_HST,
	TYPE_PRM,
	TYPE_DATA_NEW,	// not in use
	TYPE_SNS_OFF,	// not in used
	TYPE_MLT_DATA,
	TYPE_HOT_DATA,
//	TYPE_HSTR,
}MsgType;

typedef union _Uint16toBytes
{
	uint16_t iVal;
    uint8_t bVal[2];
}Uint16toBytes;

typedef union _Int16toBytes
{
	int16_t iVal;
    uint8_t bVal[2];
}Int16toBytes;

typedef union _Uint32toBytes
{
	uint32_t iVal;
    uint8_t bVal[4];
}Uint32toBytes;

union Int32toBytes
{
	int32_t iVal;
    uint8_t bVal[4];
} ;

typedef union _FloatToBytes
{
	float fVal;
    uint8_t bVal[4];
}FloatToBytes ;

#define DEFAULT_ID	0xFFFFFFFF

#define	EXTRA_STATUS_NEW		0x01
#define	EXTRA_STATUS_OFF		0x02

/*#define TYPE_NONE	0
#define TYPE_SD			99
#define TYPE_DER		100
#define TYPE_FI			101
#define TYPE_SFI		192
#define TYPE_RAW		66
//#define TYPE_SMP	98
#define TYPE_DER102		125
#define TYPE_IRG_NG		126
#define TYPE_SMP_GNRL	136	//127
#define TYPE_TNS102		128
#define TYPE_SMP_3		135
#define TYPE_SMP_6		127	//136
#define TYPE_SMP_4		137
#define TYPE_ATH		143
#define TYPE_WND		144
#ifdef IRG_WITH_CON
#define TYPE_PRS_10		161
#define TYPE_PRS_HIGH	162
#else
#define TYPE_PRS_10		145
#define TYPE_PRS_HIGH	146
#endif
#define TYPE_RAIN		147
#define TYPE_SENTEK_SOIL	112
#define TYPE_ENV_80			108
#define TYPE_TNS_AMI		156
#define TYPE_FI_102_NG		163
#define TYPE_POND_4_20		170
#define TYPE_SD_102_NG		171
#define TYPE_TANK_4_20		172
#define TYPE_AT_COOL		173
#define TYPE_FERT_FLOW_MTR	174
#define TYPE_LINE_4_20		175
#define TYPE_WTR_FLOW_MTR	176
#define TYPE_SFI_102_NG		177
#define TYPE_PRLL_FERT		178
#define TYPE_PRS_ON_105		191
*/
#define FIRST_FIELD		0
#define FIRST_FIELD_LEN	1


#endif /* GLOBAL_DEFINE_H_ */
