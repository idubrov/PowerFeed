#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"

const uint8_t BRight = 0;
const uint8_t BUp = 1;
const uint8_t BDown = 2;
const uint8_t BLeft = 3;
const uint8_t BSelect = 4;
const uint8_t BNone = -1;


extern volatile uint8_t _speed;
extern volatile uint8_t _target_speed;

Menu::Menu() : _lcd(8, 9, 4, 5, 6, 7), _toggle(1), _ipm(40), _pressed_at(0) {
}

void Menu::initialize() {
    _lcd.begin(16, 2);
    _lcd.clear();
}

void Menu::redraw() {
    _lcd.home();
    _lcd.print(_toggle ? "Toggle   " : "Hold     ");
    _lcd.print("IPM: ");
    _lcd.print(_ipm / 4);
    _lcd.print(" ");

    _lcd.setCursor(0, 1);
    _lcd.print("Speed: ");
    _lcd.print(_speed / 4);
    _lcd.print(" ");
    _lcd.setCursor(10, 1);
    
    uint8_t fault = digitalRead(FAULT_PIN);
    if (fault == LOW) {
        _lcd.print("!FAULT!");
    } else if (_speed == 0) {
        _lcd.print("       ");
    } else {
        uint8_t dir = digitalRead(DIR_PIN);
        _lcd.print(dir == HIGH ? ">>>>>>" : "<<<<<<");
    }
}

void Menu::update() {
    uint8_t b = button();
    uint8_t speed = _speed;

    if (_pressed_at == 0 || (millis() - _pressed_at) > 200) {
        if (b == BUp && _ipm < 60) {
            // one unit is 1/4 IPM
            _ipm += 4;
            _pressed_at = millis();
        } else if (b == BDown && _ipm > 1) {
            // one unit is 1/4 IPM
            _ipm -= 4;
            _pressed_at = millis();
        }
        // Update target speed when jogging in "toggle" mode
        if (_toggle && _target_speed != 0) {
            _target_speed = _ipm;
        }
    }

    if (b == BNone) {
        _pressed_at = 0;
    } else if(_pressed_at == 0) {
        if (b == BSelect && speed == 0 && _target_speed == 0) {
            // Change modes only when stopped
            _toggle = !_toggle;
        }

        if (_toggle) {
            if (_target_speed != 0 && b != BUp && b != BDown) {
                // Stop when jogging in toggle mode and button is pressed
                _target_speed = 0;
            } else if (speed == 0 && (b == BLeft || b == BRight)) {
                // Jog in toggle mode
                digitalWrite(DIR_PIN, b == BLeft ? LOW : HIGH);
                _target_speed = _ipm;
            }
        }

        // Remember time button was pressed
        _pressed_at = millis();
    }

    if (!_toggle) {
        if (b == BLeft || b == BRight) {
            uint8_t dir = digitalRead(DIR_PIN);
            uint8_t keyDir = b == BLeft ? LOW : HIGH;
            if (dir == keyDir) {
                _target_speed = _ipm;
            } else if (_speed == 0) {
                digitalWrite(DIR_PIN, keyDir); // Change direction
                _target_speed = _ipm;
            }
        } else {
            _target_speed = 0;
        }
    }
}

uint8_t Menu::button() {
    int k = analogRead(0);
    if (k < 50) {
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
