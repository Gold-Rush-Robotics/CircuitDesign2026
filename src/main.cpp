/*
 * Create a PID to do the circuit design challenge
 */

#include <Arduino.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>
#define GREEN_PIN 32
#define YELLOW_PIN 34
#define RED_PIN 36
#define MOTOR_PWR 2
#define MOTOR_ENC 21
// display
#define RS 7 // Can data be written
#define E 6  // Enable
#define D0 38
#define D1 40
#define D2 42
#define D3 44
#define WATER_SENSOR A0 
LiquidCrystal lcd(RS, E, D0, D1, D2, D3);

void writeToDisplay(char *text);

volatile unsigned long pulseCount = 0;
unsigned long prevMillis = 0;
const int interval = 500; // Calculate RPM every 500ms

void RPMInterrupt() {
    // Simple debounce: ignore pulses within 10ms of each other
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 2) {
        pulseCount++;
    }
    lastInterruptTime = interruptTime;
}

float getRPM() {
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0; 
    interrupts();

    float ppr = 20.0; // Updated to your specific motor
    // Formula: (Pulses / PPR) * (60,000ms / Interval_ms)
    float rpm = (pulses / ppr) * (60000.0 / interval);
    return rpm;
}

void setup()
{
    // initialize LED digital pin as an output.
    Serial.begin(9600);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(MOTOR_PWR, OUTPUT);
    pinMode(MOTOR_ENC, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(MOTOR_ENC), RPMInterrupt, FALLING);
    lcd.begin(16, 2);
}

int currentTick = 0;

bool started = false;
float currentRPM = getRPM();
unsigned long currentMillis;
int seconds;
void loop()
{    
    currentMillis = millis();
    seconds = currentMillis / 1000;
    
    // Countdown logic
    if (seconds <= 3) {
        lcd.setCursor(0, 1);
        lcd.print(3 - seconds);
        digitalWrite(MOTOR_PWR, LOW);
        switch(seconds){
            case 3:
                digitalWrite(RED_PIN, HIGH);
            break;
            case 2:
                digitalWrite(YELLOW_PIN, HIGH);
            break;
            case 1:
                digitalWrite(GREEN_PIN, HIGH);
            break;
        }
    } else {
        started = true;
        digitalWrite(MOTOR_PWR, HIGH);
    }
    
    // Only calculate RPM every 500ms
    if (started && (currentMillis - prevMillis >= interval)) {

        currentRPM = getRPM();
        prevMillis = currentMillis; // Update the timer
        if(seconds < 5){
            digitalWrite(YELLOW_PIN, HIGH);
            digitalWrite(RED_PIN, LOW);
            digitalWrite(GREEN_PIN, LOW);
        } else {
            digitalWrite(YELLOW_PIN, LOW);
            if (currentRPM > 90) {
                digitalWrite(GREEN_PIN, HIGH);
                digitalWrite(YELLOW_PIN, LOW);
                digitalWrite(RED_PIN, LOW);
            } else {
                digitalWrite(GREEN_PIN, LOW);
                digitalWrite(YELLOW_PIN, LOW);
                digitalWrite(RED_PIN, HIGH);
            }
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RPM: ");
        lcd.print((int)currentRPM);
    }
}

