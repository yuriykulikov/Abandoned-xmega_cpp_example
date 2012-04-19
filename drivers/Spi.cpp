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

// Scheduler include files.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
extern "C" {
#include "port_driver.h"
}
#include "Spi.h"

/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Master structure */
SpiMaster *masterC;
/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Slave structure */
SpiSlave *slaveC;
/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Master structure */
SpiMaster *masterD;
/** Global pointer, used to pass data between task and ISR. If used, should point to the allocated Slave structure */
SpiSlave *slaveD;
//Prototype
signed char Spi_handleInterrupt (SPI_t *module, SpiMaster *master, SpiSlave *slave);
//Interrupt handling for the module on the port C
ISR(SPIC_INT_vect){
    //TODO declare ISR as naked, add context switch
    uint8_t isHigherPriorityTaskWoken = Spi_handleInterrupt(&SPIC, masterC, slaveC);
}
//Interrupt handling for the module on the port D
ISR(SPID_INT_vect){
    //TODO declare ISR as naked, add context switch
    uint8_t isHigherPriorityTaskWoken = Spi_handleInterrupt(&SPID, masterD, slaveD);
}

/**
 * @brief Common for all modules interrupt handler
 * @param module pointer to the module which has requested the interrupt
 * @param master pointer to the corresponding Master structure
 * @param slave pointer to the corresponding Slave structure
 * @return true if high priority task was unblocked by queue manipulation, false if not
 * */
inline signed char Spi_handleInterrupt (SPI_t *module, SpiMaster *master, SpiSlave *slave) {
	//Used to switch tasks, if queue or semaphore manipulation should wake a task.
	signed char isHigherPriorityTaskWoken = pdFALSE;
	//If spi module was initialized as master
	if (module->CTRL & SPI_MASTER_bm){
		//TODO for arrays
	}
	//If SPI module was initialized as slave
	else {
		// Get received data.
		uint8_t receivedChar = module->DATA;
		// Post the character on the queue of received characters.
		xQueueSendToBackFromISR(slave->commandsQueue, &receivedChar, &isHigherPriorityTaskWoken );
		// Put the status
		module->DATA = slave->status;
	}
	// If the post causes a task to wake force a context switch as the waken task
	// may have a higher priority than the task we have interrupted.
	return isHigherPriorityTaskWoken;
}

/**
 * @brief Initialize SPI slave on the specified module.
 * @param module pointer to the module to use
 * @param lsbFirst
 * @param mode
 * @param bufferSize
 * @return pointer to the Slave struct, to be used with functions prefixed by SpiSlave_
 */
SpiSlave::SpiSlave(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, uint8_t queueSize) {
    // Create the queue
    commandsQueue = xQueueCreate( queueSize, sizeof(char));
    // Initial magic value, could be anything
    status = 0x42;
    //Switch by the pointer to the hardware module to set up port and ISR-Task communication structure
    if (module == &SPIC) {
        //Store the pointer to the object into the appropriate global variable
        slaveC = this;
        PORTC.DIRSET = SPI_MISO_bm;
    } else if (module == &SPID) {
        //Store the pointer to the object into the appropriate global variable
        slaveD = this;
        PORTD.DIRSET = SPI_MISO_bm;
    }
    // Set up SPI hardware module
    module->CTRL = SPI_ENABLE_bm | (lsbFirst ? SPI_DORD_bm : 0) | mode;
    module->INTCTRL = SPI_INTLVL_LO_gc;
}

char SpiSlave::getByteFromQueue(uint8_t *receivedByte, int ticksToWait) {
    return xQueueReceive(commandsQueue, receivedByte, ticksToWait);
}

