#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"

const uint8_t BRight = 0;
const uint8_t BUp = 1;
const uint8_t BDown = 2;
const uint8_t BLeft = 3;
const uint8_t BSelect = 4;
const uint8_t BNone = -1;

Menu::Menu() : _lcd(8, 9, 4, 5, 6, 7), _toggle(0), _ipm(1), _pressed_at(0) {
}

void Menu::initialize() {
    _lcd.begin(16, 2);
    _lcd.clear();
    redraw();
}

void Menu::redraw() {
    _lcd.home();
    _lcd.print("Jog");
    _lcd.print(_toggle ? " (Toggle)" : "         ");
    _lcd.setCursor(8, 1);
    _lcd.print("IPM: ");
    _lcd.print(_ipm);
    _lcd.print(" ");
}

void Menu::update() {
    uint8_t b = button();
    if (_pressed_at == 0 || (millis() - _pressed_at) > 200) {
        if (b == BUp && _ipm < 60) {
            _ipm++;
            redraw();
            _pressed_at = millis();
        } else if (b == BDown && _ipm > 1) {
            _ipm--;
            redraw();
            _pressed_at = millis();
        }
    }

    if (b == BNone) {
        _pressed_at = 0;
    } else if(_pressed_at == 0) {
        if (b == BSelect) {
            _toggle = !_toggle;
            redraw();
        }
        _pressed_at = millis();
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
