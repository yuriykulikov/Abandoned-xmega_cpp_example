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
#ifndef EXAMPLE_HANDLER_H_
#define EXAMPLE_HANDLER_H_

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "Handler.h"
#include "LedProcessorThread.h"
#include "Serial.h"
#include "Spi.h"
#include "PinChangeController.h"

#define EVENT_RUN_SPI_TEST 1
#define EVENT_BLINK 2

/** An example handler, which handles messages from CommandLineInterpreter */
class ExampleHandler: public Handler, public OnPinChangeListener {
private:
    /** Pointer to SpiSlave to use */
    SpiDevice *master;
    LedProcessorThread *led;
    Serial *debugSerial;
public:
    ExampleHandler(Looper *looper, SpiDevice *spiMaster, LedProcessorThread *led, Serial *debugUsart);
    void handleMessage(Message msg);
    bool onPinChange(PORT_t *port);
};

#endif /* EXAMPLE_HANDLER_H_ */
