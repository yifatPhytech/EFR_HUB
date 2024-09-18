/*
 * radio_handler.c
 *
 * Created on: Oct 30, 2023
 * Author: itay
 */



#include "sl_flex_rail_package_assistant.h"
#include "sl_flex_rail_channel_selector.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include <stdio.h>                          // for printf function
#include "sl_component_catalog.h"
#include "sl_flex_rail_config.h"
#if defined(SL_CATALOG_APP_ASSERT_PRESENT)
#include "app_assert.h"
#endif
#if defined(SL_CATALOG_APP_LOG_PRESENT)
#include "app_log.h"
#endif
//#include "rail_types.h"
#include "rail.h"
#include "cmsis_compiler.h"
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif
#if defined(SL_CATALOG_RAIL_SIMPLE_CPC_PRESENT)
#include "sl_rail_simple_cpc.h"
#endif
#include <stdint.h>

// Includes from the "libraries" folder
#include "libraries/RADIO/rf_monitor_decoder.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/RADIO/rf_decoder.h"
#include "libraries/UART/UARTComm.h"
#include "libraries/Hub_Definition/hub_protocols.h"
#include "libraries/Hub_Definition/hub_protocols.h"
#include "libraries/Hub_Definition/rf_rx_handle.h"
#include "libraries/RADIO/radio_handler.h"

#define EZRADIO_FIFO_SIZE       64


/// Transmit data length
#define TX_PAYLOAD_LENGTH (80U)

/// State machine of simple_trx
typedef enum
{
  S_PACKET_RECEIVED,
  S_PACKET_START_SEND,
  S_PACKET_SENT,
  S_RX_PACKET_ERROR,
  S_TX_PACKET_ERROR,
  S_PACKET_WAIT_4_PARSE,
  S_IDLE,
  S_SET_RF_PWR,
} state_t;

/// tx_requested and rx_requested boolean variables
volatile bool tx_requested = false;
volatile bool rx_requested = true;
volatile rf_power g_curRfPwr;
volatile rf_power g_newRfPwr;
/// Transmit packet
//uint8_t out_packet[TX_PAYLOAD_LENGTH];
uint8_t out_packet[21] =
    {
        20, 56, 162, 135, 0, 12, 123, 2, 2, 2, 2, 2, 2, 1, 168, 178, 255, 255, 255, 255, 75
    };

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// The variable shows the actual state of the state machine
static volatile state_t state = S_IDLE;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t error_code = 0;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

/// Receive and Send FIFO
static __ALIGNED(RAIL_FIFO_ALIGNMENT) uint8_t rx_fifo[SL_FLEX_RAIL_RX_FIFO_SIZE];

static __ALIGNED(RAIL_FIFO_ALIGNMENT) uint8_t tx_fifo[SL_FLEX_RAIL_TX_FIFO_SIZE];

