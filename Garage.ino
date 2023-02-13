#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "INFINITUM47A4_2.4"
#define STAPSK  "password"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int relay_gpio = 0;

const String postForms = "<html>\
  <head>\
    <title>Web Server IOT AGUILAR</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Garage</h1>\
  </body>\
</html>";

void handleRoot() {  
  server.send(200, "text/html", postForms);  
}

void handleForm() {
  if (server.method() != HTTP_POST) {    
    server.send(405, "text/plain", "Method Not Allowed");    
  } else {    
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(200, "text/plain", message); 
    if (server.argName(0) == "Encender" && server.arg(0)== "1") {     
        Serial.print("Encender \n");
        // Normally Open configuration, send LOW signal to let current flow
        // (if you're usong Normally Closed configuration send HIGH signal)
        digitalWrite(relay_gpio, LOW);
        Serial.println("Current Flowing");        
    } else if (server.argName(0) == "Encender" && server.arg(0)== "0") {
        Serial.print("Apagar \n");
        // Normally Open configuration, send HIGH signal stop current flow
        // (if you're usong Normally Closed configuration send LOW signal)
        digitalWrite(relay_gpio, HIGH);
        Serial.println("Current not Flowing");
    }   
    Serial.print(message + "\n");
  }
}

void handleNotFound() {  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);  
}

void setup(void) {
  pinMode(relay_gpio, OUTPUT);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot); 

  server.on("/postform/", handleForm);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
