#ifndef Menu_h
#define Menu_h

#include <Arduino.h>
#include <LiquidCrystal.h>

const uint8_t FAULT_PIN = 11;

const uint8_t STEP_PIN = 0;
const uint8_t DIR_PIN = 1;
const uint8_t ENABLE_PIN = 2;
const uint8_t RESET_PIN = 3;

const uint8_t STEP_PIN_LOW = PORTD0;

class Menu
{
public:
    Menu();

    void initialize();
    void update();
    void redraw();
private:
    uint8_t button();

    LiquidCrystal _lcd;

    uint8_t _toggle;
    uint8_t _ipm;
    unsigned long _pressed_at;
};

#endif