static uint8_t dataSize = 0;
/// Flags to update state machine from interrupt
static volatile bool packet_recieved = false;
static volatile bool packet_sent = false;
static volatile bool packet_to_send = false;
static volatile bool rx_error = false;
static volatile bool tx_error = false;
static volatile bool rf_pwr_change = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void ChangeRfPower(RAIL_Handle_t rail_handle, rf_power pwr)
{
  if (g_curRfPwr == pwr)
    return;
  g_curRfPwr = pwr;
  RAIL_ConfigTxPower(rail_handle, tx_power_dbm[(uint8_t)pwr]);
  printf("set RF power to %d", g_curRfPwr);
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  // RAIL Rx packet handles
  RAIL_RxPacketHandle_t rx_packet_handle;
  RAIL_RxPacketInfo_t packet_info;
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status = RAIL_STATUS_NO_ERROR;
  RAIL_Status_t calibration_status_buff = RAIL_STATUS_NO_ERROR;

#if defined(SL_CATALOG_RAIL_SIMPLE_CPC_PRESENT)
  uint8_t success_sent = 0x01;
#endif

  if (packet_recieved)
    {
      packet_recieved = false;
      state = S_PACKET_RECEIVED;
    }
  else if (packet_to_send)
    {
      packet_to_send = false;
      state = S_PACKET_START_SEND;
    }
  else if (packet_sent)
    {
      packet_sent = false;
      state = S_PACKET_SENT;
    }
  else if (rx_error)
    {
      rx_error = false;
      state = S_RX_PACKET_ERROR;
    }
  else if (tx_error)
    {
      tx_error = false;
      state = S_TX_PACKET_ERROR;
    }
  else if (rf_pwr_change)
    {
      rf_pwr_change = false;
      state = S_SET_RF_PWR;
    }

  switch (state)
  {
    case S_PACKET_RECEIVED:
      {
        // Packet received:
        //  - Check whether RAIL_HoldRxPacket() was successful, i.e. packet handle is valid
        //  - Copy it to the application FIFO
        //  - Free up the radio FIFO
        //  - Return to app IDLE state (RAIL will automatically switch back to Rx radio state)
        rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
        while (rx_packet_handle != RAIL_RX_PACKET_HANDLE_INVALID)
          {
            uint8_t *start_of_packet = 0;
            uint16_t packet_size = unpack_packet(rx_fifo, &packet_info, &start_of_packet);
            rail_status = RAIL_ReleaseRxPacket(rail_handle, rx_packet_handle);
            if (rail_status != RAIL_STATUS_NO_ERROR)
              {
#if defined(SL_CATALOG_APP_LOG_PRESENT)
                app_log_warning("RAIL_ReleaseRxPacket() result:%d", rail_status);
#endif
              }
            if (rx_requested)
              {
                int16_t nRSSI = RAIL_GetRssi(rail_handle, false);
                printf_rx_packet(start_of_packet, packet_size);
                SaveNewPacket(start_of_packet, packet_size, nRSSI);

                // wait to decode RX packet
//                if (getSystemMode() == ACTIVE_MODE)
//                  {
//                    decode_rf_packet(start_of_packet, packet_size);
//                  }
//                else  // MONITOR_MODE
//                  {
//                    decode_monitor_rf_packet(start_of_packet, packet_size);
//                  }
#if defined(SL_CATALOG_RAIL_SIMPLE_CPC_PRESENT)
                sl_rail_simple_cpc_transmit(packet_size, start_of_packet);
#endif
              }
            rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
          }
        state = S_IDLE;
      }
      break;
    case S_PACKET_START_SEND:
      rf_send(rail_handle, tx_fifo, dataSize);
      state = S_IDLE;
      break;
    case S_PACKET_SENT:
      {
#if defined(SL_CATALOG_APP_LOG_PRESENT)
        app_log_info("Packet has been sent\n");
#endif
#if defined(SL_CATALOG_RAIL_SIMPLE_CPC_PRESENT)
        sl_rail_simple_cpc_transmit(1, &success_sent);
#endif

#if defined(SL_CATALOG_LED1_PRESENT)
        //        sl_led_toggle(&sl_led_led1);
#else
        //        sl_led_toggle(&sl_led_led0);
#endif
        state = S_IDLE;
      }
      break;

    case S_RX_PACKET_ERROR:
      {
        // Handle Rx error
#if defined(SL_CATALOG_APP_LOG_PRESENT)
        app_log_error("Radio RX Error occurred\nEvents: %llX\n", error_code);
#endif
        state = S_IDLE;
      }
      break;

    case S_TX_PACKET_ERROR:
      {
        // Handle Tx error
#if defined(SL_CATALOG_APP_LOG_PRESENT)
        app_log_error("Radio TX Error occurred\nEvents: %llX\n", error_code);
#endif
        state = S_IDLE;
      }
      break;

    case S_SET_RF_PWR:
      ChangeRfPower(rail_handle, g_newRfPwr);
      state = S_IDLE;
      break;
    case S_IDLE:
      {
        if (tx_requested)
          {
            prepare_package(rail_handle, out_packet, sizeof(out_packet));
            rail_status = RAIL_StartTx(rail_handle, get_selected_channel(), RAIL_TX_OPTIONS_DEFAULT, NULL);
            if (rail_status != RAIL_STATUS_NO_ERROR)
              {
#if defined(SL_CATALOG_APP_LOG_PRESENT)
                app_log_warning("RAIL_StartTx() result:%d ", rail_status);
#endif
              }
            tx_requested = false;
          }
      }
      break;

//    case S_CALIBRATION_ERROR:
//      {
//        calibration_status_buff = calibration_status;
//#if defined(SL_CATALOG_APP_LOG_PRESENT)
//        app_log_error("Radio Calibration Error occurred\nEvents: %llX\nRAIL_Calibrate() result:%d\n",
//                      error_code,
//                      calibration_status_buff);
//#endif
//        state = S_IDLE;
//        break;
    default:
      // Unexpected state
#if defined(SL_CATALOG_APP_LOG_PRESENT)
      app_log_error("Unexpected Simple TRX state occurred:%d\n", state);
#endif
      } //switch
//      break;
//  }
}


