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
#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include "avr_compiler.h"

// Scheduler include files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define SPI_SS_bm             0x10
#define SPI_MOSI_bm           0x20
#define SPI_MISO_bm           0x40
#define SPI_SCK_bm            0x80

/**
 * @brief Class, representing an SPI master.
 * It is used to pass data between Task and ISR.
 * There should be one global instance of this class for each module.
 */
class SpiMaster {
public:
    /** Pointer to what module to use. */
    SPI_t *module;
    //Master specific fields
    /** Used to prevent several tasks from using SPI module simultaneously */
    xSemaphoreHandle mutex;
    /** Used to synchronize task with interrupts*/
//    xSemaphoreHandle semaphore;
    /** Pointer to data to transmit */
//    const uint8_t *transmitData;
    /** Pointer to receiving buffer */
//	volatile uint8_t *receiveData;
//	volatile uint8_t bytesToTransceive;
//	volatile uint8_t bytesTransceived;
//	volatile uint8_t bytesToReceive;
//	volatile uint8_t bytesReceived;
    SpiMaster(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, bool clk2x, SPI_PRESCALER_t clockDivision);
};

/**
 * @brief Represents particular device on the bus
 * Holds pointer to the SPI master, and SS pin description. Initialize one for each device on SPI bus.
 */
class SpiDevice {
public:
    /** Spi structure to use */
    SpiMaster *master;
    //SS pin description
    PORT_t *ssPort;
    uint8_t ssPinMask;
    SpiDevice(SpiMaster *master, PORT_t *ssPort, uint8_t ssPinMask);
    char startTransmission (int ticksToWait);
    void stopTransmission ();
    uint8_t shiftByte(uint8_t data);
};

/**
 * @brief Represents the slave.
 */
class SpiSlave {
public:
    /** Contents of this register are sent to the master when not transmitting data*/
    uint8_t status;
    /** Master out slave in commands queue */
    xQueueHandle commandsQueue;
    SpiSlave(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, uint8_t queueSize);
    char getByteFromQueue(uint8_t * receivedByte, int ticksToWait);
};

#endif
