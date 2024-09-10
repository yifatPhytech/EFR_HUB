/*
 * UARTComm.cpp
 *
 *  Created on: Sep 11, 2023
 *      Author: itay
 */

#include "libraries/UART/UARTComm.h"

#include "libraries/UART/uart_decoder.h"
#include "sl_sleeptimer.h"                          // Include the sleep timer header for EFR32


// buffer
#define BUFSIZE 80
static char buffer[BUFSIZE];


void UARTComm_init(bool allow_print)
{
  // Prevent buffering of output/input.
#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
  setvbuf(stdout, NULL, _IONBF, 0);   // Set unbuffered mode for stdout (newlib)
  setvbuf(stdin, NULL, _IONBF, 0);    // Set unbuffered mode for stdin (newlib)
#endif

  // Setting default stream
  sl_iostream_set_default(sl_iostream_inst_handle);

  if (allow_print)
    {
      // Output on vcom usart instance
      const char str1[] = "UARTComm USART with IOstream initialized.\r\n\r\n";
      sl_iostream_write(sl_iostream_inst_handle, str1, strlen(str1));


      const char str2[] = "Default stream set to USART.\r\n";
      sl_iostream_write(SL_IOSTREAM_STDOUT, str2, strlen(str2));

      // for using printf as global function should install iostream_retarget_stdio software component.
      printf("Printf uses the default stream, as long as iostream_retarget_stdio is included.\r\n");
    }
}

void UARTComm_SendNewLine(void)
{
  const char newline[] = "\r\n";
  sl_iostream_write(sl_iostream_inst_handle, newline, strlen(newline));
}

void UARTComm_process_action(void)
{
  int8_t c = 0;
  static uint8_t index = 0;
  static bool print_prompt = true;

  if (print_prompt) {
      printf("> ");
      print_prompt = false;
  }

  // Retrieve characters, print local echo and full line back
  c = getchar();
  if (c > 0)
    {
      if (c == '\r' || c == '\n')
        {
          buffer[index] = '\0';
          if(!uart_decoder(buffer))
            {
              printf("\r\nInvalid packet: %s\r\n> ", buffer);
            }
          else
            {
              printf("\r\nYou wrote: %s\r\n> ", buffer);
            }
          index = 0;
        }
      else
        {
          if (index < BUFSIZE - 1)
            {
              buffer[index] = c;
              index++;
            }

          // Local echo
          //putchar(c);
        }
    }
}

void UARTComm_SendString(const char *str, size_t length)
{
  sl_status_t status = sl_iostream_write(sl_iostream_inst_handle, str, length);
  //UARTComm_SendNewLine();

  // Introducing a delay to allow the transmission of all characters,
  // simulating a "flush" operation by ensuring the completion of data transmission
  if(status == SL_STATUS_OK)
    {
      // Calculate delay needed for the given length, assuming a baud rate of 115200 bps,
      // 10 bits per character (1 start bit, 8 data bits, 1 stop bit)
      size_t bits = length * 10;
      size_t delay_ms = (1000 * bits) / 115200; // Convert bits to milliseconds at the baud rate
      delay_ms += 1; // Add a small buffer to ensure all data is transmitted

      sl_sleeptimer_delay_millisecond(delay_ms);
    }
}

void UARTComm_SendByteArray(const char *byteArray, size_t length)
{
  sl_status_t status = sl_iostream_write(sl_iostream_inst_handle, byteArray, length);

  // Introducing a delay to allow the transmission of all characters,
  // simulating a "flush" operation by ensuring the completion of data transmission
  if(status == SL_STATUS_OK)
    {
      // Assuming a baud rate of 115200 bps, 10 bits per character (1 start bit, 8 data bits, 1 stop bit)
      sl_sleeptimer_delay_millisecond((length * 10 * 1000) / 115200);
    }
}
