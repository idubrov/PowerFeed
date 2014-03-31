#ifndef SpeedControl_h
#define SpeedControl_h

#define STEP_PIN      A1 // PORTC1
#define DIR_PIN       A2
#define ENABLE_PIN    A3
#define RESET_PIN     A4
#define FAULT_PIN     A5

#define STEP_PIN_PORT   PORTC
#define STEP_PIN_BIT    PORTC1

extern "C" {
    void TIMER1_COMPA_vect();
    void TIMER2_COMPA_vect();
}

class Stepper
{
public:
    void setup();

    inline void __attribute__((always_inline)) setTargetSpeed(uint16_t ts) {
        cli();
        _target_speed = ts;
        sei();
    }

    inline uint16_t __attribute__((always_inline)) getTargetSpeed() {
        // Should be safe to read target speed without disabling interrupts as
        // we never change it in interrupts.
        return _target_speed;
    }

    inline uint16_t __attribute__((always_inline)) getSpeed() {
        cli();
        uint16_t speed = _speed;
        sei();
        return speed;
    }

    inline void __attribute__((always_inline)) setSpeed(uint16_t speed) {
        cli();
        _speed = speed;
        sei();
    }

private:

    // Internal step generator counter, increased by _speed every timer tick.
    // Once counter overflows, a new pulse is generated.
    volatile uint16_t _counter;
    volatile uint16_t _target_speed;
    volatile uint16_t _speed;
    static const uint16_t _acceleration = 256;
    static const uint16_t _decceleration = 256;

    friend void TIMER1_COMPA_vect();
    friend void TIMER2_COMPA_vect();
};
extern Stepper g_stepper;

#endif
