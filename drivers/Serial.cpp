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

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
extern "C" {
#include "port_driver.h"
#include "usart.h"
#include <avr/pgmspace.h>
}
#include "Serial.h"

//Structures, representing uart and its buffer. for internal use.
//Memory allocated dynamically
Serial * usartC;
Serial * usartD;
Serial * usartE;

ISR(USARTC0_RXC_vect){
    //TODO declare ISR as naked, add context switch
    uint8_t isHigherPriorityTaskWoken = usartC->USART_RXComplete();
}
ISR(USARTD0_RXC_vect){
    //TODO declare ISR as naked, add context switch
    uint8_t isHigherPriorityTaskWoken =  usartD->USART_RXComplete();
}
ISR(USARTE0_RXC_vect){
    //TODO declare ISR as naked, add context switch
    uint8_t isHigherPriorityTaskWoken =  usartE->USART_RXComplete();
}

ISR(USARTC0_DRE_vect){usartC->USART_DataRegEmpty();}
ISR(USARTD0_DRE_vect){usartD->USART_DataRegEmpty();}
ISR(USARTE0_DRE_vect){usartE->USART_DataRegEmpty();}

/**
 * @brief Initializes buffer and selects what USART module to use.
 *
 * This function is a "constructor", it allocates memory,
 * initializes receive and transmit buffer and selects what USART module to use,
 * and stores the data register empty interrupt level.
 * @param module hardware module to use
 * @param baudrate
 * @param bufferSize
 * @param ticksToWait - Default wait time
 * @return pointer to the Usart software module
 */
Serial::Serial(USART_t *module, Baudrate baudrate, uint8_t bufferSize, int ticksToWait)
        :RxBuffer(ticksToWait), TxBuffer(ticksToWait) {
    PORT_t * port;
    //switch by pointer to usart structure, which will be used. Usually it is not a good idea to
    //switch by pointer, but in our case pointers are defined by hardware
    if (module == &USARTC0) {
        //copy pointer pUsartBuffer to global pUsartBufferC, which is use to handle interrupts
        usartC = this;
        //Since usart is on the port C, we will need to use PORTC
        port = &PORTC;
    } else if (module == &USARTD0) {
        //copy pointer pUsartBuffer to global pUsartBufferD, which is use to handle interrupts
        usartD = this;
        //Since usart is on the port D, we will need to use PORTD
        port = &PORTD;
    } else if (module == &USARTE0) {
        //copy pointer pUsartBuffer to global pUsartBufferD, which is use to handle interrupts
        usartE = this;
        //Since usart is on the port E, we will need to use PORTE
        port = &PORTE;
    }

    /* (TX0) as output. */
    port->DIRSET = PIN3_bm;
    /* (RX0) as input. */
    port->DIRCLR = PIN2_bm;
    //totempole and pullup
    PORT_ConfigurePins( port,PIN3_bm,false,false,PORT_OPC_PULLUP_gc,PORT_ISC_BOTHEDGES_gc );
    /* Initialize buffers. Create a queue (allocate memory) and store queue handle in Usart
     * On XMEGA port create all queues before vStartTaskScheduler() to ensure that heap size is enough */
    /* Store pointer to USART module */
    this->module = module;
    /*Store DRE level so we will know which level to enable when we put data and want it to be sent. */
    this->dreIntLevel = USART_DREINTLVL_LO_gc;
    /* @brief  Receive buffer size: 2,4,8,16,32,64,128 bytes. */
    RXqueue = xQueueCreate(bufferSize,sizeof(char));
    TXqueue = xQueueCreate(bufferSize,sizeof(char));

    /* USARTD0, 8 Data bits, No Parity, 1 Stop bit. */
    USART_Format_Set(module, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
    /* Enable RXC interrupt. */
    USART_RxdInterruptLevel_Set(module, USART_RXCINTLVL_LO_gc);

    //http://prototalk.net/forums/showthread.php?t=188
    switch (baudrate) {
    case BAUD9600:
        USART_Baudrate_Set(module, 3317 , -4);
        break;
    default:
        //9600
        USART_Baudrate_Set(module, 3317 , -4);
        break;
    }

    /* Enable both RX and TX. */
    USART_Rx_Enable(module);
    USART_Tx_Enable(module);
}

int8_t Serial::putByte(uint8_t data) {
    return xQueueSendToBack(TXqueue, &data, txTicksToWait);
}

void Serial::flush() {
    /* Enable DRE interrupt. */
    uint8_t tempCTRLA = module->CTRLA;
    tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | dreIntLevel;
    module->CTRLA = tempCTRLA;
}

int8_t Serial::getByte(char * receivedChar, int ticks) {
    return xQueueReceive(RXqueue, receivedChar, ticks);
}

/**
 * Receive complete interrupt service routine
 *
 * Stores received data in RX software buffer.
 * @param Usart software abstraction structure
 * @return xHigherPriorityTaskWoken
 */
signed char Serial::USART_RXComplete() {
    /* We have to check is we have woke higher priority task, because we post to
     * queue and high priority task might be blocked waiting for items appear on
     * this queue */
    signed char xHigherPriorityTaskWoken = pdFALSE;
    signed char cChar;
    /* Get the character and post it on the queue of Rxed characters.
     If the post causes a task to wake force a context switch as the woken task
     may have a higher priority than the task we have interrupted. */
    cChar = module->DATA;
    xQueueSendToBackFromISR( RXqueue, &cChar, &xHigherPriorityTaskWoken );
    return xHigherPriorityTaskWoken;
}

/** @brief Data Register Empty Interrupt Service Routine.
 *
 *  Data Register Empty Interrupt Service Routine. Transmits one byte from TX software buffer.
 *  Disables DRE interrupt if buffer is empty.
 *
 * @param Usart software abstraction structure
 * @return xHigherPriorityTaskWoken
 */
signed char Serial::USART_DataRegEmpty() {
    signed char cChar, cTaskWoken;
    if( xQueueReceiveFromISR( TXqueue, &cChar, &cTaskWoken ) == pdTRUE )
    {
        /* Send the next character queued for Tx. */
        module->DATA = cChar;
    } else {
        /* Queue empty, nothing to send. */
        /* Disable DRE interrupts. */
        uint8_t tempCTRLA = module->CTRLA;
        tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
        module->CTRLA = tempCTRLA;
    }
    return cTaskWoken;
}
