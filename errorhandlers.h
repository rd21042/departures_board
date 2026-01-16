#ifndef ERRORHANDLERS_H
#define ERRORHANDLERS_H

bool HTTPSStatusCheck(HttpClient http);
bool deserializationErrorHandler(JsonDocument& response, const String body);

#endif
