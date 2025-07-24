/*
 *  systime.c    64-bit, 1-s time base using Timer/Counter 0
 * ATmega32U4, F_CPU = 16 000 000 Hz
 *
 * Created: 7/2/2025 12:04:55 PM
 *  Author: Endeavor360
 *
 *
 * Timer-0 setup
 *   normal (free-running) mode
 */

#include "systime.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint64_t _ovf64 = 0ULL;

/* ---------- initialisation ---------- */
void systime_init(void)
{
    cli();                       /* global IRQs off                     */
    TCCR0A = 0;                  /* normal counting mode                */
    TCCR0B = _BV(CS01);          /* CS01:0 = 010 ? prescaler �8         */
    TIMSK0 = _BV(TOIE0);         /* enable overflow interrupt           */
    sei();                       /* back on                             */
}

/* ---------- overflow ISR (every 128 �s) ---------- */
ISR(TIMER0_OVF_vect)
{
    _ovf64++;                    /* software high-word                  */
}

/* ---------- atomic 64-bit read helper ---------- */
static inline uint64_t atomic64(volatile uint64_t *p)
{
    uint64_t v;
    uint8_t s = SREG; cli();
    v = *p;
    SREG = s;
    return v;
}

/* ---------- 1-microsecond timestamp ---------- */
uint64_t micros64(void)
{
    uint64_t ovf;
    uint8_t  tcnt;

    uint8_t s = SREG; cli();            /* critical section             */
    ovf  = _ovf64;
    tcnt = TCNT0;

    /* if overflow happened after reading TCNT0 but before cli() */
    if ((TIFR0 & _BV(TOV0)) && tcnt < 255)
        ovf++;

    SREG = s;                           /* restore                       */

    /* total half-microseconds ticks = (ovf << 8) | tcnt
       divide by 2 ? shift right once to get whole microseconds                */
    uint64_t half_us = (ovf << 8) | tcnt;
    return half_us >> 1;
}
