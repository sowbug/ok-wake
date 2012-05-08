/*
 * AVR310
 */

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include "USI_TWI_Master.h"

union USI_TWI_state {
  unsigned int errorState;
  struct {
    unsigned int addressMode : 1;
    unsigned int masterWriteDataMode : 1;
    unsigned int memReadMode : 1;
    unsigned int unused : 5;
  } state;
} USI_TWI_state;

void USI_TWI_Master_Initialize(void) {
  DDR_USI |= (1 << PIN_USI_SCL);
  DDR_USI |= (1 << PIN_USI_SDA);
  PORT_USI |= (1 << PIN_USI_SDA);
  PORT_USI |= (1 << PIN_USI_SCL);
  USIDR = 0xFF;  // Preload dataregister with "released level" data.
  USICR = (0 << USISIE) | (0 << USIOIE) | (1 << USIWM1) | (0 << USIWM0) |
      (1 << USICS1) | (0 << USICS0) | (1 << USICLK) | (0 << USITC);
  USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC) |
      (0x0 << USICNT0);
}

unsigned char USI_TWI_Get_State_Info(void) {
  return (USI_TWI_state.errorState);
}

unsigned char USI_TWI_Master_Start(void) {
  // Release SCL to ensure that (repeated) Start can be performed
  PORT_USI |= (1 << PIN_USI_SCL);

  while (!(PORT_USI & (1 << PIN_USI_SCL)));  // Verify that SCL becomes high.

  _delay_us(T2_TWI);

  // Generate Start Condition
  PORT_USI &= ~(1 << PIN_USI_SDA);  // Force SDA LOW.
  _delay_us(T4_TWI);
  PORT_USI &= ~(1 << PIN_USI_SCL);  // Pull SCL LOW.
  PORT_USI |= (1 << PIN_USI_SDA);  // Release SDA.
  return TRUE;
}

unsigned char USI_TWI_Master_Stop(void) {
  PORT_USI &= ~(1 << PIN_USI_SDA);  // Pull SDA low.
  PORT_USI |= (1 << PIN_USI_SCL);  // Release SCL.

  while (!(PIN_USI & (1 << PIN_USI_SCL)));  // Wait for SCL to go high.

  _delay_us(T4_TWI);
  PORT_USI |= (1 << PIN_USI_SDA);  // Release SDA.
  _delay_us(T2_TWI);
  return TRUE;
}

/*
 * Data to be sent has to be placed into the USIDR prior to calling this
 * function. Data read, will be return'ed from the function.
 */
unsigned char USI_TWI_Master_Transfer(unsigned char temp) {
  USISR = temp;  // Set USISR according to temp.

  // Prepare clocking.
  temp = (0 << USISIE) | (0 << USIOIE) |
      (1 << USIWM1) | (0 << USIWM0) |
      (1 << USICS1) | (0 << USICS0) | (1 << USICLK) |
      (1 << USITC);

  do {
    _delay_us(T2_TWI);
    USICR = temp;  // Generate positve SCL edge.

    while (!(PIN_USI & (1 << PIN_USI_SCL)));  // Wait for SCL to go high.

    _delay_us(T4_TWI);
    USICR = temp;  // Generate negative SCL edge.
  } while (!(USISR & (1 << USIOIF)));  // Check for transfer complete.

  _delay_us(T2_TWI);
  temp = USIDR;  // Read out data.
  USIDR = 0xFF;  // Release SDA.
  DDR_USI |= (1 << PIN_USI_SDA);  // Enable SDA as output.
  return temp;  // Return the data from the USIDR
}

/*
 * USI Transmit and receive function. LSB of first byte in buffer indicates if
 * a read or write cycles is performed. If set a read operation is performed.
 * Function generates (Repeated) Start Condition, sends address and R/W,
 * Reads/Writes Data, and verifies/sends ACK.
 *
 * This function also handles Random Read function if the memReadMode bit is
 * set. In that case, the function will: The address in memory will be the
 * second byte and is written *without* sending a STOP. Then the Read bit is
 * set (lsb of first byte), the byte count is adjusted (if needed), and the
 * function function starts over by sending the slave address again and reading
 * the data.
 *
 * Success or error code is returned. Error codes are defined in
 * USI_TWI_Master.h.
 */
