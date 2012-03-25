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

#ifndef PINCHANGECONTROLLER_H_
#define PINCHANGECONTROLLER_H_

#include "avr_compiler.h"

/**
 * An interface class, implement your own listener
 */
class OnPinChangeListener {
public:
    /** Exectued from the ISR, be careful!
     * To post to queues, or give semaphores, use only functions suffixed by FromISR!
     * You must check if interrupt was triggered by the observed pin,
     * because there is only one interrupt for all pins on the port
     * @param port on which interrupt occured
     * @return must return true if higher priority task should be woken
     */
    virtual bool onPinChange(PORT_t *port) =0;
};

class PinChangeController {
private:
public:
    //TODO add a list
    OnPinChangeListener *listener;

    PinChangeController();
    void registerOnPinChangeListener(OnPinChangeListener *listener, PORT_t * port, uint8_t pinMask, PORT_OPC_t opc, PORT_ISC_t isc);
};

#endif /* PINCHANGECONTROLLER_H_ */
