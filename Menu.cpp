#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"

const uint8_t BRight = 0;
const uint8_t BUp = 1;
const uint8_t BDown = 2;
const uint8_t BLeft = 3;
const uint8_t BSelect = 4;
const uint8_t BNone = -1;


extern volatile uint8_t _direction;
extern volatile uint8_t _speed;
extern volatile uint8_t _target_speed;

Menu::Menu() : _lcd(8, 9, 4, 5, 6, 7), _toggle(1), _ipm(10), _pressed_at(0) {
}

void Menu::initialize() {
    _lcd.begin(16, 2);
    _lcd.clear();
}

void Menu::redraw() {
    _lcd.home();
    _lcd.print(_toggle ? "Toggle   " : "Hold     ");
    _lcd.print("IPM: ");
    _lcd.print(_ipm);
    _lcd.print(" ");

    _lcd.setCursor(0, 1);
    _lcd.print("Speed: ");
    _lcd.print(_speed / 4);
    _lcd.print(" ");
    _lcd.setCursor(10, 1);
    _lcd.print(_speed == 0 ? "      " :
               (_direction ? ">>>>>>" :
                             "<<<<<<"));
}

void Menu::update() {
    uint8_t b = button();
    uint8_t speed = _speed;

    if (_pressed_at == 0 || (millis() - _pressed_at) > 200) {
        if (b == BUp && _ipm < 60) {
            _ipm++;
            _pressed_at = millis();
            // FIXME: update target speed?
        } else if (b == BDown && _ipm > 1) {
            _ipm--;
            _pressed_at = millis();
            // FIXME: update target speed?
        }
        if (_toggle && _target_speed != 0) {
            _target_speed = _ipm * 4;
        }
    }

    if (b == BNone) {
        _pressed_at = 0;
    } else if(_pressed_at == 0) {
        if (b == BSelect && speed == 0) {
            _toggle = !_toggle;
        }

        if (_toggle) {
            if (_target_speed != 0 && b != BUp && b != BDown) {
                _target_speed = 0;
            } else if (speed == 0 && (b == BLeft || b == BRight)) {
                _direction = (b == BLeft) ? 0 : 1;
                _target_speed = _ipm * 4;
            }
        }

        // Remember time we pressed the button
        _pressed_at = millis();
    }

    if (!_toggle) {
        if (b == BLeft) {
            if (_direction == 0) {
                _target_speed = _ipm * 4;
            } else if (_speed == 0) {
                _direction = 0;
                _target_speed = _ipm * 4;
            }
        } else if (b == BRight) {
            if (_direction == 1) {
                _target_speed = _ipm * 4;
            } else if (_speed == 0) {
                _direction = 1;
                _target_speed = _ipm * 4;
            }
        } else {
            _target_speed = 0;
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
