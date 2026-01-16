#include "r_transfer_api.h"
#include <Arduino.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

#include "mainfunctions.h"
#include "errorhandlers.h"
#include "secrets.h"

const char* WIFI_SSID = WIFI_NAME;
const char* WIFI_PASSWORD = WIFI_PWD;
const char* apiKey = API_KEY;

const char* apiHost = "realtime-api.trafiklab.se";
const int apiPort = 443;

WiFiSSLClient wifiClient;
HttpClient http(wifiClient, apiHost, apiPort);

/*
Attempts to connect to WiFi
*/
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected!");
}

/*
Prompts user to name a stop
*/
String getStopName() {
  Serial.println("Stop name:");

  while (!Serial.available()) {}

  String stopName = Serial.readStringUntil('\n');
  stopName.trim();
  stopName.replace(" ", "%20");
  Serial.print("Stop name: ");
  Serial.println(stopName);
  return stopName;
}

/*
Retrieves a stop's stopID
*/
int getStopID() {
  String path = "/v1/stops/name/" + getStopName() + "?key=" + apiKey;
  http.get(path);

  if (!HTTPSStatusCheck(http)) {
    http.stop();
    return -1;
  }

  JsonDocument response;
  String body = http.responseBody();
  if (!deserializationErrorHandler(response, body)) {
    http.stop();
    return -1;
  }

  http.stop();
  return response["stop_groups"][0]["id"].as<int>(); //0 is the first result 
}

/*
Retrieves departures for a given stopID.
Returns true for any error and false for success
*/
bool getDepartures(int stopID) {
  String path = "/v1/departures/" + String(stopID) + "?key=" + apiKey;
  http.get(path);

  if (!HTTPSStatusCheck(http)) {
    http.stop();
    return true;
  }

  JsonDocument response;
  String body = http.responseBody();
  if (!deserializationErrorHandler(response, body)) {
    http.stop();
    return true;
  }

  serializeJson(response, Serial);
  String readless = response["departures"][0]["route"]["designation"];
  Serial.println(readless);

  http.stop();
  return false;
}
