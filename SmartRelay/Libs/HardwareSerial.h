/*
  HardwareSerial.h - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#if ( defined(UBRRH) || defined(UBRR0H) || defined(UBRR1H) || defined(LINBRRH)) && !USE_SOFTWARE_SERIAL

#include <inttypes.h>

#include "Stream.h"
struct ring_buffer;

class HardwareSerial : public Stream
{
  private:
    volatile ring_buffer *_rx_buffer;
    volatile ring_buffer *_tx_buffer;
    volatile uint8_t *_ubrrh;
    volatile uint8_t *_ubrrl;
    volatile uint8_t *_ucsra;
    volatile uint8_t *_ucsrb;
    volatile uint8_t *_udr;
    uint8_t _rxen;
    uint8_t _txen;
    uint8_t _rxcie;
    uint8_t _udrie;
    uint8_t _u2x;
  public:
    HardwareSerial(ring_buffer *rx_buffer, ring_buffer *tx_buffer
    #if ( defined(UBRRH) || defined(UBRR0H) || defined(UBRR1H))
      ,
      volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
      volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
      volatile uint8_t *udr,
      uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udrie, uint8_t u2x);
    #else
      );
    #endif
    void begin(long);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool();
};

#endif

#if (defined(UBRRH) || defined(UBRR0H) || defined(LINBRRH)) && !USE_SOFTWARE_SERIAL
  extern HardwareSerial Serial;
#endif
#if defined(UBRR1H)
  extern HardwareSerial Serial1;
#endif

#endif
