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
#ifndef TIMER_H_
#define TIMER_H_
#include "avr_compiler.h"

enum TimerType {
    TC_type_0,
    TC_type_1,
};

/**
 * Wrapper for TC0_t and TC1_t
 */
class Timer {
private:
    /** this is pointer to the actual TC struct in volatile memory. It could be TC0_t or TC1_t*/
    TC0_t *tc;
    TimerType timerType;
public:
    Timer(TC0_t *tc);
    Timer(TC1_t *tc);

    uint16_t getCount() __attribute__ ( (always_inline) ) {
        return tc->CNT;
    }
    void enableEventDelay();
    void disableEventDelay();
    void lockCompareUpdate();
    void unlockCompareUpdate();
    void forceUpdate();
    void restart();
    void setCount(uint16_t count);
    void setPeriod(uint16_t period);
    void setPeriodBuffered(uint16_t period);
    void setCompareA(uint16_t compareValue);
    void setCompareB(uint16_t compareValue);
    uint8_t getOverflowFlag();
    void clearOverflowFlag();
    uint8_t getErrorFlag();
    void clearErrorFlag();
    uint8_t getCCAFlag();
    void clearCCAFlag();
    uint8_t getCCBFlag();
    void clearCCBFlag();
    uint8_t getCaptureA();
    uint8_t getCaptureB();

    void configClockSource(TC_CLKSEL_t clockSelection );
    void configWGM(TC_WGMODE_t wgm );
    void configInputCapture(TC_EVSEL_t eventSource );
    void enableCCChannels(uint8_t enableMask );
    void disableCCChannels(uint8_t disableMask );
    void setOverflowIntLevel(TC_OVFINTLVL_t intLevel );
    void setErrorIntLevel(TC_ERRINTLVL_t intLevel );
    void setCCAIntLevel(TC_CCAINTLVL_t intLevel );
    void setCCBIntLevel(TC_CCBINTLVL_t intLevel );
    void reset();
    void setFrequencyHz(uint32_t frequency);
    void setPeriodMs(uint16_t period);
};

#endif /* TIMER_H_ */
