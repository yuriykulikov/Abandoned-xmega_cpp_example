/**
 * PinChangeController.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: Yuriy
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
