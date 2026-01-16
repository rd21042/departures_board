#include "HttpClient.h"
#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

#include "errorhandlers.h"

/*
Checks if HTTPS request was successfull.
Returns true for success and false for any error
*/
bool HTTPSStatusCheck(HttpClient http) {
  int statusCode = http.responseStatusCode();
  Serial.print("Status: ");
  Serial.print(statusCode);
  Serial.println(" (200 is success)");

  return (statusCode == 200) ? true : false; //200 is success
}

/*
Checks if deserialization was successfull.
Returns true for success and false for any error
*/
bool deserializationErrorHandler(JsonDocument& response, const String body) {
  DeserializationError err = deserializeJson(response, body);
  if (err) {
    Serial.print("Deserialization failed: ");
    Serial.println(err.c_str());
    return false;
  }
  return true;
}
