/**
 * @file main.cpp
 * @brief Program 04: Button Debounce State Machine - STM32
 */

#include <Arduino.h>

// ==================== KONFIGURASI ====================
#define BUTTON_PIN      PB0           // Push button (internal pull-up)
#define LED_PIN         LED_BUILTIN   // Onboard LED (Blue Pill: PC13, active LOW)
#define DEBOUNCE_MS     50
#define LED_SELF_TEST_BLINKS 0

#define BUTTON_INPUT_MODE     INPUT_PULLUP
#define BUTTON_ACTIVE_STATE   LOW
#define BUTTON_INACTIVE_STATE HIGH

#define LED_ON_LEVEL  LOW
#define LED_OFF_LEVEL HIGH

// ==================== STATE MACHINE ====================
typedef enum {
    BTN_IDLE,
    BTN_DEBOUNCE,
    BTN_PRESSED,
    BTN_RELEASED
} ButtonState_t;

// ==================== VARIABEL ====================
ButtonState_t buttonState = BTN_IDLE;
bool lastButtonRead = BUTTON_INACTIVE_STATE;
unsigned long debounceStartTime = 0;
unsigned long pressStartTime = 0;
uint32_t pressCount = 0;
bool ledState = false;

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n========================================");
    Serial.println("Program 04: Button Debounce - STM32");
    Serial.println("========================================\n");
    
    pinMode(BUTTON_PIN, BUTTON_INPUT_MODE);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LED_OFF_LEVEL);  // LED OFF (active LOW)

    Serial.printf("Button: PB0 (INPUT_PULLUP), LED: LED_BUILTIN (active LOW)\n");
    Serial.println("Button mode: INTERNAL PULL-UP (ACTIVE LOW)");
    Serial.println("Startup LED self-test on LED_BUILTIN...");

    for (int i = 0; i < LED_SELF_TEST_BLINKS; i++) {
        digitalWrite(LED_PIN, LED_ON_LEVEL);
        delay(200);
        digitalWrite(LED_PIN, LED_OFF_LEVEL);
        delay(200);
    }

    Serial.println("Self-test done. Press button to toggle LED\n");
}

// ==================== LOOP ====================
void loop() {
    bool currentRead = digitalRead(BUTTON_PIN);
    
    switch (buttonState) {
        case BTN_IDLE:
            if (currentRead == BUTTON_ACTIVE_STATE &&
                lastButtonRead == BUTTON_INACTIVE_STATE) {
                buttonState = BTN_DEBOUNCE;
                debounceStartTime = millis();
            }
            break;
            
        case BTN_DEBOUNCE:
            if (millis() - debounceStartTime >= DEBOUNCE_MS) {
                if (currentRead == BUTTON_ACTIVE_STATE) {
                    buttonState = BTN_PRESSED;
                    pressStartTime = millis();
                    
                    // Toggle LED (active LOW)
                    pressCount++;
                    ledState = !ledState;
                    digitalWrite(LED_PIN, ledState ? LED_ON_LEVEL : LED_OFF_LEVEL);
                    
                    Serial.printf("Button PRESSED #%lu - LED %s\n",
                                 pressCount, ledState ? "ON" : "OFF");
                } else {
                    buttonState = BTN_IDLE;
                }
            }
            break;
            
        case BTN_PRESSED:
            if (currentRead == BUTTON_INACTIVE_STATE) {
                buttonState = BTN_RELEASED;
                debounceStartTime = millis();
            }
            break;
            
        case BTN_RELEASED:
            if (millis() - debounceStartTime >= DEBOUNCE_MS) {
                if (currentRead == BUTTON_INACTIVE_STATE) {
                    unsigned long duration = millis() - pressStartTime;
                    Serial.printf("Button RELEASED - Duration: %lu ms\n\n", duration);
                    buttonState = BTN_IDLE;
                } else {
                    buttonState = BTN_PRESSED;
                }
            }
            break;
    }
    
    lastButtonRead = currentRead;
}

/**
 * WIRING BUTTON (INTERNAL PULL-UP / ACTIVE LOW):
 *   PB0 ----[Button]---- GND
 *   (pinMode PB0 = INPUT_PULLUP)
 *
 * LED ONBOARD (LED_BUILTIN, ACTIVE LOW):
 *   LOW = LED ON, HIGH = LED OFF
 */
