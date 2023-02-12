#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "mpSwitch.h"

#define pinSwitch 0
#define pinMotorFw 1
#define pinMotorBw 2
#define pinLED 3
#define pinResistor 4

MPSwitch mps = MPSwitch(pinSwitch);
Adafruit_NeoPixel led(1, pinLED, NEO_GRB);

unsigned long milPrevPbSleep = 0;

unsigned long mil;
unsigned long milSpeedChange;
unsigned long milNextChange = 5000;
// game vars
int gameMode = 2;
int currentSpeed = 0;
int requestedSpeed = 200;
int dir = 1; // 1=fw -1=bw 0=stop

// helper functions
void setMotor(uint8_t speed, int dir = 1)
{
  switch (dir)
  {
  case -1:
    analogWrite(pinMotorFw, 0);
    analogWrite(pinMotorBw, speed);
    break;
  case 0:
    analogWrite(pinMotorFw, 0);
    analogWrite(pinMotorBw, 0);
    break;
  case 1:
  default:
    analogWrite(pinMotorFw, speed);
    analogWrite(pinMotorBw, 0);
    break;
  }
}

void updateLED(int mode)
{
  const int breathing = abs(sin(millis() / 1000.0 * PI)) * 255;
  if (mode >= 1)
    led.setPixelColor(0, (led.Color(breathing, breathing * 0.7, 0))); // breathing yellow
  else
    led.setPixelColor(0, led.Color(0, 200, 200)); // blue
  led.show();
}

// main functions
void setup()
{
  led.begin();
  led.setBrightness(20);
  led.show();

  pinMode(pinResistor, OUTPUT);
  digitalWrite(pinResistor, LOW);

  milSpeedChange = millis();
}

void loop()
{
  mil = millis();
  updateLED(gameMode);

  // pulse resistor to prevent power bank sleep
  if (mil > milPrevPbSleep)
  {
    if (mil < milPrevPbSleep + 250)
    {
      digitalWrite(pinResistor, HIGH);
      // digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      digitalWrite(pinResistor, LOW);
      // digitalWrite(LED_BUILTIN, HIGH);
      milPrevPbSleep = mil + 5000;
    }
  }

  // linear speed change (current -> requested)
  if (mil - milSpeedChange > 5)
  {
    if (currentSpeed < requestedSpeed)
      currentSpeed++;
    else if (currentSpeed > requestedSpeed)
      currentSpeed--;
    setMotor(currentSpeed, dir);
    milSpeedChange = mil;
  }

  // detect gamemode change
  if (mps.poll(mil) && mps.shortPress())
  {
    if (++gameMode > 1)
      gameMode = 0;
  }

  // game is paused
  if (!mps.isOn())
  {
    requestedSpeed = 0;
    return;
  }

  // game is running
  switch (gameMode)
  {
  case 0: // normal mode
    requestedSpeed = 100;
    dir = 1;
    break;
  case 1: // crazy mode
    if (mil < milNextChange)
      break;
    // switch to special modes sometimes
    if (random(1, 50) == 1)
    {
      // reverse hook
      milNextChange += random(500, 700);
      requestedSpeed = 120;
      dir = -1;
      gameMode = 2;
      break;
    }
    if (random(1, 25) == 1)
    {
      // sudden stop
      milNextChange += random(200, 400);
      dir = 0;
      gameMode = 2;
      break;
    }
    // get random speed
    requestedSpeed = 10 * random(1, 35);
    // stop if speed too high
    if (requestedSpeed > 255)
    {
      // but only if currently moving
      if (currentSpeed > 60)
      {
        requestedSpeed = 0;
        milNextChange += random(200, 2000);
        break;
      }
      else
      {
        requestedSpeed = requestedSpeed % 256;
      }
    }
    // speed up if too low
    if (requestedSpeed < 60)
    {
      if (currentSpeed < 60)
        requestedSpeed = 100;
      else
        requestedSpeed = 60;
    }
    // randomly change direction
    if (random(1, 30) == 1)
      dir = -1;
    else
      dir = 1;
    // randomly change duration until next change
    milNextChange += random(1000, 4000);
    break;
  case 2:
    if (mil < milNextChange)
      break;
    gameMode = 1;
    milNextChange += random(1000, 4000);
    dir = 1;
    break;
  }
}
