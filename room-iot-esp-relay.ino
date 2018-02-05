#include <ESP8266WiFi.h>
#include "secrets.h"

const String responseOk = String("HTTP/1.1 200 OK\r\n") +
  "Content-Type: text/plain\r\n" +
  "Connection: close\r\n" +
  "\r\n" +
  "Your request has been processed" +
  "\r\n";

const String responseError = String("HTTP/1.1 404 Not Found\r\n") +
  "Content-Type: text/plain\r\n" +
  "Connection: close\r\n" +
  "\r\n" +
  "Unable to process your request" +
  "\r\n";

WiFiServer server(80);

bool isEnabled = false;

String getStatusResponse() {
  return String("HTTP/1.1 200 OK\r\n") +
    "Content-Type: text/plain\r\n" +
    "Connection: close\r\n" +
    "\r\n" +
    (isEnabled ? "on" : "off") +
    "\r\n";
}

void setup() {
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\r');
        if (line.equals(String("GET /status?secret=") + secret + " HTTP/1.1")) {
          client.println(getStatusResponse());
        } else if (line.equals(String("GET /turn/on?secret=") + secret + " HTTP/1.1")) {
          client.println(responseOk);
          digitalWrite(D0, HIGH);
          isEnabled = true;
        } else if (line.equals(String("GET /turn/off?secret=") + secret + " HTTP/1.1")) {
          client.println(responseOk);
          digitalWrite(D0, LOW);
          isEnabled = false;
        } else {
          client.println(responseError);
        }
        break;
      }
    }
    delay(1);

    client.stop();
  }
}
