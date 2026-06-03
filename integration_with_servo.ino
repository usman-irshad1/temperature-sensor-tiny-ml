#include "DHT.h"
#include <ESP32Servo.h>

const float mean_temp  = 35.905424;
const float mean_humid = 58.115254;
const float std_temp   = 0.687427;
const float std_humid  = 19.940053;
const float theta[] = {0.984221, -0.062587, 6.727087};
const int pin = 18; 

Servo motor;
DHT dht(4, DHT11);

unsigned long lastSensorReadTime = 0;
const int sensorInterval = 2000; 

unsigned long lastMotorMoveTime = 0;
int motorInterval = 25; 

int currentPos = 0;       
int currentPrediction = 0; 
bool jitterState = false; 
int sweepStep = 1; 

void setup() {
  dht.begin();
  ESP32PWM::allocateTimer(0); 
  motor.setPeriodHertz(50); 
  motor.attach(pin, 500, 2400); 
  motor.write(currentPos); 
}

float sigmoid(float z) {
  return 1.0 / (1.0 + exp(-z));
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastSensorReadTime >= sensorInterval) {
    lastSensorReadTime = currentTime; 
    
    float temp = dht.readTemperature();
    float humid = dht.readHumidity();

    if (!isnan(temp) && !isnan(humid)) {
      float scaled_temp = (temp - mean_temp) / std_temp;
      float scaled_humid = (humid - mean_humid) / std_humid;
      float z = (1.0 * theta[0]) + (scaled_temp * theta[1]) + (scaled_humid * theta[2]);
      float probability = sigmoid(z);

      int oldPrediction = currentPrediction;

      if (probability < 0.5) {
        currentPrediction = 0;
      } else {
        currentPrediction = 1; 
      }

      if (currentPrediction == 0 && oldPrediction == 1) {
        sweepStep = 1; 
      }
    }
  }

  if (currentTime - lastMotorMoveTime >= motorInterval) {
    lastMotorMoveTime = currentTime; 

    if (currentPrediction == 1) {
      motorInterval = 150; 
      
      if (jitterState) {
        currentPos = 45; 
      } else {
        currentPos = 0;  
      }
      motor.write(currentPos);
      jitterState = !jitterState; 
      
    } 
    else {
      motorInterval = 25; 
      
      currentPos += sweepStep; 
      
      if (currentPos >= 180) {
        currentPos = 180;
        sweepStep = -1; 
      } 
      else if (currentPos <= 0) {
        currentPos = 0;
        sweepStep = 1;  
      }

      motor.write(currentPos);
    }
  }
}