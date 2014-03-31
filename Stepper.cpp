#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"

Stepper g_stepper;


// The algorithm for the pulse generator is the following:
// 1) Every time timer fires, we add speed value to the counter
// 2) Once counter overflows, we generate pulse for the stepper motor driver
//
// Here is the formula we use to calculate OCR1A value (pulse generator timer):
//
// CPU_Frequency = 16000000 per sec
// Prescaler = 1
// Timer_Frequency = CPU_Frequency / Prescaler = 16000000
// Counter_Overflow = 65536 (16-bit counter)
// 
// Leadscrew = 16 TPI
// Steps  = 200 steps/rev
// Microsteps = 16 pulses/step
// Pulses/r = Steps * Microsteps = 3200 pulses/rev
// Pulses/i = Pulses/r * Leadscrew = 51200 pulses/inch
//
// Target_IPM = 0.001 inches per unit of speed
// Timer_Top = 60 * Timer_Frequency / (Target_IPM * Pulses/i * Counter_Overflow) = 286
// OCR1A = Timer_TOP
void Stepper::setup() {
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
    // Value for 16-bit speed, 16 microsteps
    OCR1A = (286 - 1);  // one clock for timer reset

    // Set acceleration timer2
    // CTC mode, prescaler set to 1024, match on 100 (78.125 Hz)
    TCCR2A = (1<<WGM21);
    TCCR2B = (1<<CS20) | (1<<CS21) | (1<<CS22);

    OCR2A = 200; // why 200?

    // enable timer compare interrupt:
    TIMSK2 |= (1 << OCIE2A);

    sei();

	// Enable driver, disable outputs
	digitalWrite(RESET_PIN, 1);
	digitalWrite(ENABLE_PIN, 0);
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
            // low nible
            "lds r0, %[counter]\n\t"
            "lds r1, %[speed]\n\t"
            "add r0, r1\n\t"
            "sts %[counter], r0\n\t"
            // high nible
            "lds r0, %[counter]+1\n\t"
            "lds r1, %[speed]+1\n\t"
            "adc r0, r1\n\t"
            "sts %[counter]+1, r0\n\t"

            "brcc 1f\n\t"
            // Overflow occurred, send step pulse
            // Note: we use low-level access to the ports for performance
            "sbi %[port], %[step_bit]\n\t"
            "nop\n\t"
            "nop\n\n"
            "cbi %[port], %[step_bit]\n\t"
            "1: \n\t"
            : 
            :
                [port] "I" (_SFR_IO_ADDR(STEP_PIN_PORT)),
                [step_bit] "I" (STEP_PIN_BIT),
                [counter] "o" (g_stepper._counter),
                [speed] "o" (g_stepper._speed)
            :   "r0", "r1"
            );
}

// Acceleration/decceleration vector
ISR(TIMER2_COMPA_vect) {
    // Don't disable interrupts, we are the only ones to change it.
    uint16_t speed  = g_stepper._speed;
    // Don't disable interrupts, only main loop changes it.
    uint16_t target = g_stepper._target_speed;
    
    if (speed <= target) {
        uint16_t accel  = Stepper::_acceleration;
        // Accelerating
        if ((target - speed) > accel) {
            speed += accel;
        } else {
            // Too fast -- clamp at the target speed
            speed = target;
        }
    } else {
        uint16_t deccel  = Stepper::_decceleration;
        // Deccelerating
        if ((speed - target) > deccel) {
            speed -= deccel;
        } else {
            // Too slow -- clamp at the target speed
            speed = target;
        }
    }

    g_stepper.setSpeed(speed);
}

