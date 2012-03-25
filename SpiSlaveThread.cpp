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

// Compiler definitions include file
#include "avr_compiler.h"

// Scheduler include files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Framework
extern "C" {
#include "strings.h"
}
#include "Serial.h"
#include "Spi.h"
// File headers
#include "SpiSlaveThread.h"
#include "Thread.h"

/**
 * @brief Starts SPI Slave example task
 * @param usartBuffer
 * @param debugLed
 * @param cPriority
 * @param taskHandle
 */
SpiSlaveThread::SpiSlaveThread(SpiSlave * spiSlave, Serial * serial, const char *name, unsigned short stackDepth, char priority)
:Thread(name, stackDepth, priority){
    this->spiSlave = spiSlave;
    this->serial = serial;
}
/**
 * @brief Slave task - simple spi-usart bridge
 * Tasks blocks on the queue, and as soon as data appears on the queue, task sends it
 * over the usart.
 * @attention use @link startSpiSlaveTask
 * @param pvParameters pass the struct using xTaskCreate
 */
void SpiSlaveThread::run() {
    uint8_t receivedChar='#';

    //Infinite loop
    for (;;) {
        //Function will block the task
        if (spiSlave->getByteFromQueue(&receivedChar, portMAX_DELAY) == pdPASS ) {
            serial->putPgmString(Strings_SpiSlaveExample1);
            serial->putInt(receivedChar,16);
            serial->putPgmString(Strings_newline);
            // report some kind of status
            spiSlave->status = receivedChar + 0x01;
        }
    }
}
