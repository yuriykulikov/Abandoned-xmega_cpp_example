/*
 * PinChangeController.h
 *
 *  Created on: Jan 7, 2012
 *      Author: Yuriy
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
