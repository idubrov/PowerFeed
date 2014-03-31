#include <LiquidCrystal.h>

#include "Menu.h"
#include "Stepper.h"


void setup() {
    g_stepper.setup();
    g_menu.setup();
}

void loop() {
    g_menu.update();
    g_menu.redraw();
}

