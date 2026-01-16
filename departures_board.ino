#include "mainfunctions.h"
#include "secrets.h"

const int minute = 60000;
int stopID = -1; //Negative number is error, positive number is success

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Made possible by Trafiklab.se")

  connectToWiFi();

  while (stopID < 0) {
    stopID = getStopID();
  }
}

void loop() {
  bool err = true; //False is success and true is error
  while (err) {
    err = getDepartures(stopID);
    delay(100);
  }

  delay(minute);
}
