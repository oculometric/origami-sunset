#include <Arduino.h>

void setup()
{
    pinMode(21, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
    digitalWrite(21, HIGH);
    delay(100);
    digitalWrite(21, LOW);
    delay(100);

    Serial.println("hello");
}