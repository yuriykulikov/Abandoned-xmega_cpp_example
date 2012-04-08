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
    /**
     * @brief initializes a SPI module as master
     * @param module The SPI module
     * @param lsbFirst Data order will be LSB first if this is set to a non-zero value
     * @param mode SPI mode (Clock polarity and phase)
     * @param clk2x SPI double speed mode
     * @param clockDivision SPI clock pre-scaler division factor
     */
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
    /**
     * @brief Initialize device on the SPI bus controlled by master
     * @attention if you use the pin other than the standard SS pin, make sure the standard SS pin is not an input!
     * @param SpiMaster
     * @param ssPort
     * @param ssPinMask
     */
    SpiDevice(SpiMaster *master, PORT_t *ssPort, uint8_t ssPinMask);
    /**
     * @brief Tries to obtain the mutex and pulls SS pin low
     * @attention This is a blocking call, thread calling this may be suspended
     * @param ticks To Wait to obtain the mutex
     * @return true if access to the bus was granted (mutex obtained), false otherwise
     */
    char startTransmission(int ticksToWait);
    /**
     * @brief Set SS pin high and release the mutex
     */
    void stopTransmission();
    /**
     * @brief Tries to obtain the mutex
     * Use this method in combination with releaseMutex(), selectSlave() and unselectSlave()
     * to grant access to the bus for extended period of time. Used primarily for real-time
     * bus communication with devices like RFID readers.
     * @attention This is a blocking call, thread calling this may be suspended
     * @param ticksToWait - to obtain the mutex
     * @return true if access to the bus was granted (mutex obtained), false otherwise
     */
    bool obtainMutex(int ticksToWait);
    /**
     * @brief release the mutex
     */
    void releaseMutex();
    /**
     * @brief Pull SS pin low
     */
    void selectSlave();
    /**
     * @brief Set SS pin high
     * @attention Make sure that you have successfully obtained mutex for SPI master
     * if you have multiple devices on one bus!
     */
    void unselectSlave();
    /**
     * @brief Shifts data with the slave device.
     * @param spiMaster
     * @param data
     * @return received value from slave
     */
    uint8_t shiftByte(uint8_t data);
    /**
     * @brief Shifts done byte with the slave device.
     * @param spiMaster
     * @param data
     * @param rxDataPtr pointer to where to put read data
     */
    void shiftByte(uint8_t txData, uint8_t *rxDataPtr);

    void readByte(uint8_t *rxDataPtr);
    void writeByte(uint8_t txData);
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
    /**
     * @brief Get single byte from the slave queue, which was put there when master performed write.
     * @attention This is a blocking call, thread calling this may be suspended
     * @param *receivedByte
     * @param ticksToWait
     * @return true if data was fetched from the queue, false otherwise (no data)
     */
    char getByteFromQueue(uint8_t * receivedByte, int ticksToWait);
};

#endif
