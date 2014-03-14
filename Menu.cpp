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
const uint8_t MJog = 0;
const uint8_t MJog_Toggle = 1;
const uint8_t MJog_IPM = 2;
const uint8_t MJog_Toggle_IPM = 3;

const uint8_t _transitions[][5] = {
//   Right  Up             Down        Left    Select
    {-1,    MJog_Toggle,   MJog_Toggle, -1,    MJog_IPM},        // Jog
    {-1,    MJog,          MJog,        -1,    MJog_Toggle_IPM}, // Jog_Toggle
    {-1,    -1,            -1,          -1,    MJog},            // Jog_IPM
    {-1,    -1,            -1,          -1,    MJog_Toggle},     // Jog_Toggle_IPM
};

Menu::Menu() : _lcd(8, 9, 4, 5, 6, 7), _state(MJog), _pressed(0), _ipm(1) {
}

void Menu::initialize() {
    _lcd.begin(16, 2);
    _lcd.clear();
    redraw();
}

void Menu::redraw() {
    _lcd.print("  Jog");
    if (_state == MJog_Toggle || _state == MJog_Toggle_IPM) {
        _lcd.print(" (Toggle)");
    }
    if (_state == MJog || _state == MJog_Toggle) {
        _lcd.setCursor(0, 0);
    } else {
        _lcd.setCursor(6, 1);
    }
    _lcd.print(">");

    _lcd.setCursor(8, 1);
    _lcd.print("IPM: ");
    _lcd.print(_ipm);
    _lcd.print(" ");
}

void Menu::update() {
    uint8_t b = button();
    if (b == BNone) {
        _pressed = 0;
    } else if(_pressed == 0) { // Requires button release
        uint8_t new_state = _transitions[_state][b];
        if (new_state != MNone) {
            _state = new_state;
            _lcd.clear();
            redraw();
        }
    }


    if (_state == MJog_IPM || _state == MJog_Toggle_IPM) {
        if (_pressed == 0 || (millis() - _pressed) > 200) {
            if (b == BUp && _ipm < 60) {
                _ipm++;
                redraw();
                _pressed = millis();
            } else if (b == BDown && _ipm > 1) {
                _ipm--;
                redraw();
                _pressed = millis();
            }
        }
    }
    if (_pressed == 0 && b != BNone) {
        _pressed = millis();
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
