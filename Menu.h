#ifndef Menu_h
#define Menu_h

#include <Arduino.h>
#include <LiquidCrystal.h>

#define STEP_PIN      A1 // PORTC1
#define DIR_PIN       A2
#define ENABLE_PIN    A3
#define RESET_PIN     A4
#define FAULT_PIN     A5

#define STEP_PIN_PORT   PORTC
#define STEP_PIN_BIT    PORTC1

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
