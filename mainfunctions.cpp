#include "r_transfer_api.h"
#include <Arduino.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

#include "mainfunctions.h"
#include "errorhandlers.h"
#include "secrets.h"

String apiProvider = "trafiklab";  //Either resrobot or trafiklab
const char* WIFI_SSID = WIFI_NAME;
const char* WIFI_PASSWORD = WIFI_PWD;
const char* apiKey;
const char* apiHost;

const int apiPort = 443;
WiFiSSLClient wifiClient;
HttpClient* http;

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

void apiIntialiaztion() {
  if (apiProvider == "trafiklab") {
    apiKey = API_KEY_TRAFIKLAB;
    apiHost = "realtime-api.trafiklab.se";
  } else {
    apiKey = API_KEY_RESROBOT;
    apiHost = "api.resrobot.se";
  }

  http = new HttpClient(wifiClient, apiHost, apiPort);
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
  String path;
  if (apiProvider == "trafiklab") {
    path = "/v1/stops/name/" + getStopName() + "?key=" + apiKey;
  } else {
    path = "/v2.1/location.name?input=" + getStopName() + "&format=json&accessId=" + apiKey;
  }

  http->get(path);
  if (!HTTPSStatusCheck(*http)) {
    http->stop();
    return -1;
  }

  JsonDocument response;
  String body = http->responseBody();
  if (!deserializationErrorHandler(response, body)) {
    http->stop();
    return -1;
  }

  http->stop();

  // 0 is the first result
  if (apiProvider == "trafiklab") {
    return response["stop_groups"][0]["id"].as<int>();
  } else {
    return response["stopLocationOrCoordLocation"][0]["StopLocation"]["extId"].as<int>();
  }
}

/*
Retrieves departures for a given stopID.
Returns a bool thats true for any error and false for success
*/
bool getDepartures(int stopID) {
  String path;
  if (apiProvider == "trafiklab") {
    path = "/v1/departures/" + String(stopID) + "?key=" + apiKey;
  } else {
    path = "/v2.1/departureBoard?id=" + String(stopID) + "&format=json&accessId=" + apiKey;
  }

  http->get(path);
  if (!HTTPSStatusCheck(*http)) {
    http->stop();
    return true;
  }

  JsonDocument response;
  String body = http->responseBody();
  if (!deserializationErrorHandler(response, body)) {
    http->stop();
    return true;
  }

  http->stop();
  return false;
}
