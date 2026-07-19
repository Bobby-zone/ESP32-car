#include <Arduino.h>

const int ain1 = 23;
const int ain2 = 22;
const int pwma = 21;

const int bin1 = 19;
const int bin2 = 18;
const int pwmb = 17;

const int pota = 32;
const int potb = 33;

int motorSpeedA = 0;
int motorSpeedB = 0;

const int pwmaCh = 0;
const int pwmbCh = 1;
const int pwmFreq = 25000;
const int pwmResolution = 8;

void setup() {
  Serial.begin(115200);

  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(pwma, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(pwmb, OUTPUT);

  pinMode(pota, INPUT);
  pinMode(potb, INPUT);

  ledcSetup(pwmaCh, pwmFreq, pwmResolution);
  ledcAttachPin(pwma, pwmaCh);
  ledcSetup(pwmbCh, pwmFreq, pwmResolution);
  ledcAttachPin(pwmb, pwmbCh);
}

void loop() {

  motorSpeedA = map(analogRead(pota), 0, 4096, -255, 255);
  if (motorSpeedA < -8) {
    // Reverse
    digitalWrite(ain1, HIGH);
    digitalWrite(ain2, LOW);
    ledcWrite(pwmaCh, -motorSpeedA); // absolute value
  } else if (motorSpeedA > 8) {
    // Forward
    digitalWrite(ain1, LOW);
    digitalWrite(ain2, HIGH);
    ledcWrite(pwmaCh, motorSpeedA);
  } else {
    // Stop
    digitalWrite(ain1, LOW);
    digitalWrite(ain2, LOW);
    ledcWrite(pwmaCh, 0);
  }
  motorSpeedB = map(analogRead(potb), 0, 4096, -255, 255);
  if (motorSpeedB < -8) {
    // Reverse
    digitalWrite(bin1, HIGH);
    digitalWrite(bin2, LOW);
    ledcWrite(pwmbCh, -motorSpeedB); // absolute value
  } else if (motorSpeedB > 8) {
    // Forward
    digitalWrite(bin1, LOW);
    digitalWrite(bin2, HIGH);
    ledcWrite(pwmbCh, motorSpeedB);
  } else {
    // Stop
    digitalWrite(bin1, LOW);
    digitalWrite(bin2, LOW);
    ledcWrite(pwmbCh, 0);
  }
}