#include "DHT.h"
const float mean_temp  = 35.905424;
const float mean_humid = 58.115254;
const float std_temp   = 0.687427;
const float std_humid  = 19.940053;
const float theta[] = {0.984221, -0.062587, 6.727087};
DHT dht(4, DHT11);
void setup() {
  Serial.begin(115200);
  dht.begin();
}
float sigmoid(float z) {
  return 1.0 / (1.0 + exp(-z));
}
void loop() {
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();
  // Serial.print(temp);
  // Serial.print(",");
  // Serial.println(humid);
  float scaled_temp=(temp-mean_temp)/std_temp;
  float scaled_humid=(humid-mean_humid)/std_humid;
  float z = (1.0 * theta[0]) + (scaled_temp * theta[1]) + (scaled_humid * theta[2]); //this gets the predicted value
  float probability = sigmoid(z);
  int prediction;
  if (probability<0.5){
    prediction=0;
  }
  else{
    prediction=1;
  }

  if (prediction==1){
    Serial.print("Respiration Spike Event\n");
  }
  else{
   Serial.println("Status: Normal Operational Ambient Space");
  }
  


  delay(2000);
}