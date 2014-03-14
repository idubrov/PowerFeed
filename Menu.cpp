#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"

const uint8_t BRight = 0;
const uint8_t BUp = 1;
const uint8_t BDown = 2;
const uint8_t BLeft = 3;
const uint8_t BSelect = 4;
const uint8_t BNone = -1;


const uint8_t MNone = -1;
const uint8_t MSettings = 0;
const uint8_t MJog = 1;
const uint8_t MJog_Toggle = 2;

const uint8_t _transitions[3][5] = {
//   Right  Up             Down        Left    Select
    {-1,    MJog_Toggle,   MJog,        -1,    -1},     // Settings
    {-1,    MSettings,     MJog_Toggle, -1,    -1},     // Jog
    {-1,    MJog,          MSettings,   -1,    -1}      // Jog_Toggle
};

Menu::Menu() : _lcd(8, 9, 4, 5, 6, 7), _state(MSettings), _pressed(0) {
}

void Menu::initialize() {
    _lcd.begin(16, 2);
    _lcd.clear();
    redraw();
}

void Menu::redraw() {
    if (_state == MSettings) {
        _lcd.print("Settings");
    } else if (_state == MJog) {
        _lcd.print("Jog");
    } else if (_state == MJog_Toggle) {
        _lcd.print("Jog (Toggle)");
    }
}

void Menu::update() {
    uint8_t b = button();
    if (b == BNone) {
        _pressed = 0;
    } else if(_pressed == 0) {
        _pressed = 1;
        uint8_t new_state = _transitions[_state][b];
        if (new_state != MNone) {
            _state = new_state;
            _lcd.clear();
            redraw();
        }
    }
}

uint8_t Menu::button() {
    int k = analogRead(0);
    if (k < 30) {
        return BRight;
    } else if (k < 180) {
        return BUp;
    } else if (k < 360) {
        return BDown;
    } else if (k < 540) {
        return BLeft;
    } else if (k < 760) {
        return BSelect;
    }
    return BNone;
}
