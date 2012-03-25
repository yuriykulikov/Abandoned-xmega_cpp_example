/* This file has been prepared for Doxygen automatic documentation generation.*/
/*
 * Copyright (C) 2012 Yuriy Kulikov
 *      Universitaet Erlangen-Nuernberg
 *      LS Informationstechnik (Kommunikationselektronik)
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef USART_DRIVER_H
#define USART_DRIVER_H
#include "avr_compiler.h"
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef enum {
	BAUD9600 = 1,
	BAUD19200 = 2,
} Baudrate;

/*! \brief Struct used when interrupt driven driver is used.
*  Struct containing pointer to a usart, a buffer and a location to store Data
*  register interrupt level temporary.
*/
class Serial {
private:
    /* \brief Pointer to USART module to use. */
    USART_t * module;
    /* \brief Data register empty interrupt level. */
    USART_DREINTLVL_t dreIntLevel;
    /** Default ticksToWait value */
    int ticksToWait;
    /* \brief Data buffer. */
    xQueueHandle RXqueue;
    xQueueHandle TXqueue;
public:
    signed char USART_RXComplete();
    signed char USART_DataRegEmpty();
    Serial(USART_t *module, Baudrate baudrate, uint8_t bufferSize, int ticksToWait);
    int8_t putByte(uint8_t data);
    int8_t putString(const char *string);
    int8_t putPgmString(const char *progmem_s);
    int8_t putInt(int16_t Int,int16_t radix);
    int8_t getByte(char * receivedChar);
    int8_t getByte(char * receivedChar, int ticks);
};
#endif