unsigned char USI_TWI_Start_Transceiver_With_Data(unsigned char *msg,
                                                  unsigned char msgSize) {
  unsigned char const tempUSISR_8bit = (1 << USISIF) | (1 << USIOIF) |
      (1 << USIPF) | (1 << USIDC) | (0x0 << USICNT0);
  unsigned char const tempUSISR_1bit = (1 << USISIF) | (1 << USIOIF) |
      (1 << USIPF) | (1 << USIDC) | (0xE << USICNT0);
  unsigned char *savedMsg = msg;
  unsigned char savedMsgSize = msgSize;

  // The LSB in the address byte determines if is a masterRead or
  // masterWrite operation.
  USI_TWI_state.state.masterWriteDataMode = (!(*msg & (1 << TWI_READ_BIT)));
  // Always true for first byte
  USI_TWI_state.state.addressMode = TRUE;

  if (!USI_TWI_Master_Start()) {
    return FALSE;  // Send a START condition on the TWI bus.
  }

  // Write address and Read/Write data
  do {
    // If masterWrite cycle (or initial address tranmission)
    if (USI_TWI_state.state.addressMode ||
        USI_TWI_state.state.masterWriteDataMode) {
      // Write a byte
      PORT_USI &= ~(1 << PIN_USI_SCL);
      USIDR = *(msg++);
      USI_TWI_Master_Transfer(tempUSISR_8bit);

      // Clock and verify (N)ACK from slave
      DDR_USI &= ~(1 << PIN_USI_SDA);

      if (USI_TWI_Master_Transfer(tempUSISR_1bit) & (1 << TWI_NACK_BIT)) {
        if (USI_TWI_state.state.addressMode) {
          USI_TWI_state.errorState = USI_TWI_NO_ACK_ON_ADDRESS;
        } else {
          USI_TWI_state.errorState = USI_TWI_NO_ACK_ON_DATA;
        }

        return FALSE;
      }

      // means memory start address has been written
      if ((!USI_TWI_state.state.addressMode) &&
          USI_TWI_state.state.memReadMode) {
        msg = savedMsg;  // start at slave address again
        *(msg) |= (TRUE << TWI_READ_BIT);
        USI_TWI_state.errorState = 0;
        USI_TWI_state.state.addressMode = TRUE;
        msgSize = savedMsgSize;

        // Note that the length should be Slave adrs byte + # bytes
        // to read + 1 (gets decremented below)
        if (!USI_TWI_Master_Start()) {
          USI_TWI_state.errorState = USI_TWI_BAD_MEM_READ;
          // Send a START condition on the TWI bus.
          return FALSE;
        }
      } else {
        // Only perform address transmission once.
        USI_TWI_state.state.addressMode = FALSE;
      }
    } else {
      // Read a data byte
      DDR_USI &= ~(1 << PIN_USI_SDA);
      *(msg++) = USI_TWI_Master_Transfer(tempUSISR_8bit);

      // Prepare to generate ACK (or NACK in case of End Of Transmission)
      // If transmission of last byte was performed.
      if (msgSize == 1) {
        USIDR = 0xFF;  // Load NACK to confirm End Of Transmission.
      } else {
        // Load ACK. Set data register bit 7 (output for SDA) low.
        USIDR = 0x00;
      }

      USI_TWI_Master_Transfer(tempUSISR_1bit);  // Generate ACK/NACK.
    }
  } while (--msgSize);  // Until all data sent/received.

  // Send a STOP condition on the TWI bus.
  if (!USI_TWI_Master_Stop()) {
    return FALSE;
  }

  return TRUE;
}

/*
 * USI Normal Read / Write Function Transmit and receive function. LSB of first
 * byte in buffer indicates if a read or write cycles is performed. If set a
 * read operation is performed.
 *
 * Function generates (Repeated) Start Condition, sends address and R/W,
 * Reads/Writes Data, and verifies/sends ACK.
 *
 * Success or error code is returned. Error codes are defined in
 * USI_TWI_Master.h.
 */
unsigned char USI_TWI_Start_Read_Write(unsigned char *msg,
                                       unsigned char msgSize) {
  USI_TWI_state.errorState = 0;  // Clears all mode bits also
  return (USI_TWI_Start_Transceiver_With_Data(msg, msgSize));
}