/******************************************************************************
 * RAIL callback, called if a RAIL event occurs.
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  error_code = events;
  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION )
    {
      if (events & RAIL_EVENT_RX_PACKET_RECEIVED)
        {
          // Keep the packet in the radio buffer, download it later at the state machine
          RAIL_HoldRxPacket(rail_handle);

          packet_recieved = true;
        }
      else
        {
          // Handle Rx error
          rx_error = true;
        }
    }
  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION)
    {
      if (events & RAIL_EVENT_TX_PACKET_SENT)
        {
          packet_sent = true;
        }
      else
        {
          // Handle Tx error
          tx_error = true;
        }
    }

  // Perform all calibrations when needed
//  if ( events & RAIL_EVENT_CAL_NEEDED )
//    {
//      calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
//      if (calibration_status != RAIL_STATUS_NO_ERROR)
//        {
//          cal_error = true;
//        }
//    }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

#if defined(SL_CATALOG_RAIL_SIMPLE_CPC_PRESENT)
void sl_rail_simple_cpc_receive_cb(sl_status_t status, uint32_t len, uint8_t *data)
{
  //SL_STATUS_OK
  if (status == SL_STATUS_OK) {
      if (len == 1) {
          if (data[0] == 0x01 || data[0] == '1') {
              tx_requested = true;
          }
          if (data[0] == 0x00 || data[0] == '0') {
              rx_requested = !rx_requested;
          }
      }
  }
}
#endif

/******************************************************************************
 * Set up the rail TX fifo for later usage
 * @param rail_handle Which rail handler should be updated
 *****************************************************************************/
void set_up_tx_fifo(RAIL_Handle_t rail_handle)
{
  uint16_t allocated_tx_fifo_size = 0;
  allocated_tx_fifo_size = RAIL_SetTxFifo(rail_handle, tx_fifo, 0, SL_FLEX_RAIL_TX_FIFO_SIZE);
#if defined(SL_CATALOG_APP_ASSERT_PRESENT)
  app_assert(allocated_tx_fifo_size == SL_FLEX_RAIL_TX_FIFO_SIZE,
             "RAIL_SetTxFifo() failed to allocate a large enough fifo (%d bytes instead of %d bytes)\n",
             allocated_tx_fifo_size,
             SL_FLEX_RAIL_TX_FIFO_SIZE);
#endif
}

RAIL_Status_t rf_send(RAIL_Handle_t rail_handle, uint8_t *packet, size_t packet_len)
{
  // Debug print to log the original packet_len
  printf("Original packet length: %zu\n", packet_len);

  // Ensure that the packet_len can be accommodated in a uint8_t
  if (packet_len > UINT8_MAX || packet_len > SL_FLEX_RAIL_TX_FIFO_SIZE - 1) // -1 to account for the added length byte
    {
      return RAIL_STATUS_INVALID_PARAMETER;
    }

  uint8_t modified_packet[packet_len + 1]; // Variable Length Array (VLA)
  modified_packet[0] = (uint8_t) packet_len; // Set the first byte to be the length of the packet
  memcpy(&modified_packet[1], packet, packet_len); // Copy the rest of the packet

  // Debug print to log the modified packet length
  printf("Modified packet length (with added length byte): %zu\n", packet_len + 1);

  // Print the buffer for debugging
#if defined(SL_CATALOG_APP_LOG_PRESENT)
  printf("Sending Buffer: ");
  for(size_t i = 0; i <= packet_len; i++) // <= to include the length byte
    {
      printf("%d ", modified_packet[i]);
    }
  printf("\n");
#else
  printf("Sending Buffer: ");
  for(size_t i = 0; i <= packet_len; i++) // <= to include the length byte
    {
      printf("%02x ", modified_packet[i]);
    }
  printf("\n");
#endif

  // Prepare the packet
  prepare_package(rail_handle, modified_packet, packet_len + 1); // +1 to include the length byte

  // Send the packet
  RAIL_Status_t status = RAIL_StartTx(rail_handle, get_selected_channel(), RAIL_TX_OPTIONS_DEFAULT, NULL);

  if (status != RAIL_STATUS_NO_ERROR)
    {
#if defined(SL_CATALOG_APP_LOG_PRESENT)
      app_log_warning("rf_send() result:%d ", status);
#endif
    }

  // Reset the tx_requested flag as the transmission is initiated.
  tx_requested = false;

  return status;
}

