#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS 0x27
#define LM35_PIN A1
#define BUTTON_PIN 3
#define NUM_READINGS 100

bool backlightState = true;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

float temperatureReadings[NUM_READINGS] = {0};
int readingIndex = 0;  // Aktualny indeks w tablicy

void setup() {
    Serial.begin(9600);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
}

void loop() {
    int sensorValue = analogRead(LM35_PIN);
    float voltage = sensorValue * (5.0 / 1023.0);
    float temperature = voltage * 100.0;

    temperatureReadings[readingIndex] = temperature;
    readingIndex = (readingIndex + 1) % NUM_READINGS; // Bufor cykliczny

    float sum = 0;
    for (int i = 0; i < NUM_READINGS; i++) {
        sum += temperatureReadings[i];
    }
    float averageTemperature = sum / NUM_READINGS;

    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print(" C | Avg: ");
    Serial.print(averageTemperature);
    Serial.println(" C");

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(temperature);
    lcd.print("C  Avg:");
    lcd.print(averageTemperature, 1);

    int reading = digitalRead(BUTTON_PIN);
    if (reading == LOW && lastButtonState == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
            lastDebounceTime = millis();
            toggleBacklight();
        }
    }
    lastButtonState = reading;

    delay(500);
}

void toggleBacklight() {
    backlightState = !backlightState;
    if (backlightState) {
        lcd.backlight();
    } else {
        lcd.noBacklight();
    }
}
