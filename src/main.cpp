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
#define MOTOR_ENC 22
//display 
#define RS 7 //Can data be written
#define E 6  //Enable
#define D0 38
#define D1 40
#define D2 42 
#define D3 44
LiquidCrystal lcd(RS, E, D0, D1, D2, D3);


void writeToDisplay(char* text);
void countRPM();
volatile int RPM = 0;
volatile int RPMCount = 0;
unsigned long lastTime = 0;

void setup()
{
  // initialize LED digital pin as an output.
  Serial.begin(9600);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(MOTOR_PWR, OUTPUT);
  pinMode(MOTOR_ENC, INPUT_PULLUP);
  attachInterrupt(MOTOR_ENC, countRPM, FALLING);
  lcd.begin(16,2);
    
}

void loop()
{
    lcd.clear();
    lcd.print("Hello, world!");
    lcd.setCursor(0,1);
    lcd.print(RPMCount);
    digitalWrite(MOTOR_PWR, LOW);
    digitalWrite(RED_PIN, HIGH);
    delay(1000);
    digitalWrite(YELLOW_PIN, HIGH);
    delay(1000);
    digitalWrite(GREEN_PIN, HIGH);
    delay(1000);
    digitalWrite(MOTOR_PWR, HIGH);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);

    delay(3000);
    

}

void countRPM(){
    unsigned long now = millis();
    RPMCount++;
    

}
void writeToDisplay(char* text){
    
}