SpiMaster::SpiMaster(SPI_t *module, bool lsbFirst, SPI_MODE_t mode, bool clk2x, SPI_PRESCALER_t clockDivision){
    // Store module pointer first
    this->module=module;
    // Used to set pins as inputs or outputs
    PORT_t *port = NULL;
    //Switch by the pointer to the hardware module to set up port and ISR-Task communication structure
    if (module == &SPIC) {
        // SPIC interrupt vector will be triggered, connect it to the structure.
        masterC = this;
        port = &PORTC;
    } else if (module == &SPID) {
        // SPID interrupt vector will be triggered, connect it to the structure.
        masterD = this;
        port = &PORTD;
    }
    // Set MOSI and SCK pins outputs;
    port->DIRSET = SPI_MOSI_bm | SPI_SCK_bm;
    // SS should be set to the output, see SS pin section in the XMEGA SPI application note
    port->DIRSET = SPI_SS_bm;
    // Set SS pin high
    port->OUTSET = SPI_SS_bm;
    // Set MISO input
    port->DIRCLR = SPI_MISO_bm;
    // Enable pullup resistor on MISO line
    PORT_ConfigurePins(port, SPI_MISO_bm, false, false, PORT_OPC_PULLUP_gc, PORT_ISC_BOTHEDGES_gc);
    //Create binary semaphore which will be used for synchronization between task and ISR
    //vSemaphoreCreateBinary(master->semaphore);
    // Create mutex which will be used to prevent using module by several tasks at the same time.
    mutex = xSemaphoreCreateMutex();
    // Set up the SPI hardware module
    module->CTRL = clockDivision|(clk2x ? SPI_CLK2X_bm : 0)|SPI_ENABLE_bm
            |(lsbFirst ? SPI_DORD_bm  : 0) |SPI_MASTER_bm |mode;	// TODO Add a field to hold the interrupt level which would be used to transmit large packets
    // Do not use interrupts for single-byte transmissions
    module->INTCTRL = SPI_INTLVL_OFF_gc;
}

SpiDevice::SpiDevice(SpiMaster *master, PORT_t *ssPort, uint8_t ssPinMask){
    // Store spi structure to use for this device
    this->master = master;
    //save shortcuts to decrease relative addressing time
    this->dataReg = &(master->module->DATA);
    this->statusReg = &(master->module->STATUS);
    // Store SS configuration.
    this->ssPort = ssPort;
    this->ssPinMask = ssPinMask;
    // Set SS pin output
    ssPort->DIRSET = ssPinMask;
    // Configure wired and pull-up
    PORT_ConfigurePins(ssPort, ssPinMask,false,false,PORT_OPC_PULLUP_gc, PORT_ISC_BOTHEDGES_gc );
    // Set SS pin high
    ssPort->OUTSET = ssPinMask;
}

char SpiDevice::startTransmission (int ticksToWait) {
    //Try to obtain mutex
    if (xSemaphoreTake(master->mutex, ticksToWait)==pdTRUE) {
        // Pull SS low
        ssPort->OUTCLR=ssPinMask;
        return pdTRUE;
    }
    // Function could not obtain the mutex, so return false
    return pdFALSE;
}

void SpiDevice::stopTransmission () {
    // Pull SS high
    ssPort->OUTSET = ssPinMask;
    //Release the mutex
    xSemaphoreGive(master->mutex);
}

bool SpiDevice::obtainMutex(int ticksToWait) {
    //Try to obtain mutex
    return xSemaphoreTake(master->mutex, ticksToWait);
}

void SpiDevice::releaseMutex(){
    xSemaphoreGive(master->mutex);
}

void SpiDevice::selectSlave(){
    // Pull SS low
    ssPort->OUTCLR=ssPinMask;
}

void SpiDevice::unselectSlave(){
    // Pull SS high
    ssPort->OUTSET = ssPinMask;
}

uint8_t SpiDevice::shiftByte(uint8_t data) {
    //Put byte to initialize data transmission
    *dataReg = data;
    //Wait until byte is shifted
    while(!(*statusReg & SPI_IF_bm)) { /*empty*/ }
    // Accessing the DATA register will clear the SPI_IF_bm
    return *dataReg;
}

void SpiDevice::shiftByte(uint8_t txData, uint8_t *rxDataPtr) {
    //Put byte to initialize data transmission
    *dataReg = txData;
    //Wait until byte is shifted
    while(!(*statusReg & SPI_IF_bm)) { /*empty*/ }
    // Accessing the DATA register will clear the SPI_IF_bm
    *rxDataPtr = *dataReg;
}

void SpiDevice::readByte(uint8_t *rxDataPtr) {
    shiftByte(0x00, rxDataPtr);
}

void SpiDevice::writeByte(uint8_t txData) {
    shiftByte(txData);
}
