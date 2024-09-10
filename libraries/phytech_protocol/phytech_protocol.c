/*
 * phytech_protocol.c
 *
 *  Created on: Nov 4, 2023
 *      Author: itay
 */

#include "libraries/phytech_protocol/phytech_protocol.h"

const char* header_to_string(uint8_t header)
{
  switch(header)
  {
    case HEADER_MSR_URGENT:            return "HEADER_MSR_URGENT";
    case HEADER_MSR_URGENT_ACK:        return "HEADER_MSR_URGENT_ACK";
    case HEADER_SEN_LGR:               return "HEADER_SEN_LGR";
    case HEADER_SEN_LGR_ACK:           return "HEADER_SEN_LGR_ACK";
    case HEADER_MON_GETID:             return "HEADER_MON_GETID";
    case HEADER_MON_GETID_ACK:         return "HEADER_MON_GETID_ACK";
    case HEADER_MON_MSR:               return "HEADER_MON_MSR";
    case HEADER_MON_MSR_ACK:           return "HEADER_MON_MSR_ACK";
    case HEADER_MON_ID_OK:             return "HEADER_MON_ID_OK";
    case HEADER_GETID:                 return "HEADER_GETID";
    case HEADER_GETID_ACK:             return "HEADER_GETID_ACK";
    case HEADER_ID_OK:                 return "HEADER_ID_OK";
    case HEADER_SEN_LOST:              return "HEADER_SEN_LOST";
    case HEADER_SEN_LOST_ACK:          return "HEADER_SEN_LOST_ACK";
    case HEADER_MSR_ONLY_INT:          return "HEADER_MSR_ONLY_INT";
    case HEADER_MSR_MLT_ONLY:          return "HEADER_MSR_MLT_ONLY";
    case HEADER_MSR_ACK_GET_PRM:       return "HEADER_MSR_ACK_GET_PRM";
    case HEADER_SEN_PRM:               return "HEADER_SEN_PRM";
    case HEADER_SEN_PRM_ACK:           return "HEADER_SEN_PRM_ACK";
    case HEADER_MSR_ONLY:              return "HEADER_MSR_ONLY";
    case HEADER_MSR_ONLY_ACK:          return "HEADER_MSR_ONLY_ACK";
    case HEADER_SEN_CHECK_4_UPDATE:    return "HEADER_SEN_CHECK_4_UPDATE";
    case HEADER_SEN_UPDATE_STRT_PKT:   return "HEADER_SEN_UPDATE_STRT_PKT";
    case HEADER_SEN_UPDATE:            return "HEADER_SEN_UPDATE";
    case HEADER_SEN_UPDATE_ACK:        return "HEADER_SEN_UPDATE_ACK";
    case HEADER_SEN_FCTR_RST:          return "HEADER_SEN_FCTR_RST";
    case HEADER_SEN_FCTR_RST_ACK:      return "HEADER_SEN_FCTR_RST_ACK";
    case HEADER_SND_DATA:              return "HEADER_SND_DATA";
    case HEADER_SND_DATA_ACK:          return "HEADER_SND_DATA_ACK";
    case HEADER_SND_DATA_ALERT:        return "HEADER_SND_DATA_ALERT";
    case HEADER_HUB_PRM:               return "HEADER_HUB_PRM";
    case HEADER_HUB_PRM_ACK:           return "HEADER_HUB_PRM_ACK";
    case HEADER_HUB_SNS_PRM:           return "HEADER_HUB_SNS_PRM";
    case HEADER_HUB_SNS_PRM_ACK:       return "HEADER_HUB_SNS_PRM_ACK";
    case HEADER_HUB_CHANGE_SLOT:       return "HEADER_HUB_CHANGE_SLOT";
    case HEADER_HUB_CHANGE_SLOT_ACK:   return "HEADER_HUB_CHANGE_SLOT_ACK";
    case HEADER_HUB_CHECK_4_UPDATE:    return "HEADER_HUB_CHECK_4_UPDATE";
    case HEADER_HUB_UPDATE_START_PKT:  return "HEADER_HUB_UPDATE_START_PKT";
    case HEADER_HUB_UPDATE:            return "HEADER_HUB_UPDATE";
    case HEADER_HUB_UPDATE_ACK:        return "HEADER_HUB_UPDATE_ACK";
    case HEADER_HUB_STOP:              return "HEADER_HUB_STOP";
    case HEADER_HUB_STOP_ACK:          return "HEADER_HUB_STOP_ACK";
    case HEADER_TEST_RF:               return "HEADER_TEST_RF";
    case HEADER_TEST_RF_ACK:           return "HEADER_TEST_RF_ACK";
    default:                           return "UNKNOWN_HEADER";
  }
}

const char* sensor_type_to_string(SensorTypes sensor_type)
{
  switch(sensor_type)
  {
    case TYPE_IRRIGATION:          return "TYPE_IRRIGATION";
    case TYPE_SMP_6:               return "TYPE_SMP_6";
    case TYPE_ENV_80:              return "TYPE_ENV_80";
    case TYPE_FLOW_MTR:            return "TYPE_FLOW_MTR";
    case TYPE_PRSR_IRG:            return "TYPE_PRSR_IRG";
    case TYPE_SENTEK_SOIL:         return "TYPE_SENTEK_SOIL";
    case TYPE_4_20:                return "TYPE_4_20";
    case TYPE_SMP_GNRL:            return "TYPE_SMP_GNRL";
    case TYPE_SMP_3:               return "TYPE_SMP_3";
    case TYPE_SMP_4:               return "TYPE_SMP_4";
    case TYPE_ATH:                 return "TYPE_ATH";
    case TYPE_FERT_TANK_LVL:       return "TYPE_FERT_TANK_LVL";
    case TYPE_WND:                 return "TYPE_WND";
    case TYPE_WATER_PRESSURE:      return "TYPE_WATER_PRESSURE";
    case TYPE_POND_LVL:            return "TYPE_POND_LVL";
    case TYPE_TANK_LVL_4_20:       return "TYPE_TANK_LVL_4_20";
    case TYPE_FERT_FLOW_MTR:       return "TYPE_FERT_FLOW_MTR";
    case TYPE_MAIN_LINE_4_20:      return "TYPE_MAIN_LINE_4_20";
    case TYPE_SMALL_FI:            return "TYPE_SMALL_FI";
    case TYPE_AT_COOLING:          return "TYPE_AT_COOLING";
    case TYPE_WATER_PRESSURE_HUB:  return "TYPE_WATER_PRESSURE_HUB";
    default:                       return "UNKNOWN_SENSOR_TYPE";
  }
}
