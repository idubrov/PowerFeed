#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu.h"

const uint8_t BRight = 0;
const uint8_t BUp = 1;
const uint8_t BDown = 2;
const uint8_t BLeft = 3;
const uint8_t BSelect = 4;
const uint8_t BNone = -1;

Menu g_menu;

Menu::Menu() : _lcd(8, 9, 4, 5, 6, 7), _toggle(1), _ipm(40), _pressed_at(0) {
}

void Menu::setup() {
    _lcd.begin(16, 2);
    _lcd.clear();
}

void Menu::redraw() {
    _lcd.home();
    _lcd.print(_toggle ? "Toggle   " : "Hold     ");
    _lcd.print("IPM: ");
    _lcd.print(_ipm / 1000);
    _lcd.print(" ");

    _lcd.setCursor(0, 1);
    _lcd.print("Speed: ");
    uint16_t speed = g_stepper.getSpeed();
    _lcd.print(speed / 1000);
    _lcd.print(" ");
    _lcd.setCursor(10, 1);
    
    uint8_t fault = digitalRead(FAULT_PIN);
    if (fault == LOW) {
        _lcd.print("!FAULT!");
    } else if (speed == 0) {
        _lcd.print("       ");
    } else {
        uint8_t dir = digitalRead(DIR_PIN);
        _lcd.print(dir == HIGH ? ">>>>>>" : "<<<<<<");
    }
}

void Menu::update() {
    uint8_t b = button();
    uint16_t speed = g_stepper.getSpeed();
    uint16_t targetSpeed = g_stepper.getTargetSpeed();

    if (_pressed_at == 0 || (millis() - _pressed_at) > 200) {
        if (b == BUp && _ipm < 60) {
            // one unit is 1/1000 IPM
            _ipm += 1000;
            _pressed_at = millis();
        } else if (b == BDown && _ipm > 1) {
            // one unit is 1/1000 IPM
            _ipm -= 1000;
            _pressed_at = millis();
        }
        // Update target speed when jogging in "toggle" mode
        if (_toggle && targetSpeed != 0) {
			digitalWrite(ENABLE_PIN, 1);
            g_stepper.setTargetSpeed(_ipm);
        }
    }

    if (b == BNone) {
        _pressed_at = 0;
    } else if(_pressed_at == 0) {
        if (b == BSelect && speed == 0 && targetSpeed == 0) {
            // Change modes only when stopped
            _toggle = !_toggle;
        }

        if (_toggle) {
            if (targetSpeed != 0 && b != BUp && b != BDown) {
                // Stop when jogging in toggle mode and button is pressed
                g_stepper.setTargetSpeed(0);
            } else if (speed == 0 && (b == BLeft || b == BRight)) {
                // Jog in toggle mode
                digitalWrite(DIR_PIN, b == BLeft ? LOW : HIGH);
                g_stepper.setTargetSpeed(_ipm);
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
				digitalWrite(ENABLE_PIN, 1);
                g_stepper.setTargetSpeed(_ipm);
            } else if (speed == 0) {
                digitalWrite(DIR_PIN, keyDir); // Change direction
				digitalWrite(ENABLE_PIN, 1);
                g_stepper.setTargetSpeed(_ipm);
            }
        } else {
            g_stepper.setTargetSpeed(0);
        }
    }

	if (targetSpeed == 0 && speed == 0) {
		// Not moving -- disable driver outputs
		digitalWrite(ENABLE_PIN, 0);
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
