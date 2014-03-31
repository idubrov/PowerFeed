#ifndef Menu_h
#define Menu_h

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Stepper.h"

class Menu
{
public:
    Menu();

    void setup();
    void update();
    void redraw();
private:
    uint8_t button();

    LiquidCrystal _lcd;

    uint8_t _toggle;
    uint16_t _ipm;
    unsigned long _pressed_at;
};
extern Menu g_menu;

#endif
