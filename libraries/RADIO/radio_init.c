#include "libraries/RADIO/radio_init.h"

#include <stdint.h>
#include "sl_component_catalog.h"
#include "rail.h"
#include "sl_rail_util_init.h"
#include "sl_simple_led_instances.h"
#if defined(SL_CATALOG_APP_LOG_PRESENT)
#include "app_log.h"
#endif
#include "sl_flex_rail_channel_selector.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

#include "libraries/RADIO/radio_handler.h"
#include <em_core_generic.h>
#include <rail_types.h>
#include <em_core.h>
#include <stdio.h>     // for printf function


/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/

RAIL_Handle_t Initialize_RADIO(void)
{
  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  radio_sleep_configuration(rail_handle); // radio sleep configuration
  set_up_tx_fifo(rail_handle);

  set_radio_to_idle_mode(rail_handle);    // set radio to idle mode

  //sl_sleeptimer_delay_millisecond(5);
  //set_radio_to_sleep_mode();

  return rail_handle;
}

void radio_sleep_configuration(RAIL_Handle_t railHandle)
{
  // #NOTE The active RAIL configuration must be idle to enable sleep.

  // Timer synchronization disabled
  // Configure sleep for timer synchronization
  RAIL_Status_t status = RAIL_ConfigSleep(railHandle, RAIL_SLEEP_CONFIG_TIMERSYNC_DISABLED);
  assert(status == RAIL_STATUS_NO_ERROR);

  // Check the status and handle the error
  if (status != RAIL_STATUS_NO_ERROR)
    {
      printf("RAIL_ConfigSleep() failed with status: %d\n", status);
    }
}

bool set_radio_to_idle_mode(RAIL_Handle_t rail_handle)
{
    RAIL_RadioState_t state;
    // Make sure to disable all radio activities
    RAIL_Idle(rail_handle, RAIL_IDLE, true);
//    if (status != RAIL_STATUS_NO_ERROR) {
//        printf("RAIL_Idle() failed with status: %d\n", status);
//        return false; // Handle error appropriately
//    }

    // Check the radio state
    state = RAIL_GetRadioState(rail_handle);
    printf("Radio state: %d\n", state);
    if (state != RAIL_RF_STATE_IDLE)
      {
        printf("Radio did not enter idle state!\n");
        return false;
    }
    return true;
}

/// RADIO MODES
void set_radio_to_sleep_mode(RAIL_Handle_t railHandle)
{
  bool deepSleepAllowed = false;
  uint16_t t = 0,n = 10;
  RAIL_Status_t status;

  // Go critical to assess sleep decisions
//  CORE_irqState_t irqState;
//  irqState = CORE_EnterCritical();

  if (!set_radio_to_idle_mode(railHandle))
    return;
  printf("RAIL in IDLE mode\n");
  do
    {
  status = RAIL_Sleep(t, &deepSleepAllowed);

  if (status != RAIL_STATUS_NO_ERROR)
    {
//      CORE_ExitCritical(irqState);
      printf("Error: cannot set radio to sleep. Status: %d deepSleepAllowed: %d\n", status, deepSleepAllowed);
    }
  sl_sleeptimer_delay_millisecond(100);
  n--;
    }
  while ((status != RAIL_STATUS_NO_ERROR) && (n > 0));
//  else
//    {
//      CORE_ExitCritical(irqState);  // Ensure to exit critical section if no error
//    }
}

void set_radio_to_wakeup_mode(void)
{
  // Go critical to assess sleep decisions
  CORE_irqState_t irqState;
  irqState = CORE_EnterCritical();

  RAIL_Status_t status = RAIL_Wake(0);
  if (status != RAIL_STATUS_NO_ERROR)
    {
      CORE_ExitCritical(irqState);
      printf("Error: cannot set radio to wakeup. Status: %d\n", status);
    }
  else
    {
      CORE_ExitCritical(irqState);  // Ensure to exit critical section if no error
    }
}

//void set_radio_to_idle_mode(RAIL_Handle_t rail_handle)
//{
//  // Make sure to disable all radio activities
//  RAIL_Idle(rail_handle, RAIL_IDLE, true);
//}

void set_radio_to_rx_mode(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t status = RAIL_StartRx(rail_handle, get_selected_channel(), NULL);

  if (status != RAIL_STATUS_NO_ERROR)
    {
#if defined(SL_CATALOG_APP_LOG_PRESENT)
      app_log_warning("Failed to switch to RX mode. RAIL_StartRx() result:%d ", status);
#endif
    }
}
