#include <LiquidCrystal.h>
#include "Menu.h"

Menu _menu;

volatile uint8_t _speed = 0;
volatile uint8_t _target_speed = 0;
volatile uint8_t _acceleration = 1;
volatile uint8_t _decceleration = 1;
volatile uint8_t _counter;          // Internal step generator counter

void setup() {
    // Configure fault pin as input with pull-up resistor.
    // Fault condition when pulled low.
    pinMode(FAULT_PIN, INPUT_PULLUP);

    // Configure output pins
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(RESET_PIN, OUTPUT);

    cli();
    // Set step generation timer1
    // CTC mode (OCR1A is top), no prescaler
    TCCR1A = 0;
    TCCR1B = (1<<WGM12) | (1<<CS10);

    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);

    // should be calculated! depends on mechanic configuration...
    OCR1A = (293 - 1);  // one clock for timer reset

    // Set acceleration timer2
    // CTC mode, prescaler set to 1024, match on 100 (78.125 Hz)
    TCCR2A = (1<<WGM21);
    TCCR2B = (1<<CS20) | (1<<CS21) | (1<<CS22);

    OCR2A = 200;

    // enable timer compare interrupt:
    TIMSK2 |= (1 << OCIE2A);

    sei();

    _menu.initialize();
}

void loop() {
    _menu.update();
    _menu.redraw();
}

// We use simple pulse generation algorithm. Every timer 'tick' we add
// speed to pulse_counter. Once pulse_counter overflows, we generate step.

// Note that we never reset counter to zero, even when changing direction.
// It is assumed that minor error accumulated in counter would not have any
// effect, as we are not doing precise positioning.
ISR(TIMER1_COMPA_vect) {
    // This routine is critical. We don't want anything else to run in parallel!
    // Note: assume r0 and r1 are preserved in stack.
    asm volatile(
            "cli\n\t"
            "lds r0, _counter\n\t"
            "lds r1, _speed\n\t"
            "add r0, r1\n\t"
            "sts _counter, r0\n\t"
            "brcc 1f\n\t"
            // Overflow occurred, send step pulse
            "sbi %0, %1\n\t"
            "nop\n\t"
            "nop\n\n"
            "cbi %0, %1\n\t"
            "1: \n\t"
            // Note: we use low-level access to the ports for performance
            :: "I" (_SFR_IO_ADDR(PORTD)), "I" (PORTD0)
            );
}

// Acceleration/decceleration vector
ISR(TIMER2_COMPA_vect) {
    uint8_t speed  = _speed;
    uint8_t target = _target_speed;
    if (speed <= target) {
        uint8_t accel  = _acceleration;
        // Accelerating
        if ((target - speed) > accel) {
            speed += accel;
        } else {
            // Too fast -- clamp at the target speed
            speed = target;
        }
    } else {
        uint8_t deccel  = _decceleration;
        // Deccelerating
        if ((speed - target) > deccel) {
            speed -= deccel;
        } else {
            // Too slow -- clamp at the target speed
            speed = target;
        }
    }

    // Update speed
    _speed = speed;
}

