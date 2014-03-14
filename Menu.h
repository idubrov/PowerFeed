#ifndef Menu_h
#define Menu_h

#include <Arduino.h>
#include <LiquidCrystal.h>

class Menu
{
public:
    Menu();

    void initialize();
    void update();
private:
    void redraw();
    uint8_t button();

    LiquidCrystal _lcd;
    uint8_t _state;
    unsigned long _pressed;
    uint8_t _ipm;
};

#endif
