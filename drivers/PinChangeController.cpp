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

#include "PinChangeController.h"
extern "C" {
#include "avr_compiler.h"
#include "port_driver.h"
}

volatile PinChangeController *pinChangeController;

//TODO we need a thread as well
PinChangeController::PinChangeController() {
    pinChangeController = this;
    //TODO init the list
}
/**
 * Registers a listener (callback) object. onPinChange method will be invoked when pin change occurs.
 * @param listener
 * @param port
 * @param pinMask
 * @param opc pullup configuration default is PORT_OPC_TOTEM_gc
 * @param isc default is PORT_ISC_RISING_gc
 */
void PinChangeController::registerOnPinChangeListener(OnPinChangeListener *listener, PORT_t * port, uint8_t pinMask, PORT_OPC_t opc, PORT_ISC_t isc) {
    //XXX
    this->listener = listener;
    /* Configure PC0 as input, triggered on rising edge. */
    PORT_ConfigurePins(port, pinMask, false, false, opc, isc );

    PORT_SetPinsAsInput(port, pinMask);
    // Configure Interrupt
    PORT_ConfigureInterrupt0(port, PORT_INT0LVL_LO_gc, pinMask);
}

//XXX
extern "C" ISR(PORTD_INT1_vect) {
    pinChangeController->listener->onPinChange(&PORTC);
}
extern "C" ISR(PORTD_INT0_vect) {
    pinChangeController->listener->onPinChange(&PORTC);
}
