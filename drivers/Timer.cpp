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
#include "Timer.h"
#include "avr_compiler.h"

Timer::Timer(TC0_t *tc) {
    this->tc = tc;
    timerType = TC_type_0;
}
Timer::Timer(TC1_t *tc) {
    this->tc = (TC0_t*) tc;
    timerType = TC_type_1;
}

void Timer::enableEventDelay() {
    tc->CTRLD |= TC0_EVDLY_bm;
}

void Timer::disableEventDelay() {
    tc->CTRLD &= ~TC0_EVDLY_bm;
}

void Timer::lockCompareUpdate() {
    tc->CTRLFSET = TC0_LUPD_bm;
}

void Timer::unlockCompareUpdate() {
    tc->CTRLFCLR = TC0_LUPD_bm;
}

void Timer::forceUpdate() {
    tc->CTRLFSET = TC_CMD_UPDATE_gc;
}

void Timer::restart() {
    tc->CTRLFSET = TC_CMD_RESTART_gc;
}

void Timer::setCount(uint16_t count) {
    tc->CNT = count;
}

void Timer::setPeriod(uint16_t period) {
    tc->PER = period;
}

void Timer::setPeriodBuffered(uint16_t period) {
    tc->PERBUF = period;
}

void Timer::setCompareA(uint16_t compareValue) {
    tc->CCABUF = compareValue;
}

void Timer::setCompareB(uint16_t compareValue) {
    tc->CCBBUF = compareValue;
}

uint8_t Timer::getOverflowFlag() {
    return tc->INTFLAGS & TC0_OVFIF_bm;
}

void Timer::clearOverflowFlag() {
    tc->INTFLAGS = TC0_OVFIF_bm;
}

uint8_t Timer::getErrorFlag() {
    return tc->INTFLAGS & TC0_ERRIF_bm;
}

void Timer::clearErrorFlag() {
    tc->INTFLAGS = TC0_ERRIF_bm;
}

uint8_t Timer::getCCAFlag() {
    return tc->INTFLAGS & TC0_CCAIF_bm;
}

void Timer::clearCCAFlag() {
    tc->INTFLAGS = TC0_CCAIF_bm;
}

uint8_t Timer::getCCBFlag() {
    return tc->INTFLAGS & TC0_CCBIF_bm;
}

void Timer::clearCCBFlag() {
    tc->INTFLAGS = TC0_CCBIF_bm;
}

uint8_t Timer::getCaptureA() {
    return tc->CCA;
}

uint8_t Timer::getCaptureB() {
    return tc->CCB;
}

void Timer::configClockSource(TC_CLKSEL_t clockSelection) {
    tc->CTRLA = (tc->CTRLA & ~TC0_CLKSEL_gm) | clockSelection;
}
void Timer::configWGM(TC_WGMODE_t wgm) {
    tc->CTRLB = (tc->CTRLB & ~TC0_WGMODE_gm) | wgm;
}

void Timer::configInputCapture(TC_EVSEL_t eventSource) {
    tc->CTRLD = (tc->CTRLD & ~(TC0_EVSEL_gm | TC0_EVACT_gm)) | eventSource
            | TC_EVACT_CAPT_gc;
}

void Timer::enableCCChannels(uint8_t enableMask) {
    if (timerType == TC_type_0) {
        /* Make sure only CCxEN bits are set in enableMask. */
        enableMask &=
                (TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm);
    } else {
        /* Make sure only CCxEN bits are set in enableMask. */
        enableMask &= (TC1_CCAEN_bm | TC1_CCBEN_bm);
    }
    /* Enable channels. */
    tc->CTRLB |= enableMask;
}

void Timer::disableCCChannels(uint8_t disableMask) {
    if (timerType == TC_type_0) {
        /* Make sure only CCxEN bits are set in disableMask. */
        disableMask &= (TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm
                | TC0_CCDEN_bm);
    } else {
        /* Make sure only CCxEN bits are set in disableMask. */
        disableMask &= (TC0_CCAEN_bm | TC0_CCBEN_bm);
    }
    /* Disable channels. */
    tc->CTRLB &= ~disableMask;
}

void Timer::setOverflowIntLevel(TC_OVFINTLVL_t intLevel) {
    tc->INTCTRLA = (tc->INTCTRLA & ~TC0_OVFINTLVL_gm) | intLevel;
}
void Timer::setErrorIntLevel(TC_ERRINTLVL_t intLevel) {
    tc->INTCTRLA = (tc->INTCTRLA & ~TC0_ERRINTLVL_gm) | intLevel;
}
void Timer::setCCAIntLevel(TC_CCAINTLVL_t intLevel) {
    tc->INTCTRLB = (tc->INTCTRLB & ~TC0_CCAINTLVL_gm) | intLevel;
}
void Timer::setCCBIntLevel(TC_CCBINTLVL_t intLevel) {
    tc->INTCTRLB = (tc->INTCTRLB & ~TC0_CCBINTLVL_gm) | intLevel;
}

void Timer::reset() {
    /* TC must be turned off before a Reset command. */
    tc->CTRLA = (tc->CTRLA & ~TC0_CLKSEL_gm) | TC_CLKSEL_OFF_gc;

    /* Issue Reset command. */
    tc->CTRLFSET = TC_CMD_RESET_gc;
}

void Timer::setFrequencyHz(uint32_t frequency) {
    uint16_t divider = F_CPU / frequency;
    TC_CLKSEL_t clcSelMask = TC_CLKSEL_OFF_gc;
    if (divider > 1024) {
        divider = 1024;
        clcSelMask = TC_CLKSEL_DIV1024_gc;
    } else if (divider > 256) {
        divider = 256;
        clcSelMask = TC_CLKSEL_DIV256_gc;
    } else if (divider > 64) {
        divider = 64;
        clcSelMask = TC_CLKSEL_DIV64_gc;
    } else if (divider > 8) {
        divider = 8;
        clcSelMask = TC_CLKSEL_DIV8_gc;
    } else if (divider > 4) {
        divider = 4;
        clcSelMask = TC_CLKSEL_DIV4_gc;
    } else if (divider > 2) {
        divider = 2;
        clcSelMask = TC_CLKSEL_DIV1_gc;
    } else {
        divider = 1;
        clcSelMask = TC_CLKSEL_DIV1_gc;
    }
    configClockSource(clcSelMask);
    // Since we do not have exact prescalers, we have to adjust period as well. For example, if we need a prescaler of 6,
    // we will use prescaler of 8, but decrease the period to 6/8 of its maximum value.
    setPeriod(F_CPU / frequency / divider - 1);
}
void Timer::setPeriodMs(uint16_t period) {

}
