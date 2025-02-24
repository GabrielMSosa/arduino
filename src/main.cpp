/*
 *  Simple hello world Json REST response
  *  by Mischianti Renzo <https://mischianti.org>
 *
 *  https://mischianti.org/
 *
 */
 
 #include "Arduino.h"
 #include <ESP8266WiFi.h>
 #include <WiFiClient.h>
 #include <ESP8266WebServer.h>
 #include <ESP8266mDNS.h>
 #include <ArduinoJson.h>

 
 const char* ssid = "Personal-0C0-2.4GHz";
 const char* password = "3B779BD0C0";
 ESP8266WebServer server(80);
  
// Define a class to represent the JSON data
class MyData {
  public:
      String name;
      int age;
  
      MyData(const String& name, int age) : name(name), age(age) {}
  };
  
  // Handle POST request with JSON body
  void handlePostRequestii() {
      if (server.hasArg("plain") == false) { // Check if body received
          server.send(400, "text/plain", "Body not received");
          return;
      }
      String body = server.arg("plain");
  
      // Parse JSON
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, body);
      if (error) {
          server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
          return;
      }
  
      // Extract data from JSON
      String name = doc["name"];
      int age = doc["age"];
  
      // Create an instance of MyData
      MyData data(name, age);
  
      // Process the data as needed
      // For example, you can print it to the serial monitor
      Serial.print("Name: ");
      Serial.println(data.name);
      Serial.print("Age: ");
      Serial.println(data.age);
  
      // Send response
      server.send(200, "application/json", "{\"message\":\"POST request received\", \"name\":\"" + data.name + "\", \"age\":" + String(data.age) + "}");
  }
  


 // Serving Hello world
 void getHelloWord() {
     server.send(200, "text/json", "{\"name\": \"Hello world\"}");
 }
  
 void handlePostRequest() {
  if (server.hasArg("plain") == false) { // Check if body received
      server.send(400, "text/plain", "Body not received");
      return;
  }
  String body = server.arg("plain");
  // Process the body as needed
  server.send(200, "application/json", "{\"message\":\"POST request received\", \"body\":\"" + body + "\"}");
}

 void handleAuthorizedRequest() {
  if (!server.hasHeader("Authorization")) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  String authHeader = server.header("Authorization");
  if (authHeader != "Bearer your_token") {
    server.send(403, "text/plain", "Forbidden");
    return;
  }

  server.send(200, "application/json", "{\"message\":\"Authorized access\"}");
}


 // Define routing
 void restServerRouting() {
     server.on("/", HTTP_GET, []() {
         server.send(200, F("text/html"),
             F("Welcome to the REST Web Server"));
     });
     server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
     server.on("/secure", HTTP_GET, handleAuthorizedRequest);
     server.on("/postEndpoint", HTTP_POST, handlePostRequest); // Declare POST handler
     server.on("/postEndpointii", HTTP_POST, handlePostRequestii); 
 }
  
 // Manage not found URL
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
   Serial.begin(115200);
   WiFi.mode(WIFI_STA);
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
  
   // Activate mDNS this is used to be able to connect to the server
   // with local DNS hostmane esp8266.local
   if (MDNS.begin("esp8266")) {
     Serial.println("MDNS responder started");
   }
   
   // Set server routing
   restServerRouting();
   // Set not found response
   server.onNotFound(handleNotFound);
//get header
const char *headerKeys[] = {"Cookie","Content-Type","X-test"};  
size_t headerKeysSize = sizeof(headerKeys)/sizeof(char*);
server.collectHeaders( headerKeys, headerKeysSize );

   // Start server
   server.begin();
   Serial.println("HTTP server started");
 }
  
 void loop(void) {
   server.handleClient();
 }