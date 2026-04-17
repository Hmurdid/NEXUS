#include "../display.h"
#include "../keyboard.h"
#include "../shell.h"

void runVoltmeter()   { clearScreen(); drawTitle("Voltmeter");   drawCenteredText("Coming soon", 80, COL_TEXT, 1); while(currentState == STATE_VOLTMETER)   { if(keyPressed('B')) currentState = STATE_SHELL; } }
// runWifiScan реалізовано в wifi_scan.cpp
void runBattery()     { clearScreen(); drawTitle("Battery");     drawCenteredText("Coming soon", 80, COL_TEXT, 1); while(currentState == STATE_BATTERY)     { if(keyPressed('B')) currentState = STATE_SHELL; } }
