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

#ifndef LEDGROUP_H_
#define LEDGROUP_H_

#include "avr_compiler.h"
/* Group of leds */
class LedGroup {
private:
    uint8_t amountOfLeds;
    PORT_t ** ports;
    uint8_t *bitmasks;
public:
    LedGroup(uint8_t maxAmountOfLeds);
    void add(PORT_t * port, uint8_t bitmask, uint8_t isActiveLow);
    void set(uint8_t bitmask);
};

/*
 * Assume that RGB led is a led group of 3 leds,
 * then colors are bitmasks.
 */
typedef enum {
	RED = 0x01,
	GREEN = 0x02,
    BLUE= 0x04,

    ORANGE = 0x03,
    PINK = 0x05,
    SKY = 0x06,

    WHITE = 0x07,
    NONE = 0x00,
} Color_enum;

#endif /* LEDGROUP_H_ */