RAIL_Status_t rf_send_NG(RAIL_Handle_t rail_handle, uint8_t *packet, size_t packet_len)
{
  // Debug print to log the original packet_len
  printf("Original packet length: %zu\n", packet_len);

  // Ensure that the packet_len can be accommodated in a uint8_t
  if (packet_len > UINT8_MAX || packet_len > SL_FLEX_RAIL_TX_FIFO_SIZE - 1) // -1 to account for the added length byte
    {
      return RAIL_STATUS_INVALID_PARAMETER;
    }

  uint8_t modified_packet[packet_len + 1]; // Variable Length Array (VLA)
  modified_packet[0] = (uint8_t) packet_len; // Set the first byte to be the length of the packet
  memcpy(&modified_packet[1], packet, packet_len); // Copy the rest of the packet

  // Debug print to log the modified packet length
  printf("Modified packet length (with added length byte): %zu\n", packet_len + 1);

  // Print the buffer for debugging
#if defined(SL_CATALOG_APP_LOG_PRESENT)
  printf("Sending Buffer: ");
  for(size_t i = 0; i <= packet_len; i++) // <= to include the length byte
    {
      printf("%d ", modified_packet[i]);
    }
  printf("\n");
#else
  printf("Sending Buffer: ");
  for(size_t i = 0; i <= packet_len; i++) // <= to include the length byte
    {
      printf("%02x ", modified_packet[i]);
    }
  printf("\n");
#endif

  // Prepare the packet
  prepare_package(rail_handle, modified_packet, packet_len + 1); // +1 to include the length byte

  // Send the packet
  RAIL_Status_t status = RAIL_StartTx(rail_handle, get_selected_channel(), RAIL_TX_OPTIONS_DEFAULT, NULL);

  if (status != RAIL_STATUS_NO_ERROR)
    {
#if defined(SL_CATALOG_APP_LOG_PRESENT)
      app_log_warning("rf_send_NG() result:%d ", status);
#endif
    }

  // Reset the tx_requested flag as the transmission is initiated.
  tx_requested = false;

  return status;
}

RAIL_Status_t rf_send_LEGACY(RAIL_Handle_t rail_handle, uint8_t *packet, size_t packet_len)
{
  // Debug print to log the original packet_len
  printf("Original packet length (LEGACY): %zu\n", packet_len);

  // Ensure that the packet_len can be accommodated in a uint8_t
  if (packet_len > UINT8_MAX || packet_len > SL_FLEX_RAIL_TX_FIFO_SIZE)
    {
      return RAIL_STATUS_INVALID_PARAMETER;
    }

  // Print the buffer for debugging
  printf("Sending Buffer (LEGACY): ");
  for(size_t i = 0; i < packet_len; i++)
    {
      printf("%d ", packet[i]);
    }
  printf("\n");

  // Prepare the packet
  prepare_package(rail_handle, packet, packet_len);

  // Send the packet
  RAIL_Status_t status = RAIL_StartTx(rail_handle, get_selected_channel(), RAIL_TX_OPTIONS_DEFAULT, NULL);

  if (status != RAIL_STATUS_NO_ERROR)
    {
#if defined(SL_CATALOG_APP_LOG_PRESENT)
      app_log_warning("rf_send_LEGACY() result:%d ", status);
#endif
    }

  // Reset the tx_requested flag as the transmission is initiated.
  tx_requested = false;

  return status;
}

RAIL_Status_t rf_send_adc_results(RAIL_Handle_t rail_handle, IADC_Result_t *adcResults, size_t length)
{
  printf("Ch0: %lu, Ch1: %lu, Ch2: %lu, Ch3: %lu\r\n",
         (uint32_t)adcResults[0].data,
         (uint32_t)adcResults[1].data,
         (uint32_t)adcResults[2].data,
         (uint32_t)adcResults[3].data);
  return rf_send(rail_handle, (uint8_t *)adcResults, length);
}

void BufferEnvelopeTransmit()
{
  uint8_t bufLen = msgOut.Header.m_size;
    uint8_t radioTxPkt[bufLen];
  printf("BufferEnvelopeTransmit");
  if (bufLen >= 64)
  {
      printf("BufferEnvelopeTransmit: SIZE: %d too long. delete transmission", bufLen);//g_LoggerID = %d", g_LoggerID);
    return;
  }

//  if ((g_msgType == MSG_CONFIG) || (g_msgType == MSG_FW_UPDATE) || (g_wCurMode == MODE_WRITE_EEPROM))
//  {
//    mntr.m_size++;
//    bufLen = mntr.m_size;
//    for ( i = 0; i < bufLen-1; i++)
//      radioTxPkt[FIRST_FIELD_LEN+i] = (((const uint8_t *) &mntr) [i]);
//  }
//  else
  {
    memcpy(tx_fifo, (uint8_t *) &msgOut, bufLen); // Copy the packet
}
  tx_fifo[bufLen] = GetCheckSum(tx_fifo, bufLen);
  dataSize = bufLen;
  packet_to_send = true;
//  rf_send( rail_handle, radioTxPkt, bufLen+1);

}

void SetNewRfPower(rf_power newPwr)
{
  rf_pwr_change = true;
  g_newRfPwr = newPwr;
}
