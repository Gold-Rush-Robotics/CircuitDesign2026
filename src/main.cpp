/*
 * Create a PID to do the circuit design challenge
 */

#include <Arduino.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
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
Adafruit_INA219 ina219;

void writeToDisplay(char *text);

volatile unsigned long pulseCount = 0;
unsigned long prevMillis = 0;
const int SCREEN_UPDATE_INTERVAL = 500;
const int RPM_UPDATE_INTERVAL = 500;

void RPMInterrupt()
{
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 2)
    {
        pulseCount++;
    }
    lastInterruptTime = interruptTime;
}

float getRPM()
{
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    float ppr = 20.0;
    float rpm = (pulses / ppr) * (60000.0 / RPM_UPDATE_INTERVAL);
    return rpm;
}

void setup()
{
    Serial.begin(9600);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(MOTOR_PWR, OUTPUT);
    pinMode(MOTOR_ENC, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(MOTOR_ENC), RPMInterrupt, FALLING);
    uint32_t currentFrequency;
    ina219.begin();
    lcd.begin(16, 2);
}

int currentTick = 0;

bool started = false;
bool isRamping = false;

float currentRPM = getRPM();
unsigned long currentMillis;
int seconds;
unsigned long rampStartTime = 0;
void loop()
{    
    
    if(analogRead(A0) < 90){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("No Water");
        digitalWrite(GREEN_PIN, LOW);
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(MOTOR_PWR, LOW);
        currentTick = 0;
        started = false;
        currentRPM = getRPM();
        currentMillis = 0;
        pulseCount = 0;
        prevMillis = 0;
        seconds = 0;
        delay(50);
    } else {
    currentMillis = millis();
    seconds = currentMillis / 1000;

    if (seconds < 3)
    {
        lcd.setCursor(0, 1);
        lcd.print(3 - seconds);
        digitalWrite(MOTOR_PWR, LOW);
        switch (seconds)
        {
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
    }
    else
    {
        started = true;
    }

    if (started)
    {
        if (isRamping)
        {
            if (rampStartTime == 0)
                rampStartTime = millis();
            unsigned long elapsed = millis() - rampStartTime;

            int target = map(elapsed, 0, 2000, 100, 255);
            target = constrain(target, 0, 255);

            analogWrite(MOTOR_PWR, target);
        }

        if (seconds < 5)
        {
            isRamping = true;
            digitalWrite(YELLOW_PIN, HIGH);
            digitalWrite(RED_PIN, LOW);
            digitalWrite(GREEN_PIN, LOW);

            int remainingMS = millis() - 3000;

            int msPerTick = 2000 / 255;

        }
        else
        {
            digitalWrite(MOTOR_PWR, HIGH);
            isRamping = false;
            digitalWrite(YELLOW_PIN, LOW);
            if (currentRPM > 90)
            {
                digitalWrite(GREEN_PIN, HIGH);
                digitalWrite(YELLOW_PIN, LOW);
                digitalWrite(RED_PIN, LOW);
            }
            else
            {
                digitalWrite(MOTOR_PWR, HIGH);
            }
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
            lcd.setCursor(0, 1);
            lcd.print("Current: ");
            lcd.print((float)ina219.getCurrent_mA());
        }
    }

    if (started && (currentMillis - prevMillis >= SCREEN_UPDATE_INTERVAL))
    {

        lcd.setCursor(0, 0);
        lcd.print("RPM: ");
        if (isRamping)
        {
            lcd.clear();
            lcd.print("RAMPING");
        }
        else
        {
            lcd.clear();
            lcd.print((int)currentRPM);
        }
    }
}
