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

#define DEBUG
//#undef DEBUG

#include "ExampleHandler.h"
#include "Handler.h"
#include "Leds.h"
#include "LedProcessorThread.h"
#include "TxBuffer.h"
#include "Spi.h"
#include "PinChangeController.h"

extern "C" {
#include "strings.h"
}

ExampleHandler::ExampleHandler(Looper *looper, SpiDevice *spiMaster, LedProcessorThread *led, TxBuffer *debugTx)
        :Handler(looper){
    this->master = spiMaster;
    this->led = led;
    this->debugTx = debugTx;
}

void ExampleHandler::handleMessage(Message msg) {
#ifdef DEBUG
    debugTx->putString("ExampleHandler.handleMessage(");
    debugTx->putInt(msg.what, 10);
    debugTx->putString(")\n");
#endif
    TxBuffer *responseTx;
    switch (msg.what) {
    case EVENT_RUN_SPI_TEST:
        responseTx = (TxBuffer*)msg.ptr;
        char obtainedMutex;
        obtainedMutex = master->startTransmission(10);
        if (obtainedMutex) {
            //Transmit bytes
            uint8_t receivedChar = '#';
            responseTx->putPgmString(Strings_SpiMasterExample1);
            receivedChar = master->shiftByte(0xC0);
            responseTx->putInt(receivedChar, 16);
            receivedChar = master->shiftByte(0x01);
            responseTx->putInt(receivedChar, 16);
            responseTx->putPgmString(Strings_newline);
            //Transmit more bytes
            vTaskDelay(1);
            responseTx->putPgmString(Strings_SpiMasterExample2);
            receivedChar = master->shiftByte(0xC0);
            responseTx->putInt(receivedChar, 16);
            receivedChar = master->shiftByte(0xDE);
            responseTx->putInt(receivedChar, 16);
            responseTx->putPgmString(Strings_newline);
            //Transmit more bytes
            vTaskDelay(500);
            responseTx->putPgmString(Strings_SpiMasterExample3);
            receivedChar = master->shiftByte(0xD0);
            responseTx->putInt(receivedChar, 16);
            receivedChar = master->shiftByte(0x00);
            responseTx->putInt(receivedChar, 16);
            receivedChar = master->shiftByte(0xDE);
            responseTx->putInt(receivedChar, 16);
            responseTx->putPgmString(Strings_newline);
            master->stopTransmission();
        }
        break;

    case EVENT_BLINK:
        responseTx = (TxBuffer*) msg.ptr;
        led->post(0x01, 100);
        led->post(0x03, 100);
        led->post(0x06, 100);
        led->post(0x0c, 100);
        led->post(0x18, 100);
        led->post(0x30, 100);
        led->post(0x60, 100);
        led->post(0x40, 100);
        if (responseTx != NULL) {
            responseTx->putPgmString(Strings_BlinkResp);
        } else {
            debugTx->putPgmString(Strings_BlinkResp);
        }
        break;
    }
}

bool ExampleHandler::onPinChange(PORT_t *port) {
    // this will be executed from the interrupt
    return this->sendMessageFromISR(EVENT_BLINK);
}
