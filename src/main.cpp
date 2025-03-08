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
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
const char *ssid = "Personal-0C0-2.4GHz";
const char *password = "3B779BD0C0";
const char* serverName = "http://192.168.0.10:8080/api/v1/register/device";
WiFiClient client;
String DeviceId="";
ESP8266WebServer server(80);
// constants won't change :
const long interval = 5000; // interval at which to blink (milliseconds)
unsigned long previousMillis = 0;
// Define a class to represent the JSON data
class MyData
{
public:
  String name;
  int age;

  MyData(const String &name, int age) : name(name), age(age) {}
};

// Handle POST request with JSON body
// Handler que va a recibir el body del mobile 
void handleRegisterMobile(){

  if (!server.hasHeader("Authorization"))
  {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  String authHeader = server.header("Authorization");
  if (authHeader != "Bearer your_token")
  {
    server.send(403, "text/plain", "Forbidden");
    return;
  }
  if (server.hasArg("plain") == false)
  { // Check if body received
    server.send(400, "text/plain", "Body not received");
    return;
  }
  String body = server.arg("plain");
//coordenadas fransisco de haro 
//-27.38838088127979, -55.904381445366084
  // Parse JSON
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
  }
    // Validate JSON contract
    if (!doc.containsKey("idmobile") || !doc["idmobile"].is<String>()) {
      server.send(400, "application/json", "{\"error\":\"Invalid or missing 'idmobile' field\"}");
      return;
  }
  if (!doc.containsKey("xgeo") || !doc["xgeo"].is<float>()) {
      server.send(400, "application/json", "{\"error\":\"Invalid or missing 'xgeo' field\"}");
      return;
  }
  if (!doc.containsKey("ygeo") || !doc["ygeo"].is<float>()) {
      server.send(400, "application/json", "{\"error\":\"Invalid or missing 'ygeo' field\"}");
      return;
  }
  if (!doc.containsKey("deviceid") || !doc["deviceid"].is<String>()) {
    server.send(400, "application/json", "{\"error\":\"Invalid or missing 'deviceid' field\"}");
    return;
}


    // Extract data from JSON
    String idmobile = doc["idmobile"];
    float xgeo = doc["xgeo"];
    float ygeo = doc["ygeo"];
    String deviceid = doc["deviceid"];
    Serial.print("idmobile: ");
    Serial.println(idmobile);
    Serial.print("xgeo: ");
    Serial.println(xgeo);
    Serial.print("ygeo: ");
    Serial.println(ygeo);
    Serial.print("deviceid: ");
    Serial.println(deviceid);
    HTTPClient http;
    http.begin(client,serverName);
    http.addHeader("Content-Type", "application/json");

    // Create JSON object
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["deviceid"] =deviceid; // Replace with actual temperature reading
    jsonDoc["xposition"]=xgeo;
    jsonDoc["yposition"]=ygeo;
    jsonDoc["mobileid"] = idmobile;
    jsonDoc["status"]="CREATED";
    String requestBody;
    serializeJson(jsonDoc, requestBody);
    
    Serial.println(requestBody);
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }


  // Process the body as needed
  server.send(200, "application/json", "{\"message\":\"POST request received\", \"body\":\"" + body + "\"}");

}


void clearEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("EEPROM borrada");
}

void writeStringToEEPROM(int addrOffset, const String &strToWrite) {
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++) {
      EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  EEPROM.commit();
}

String readStringFromEEPROM(int addrOffset) {
  int len = EEPROM.read(addrOffset);
  char data[len + 1];
  for (int i = 0; i < len; i++) {
      data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[len] = '\0'; // Añadir el terminador nulo
  return String(data);
}



// Handle POST request with JSON body
void handlePostRequestii()
{
  if (server.hasArg("plain") == false)
  { // Check if body received
    server.send(400, "text/plain", "Body not received");
    return;
  }
  String body = server.arg("plain");

  // Parse JSON
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error)
  {
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
void getHelloWord()
{
  server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}

void handlePostRequest()
{
  if (server.hasArg("plain") == false)
  { // Check if body received
    server.send(400, "text/plain", "Body not received");
    return;
  }
  String body = server.arg("plain");
  // Process the body as needed
  server.send(200, "application/json", "{\"message\":\"POST request received\", \"body\":\"" + body + "\"}");
}


void handleGetInfoDevice(){
  if (!server.hasHeader("Authorization"))
  {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  String authHeader = server.header("Authorization");
  if (authHeader != "Bearer your_token")
  {
    server.send(403, "text/plain", "Forbidden");
    return;
  }
   String data= DeviceId;

  server.send(200, "application/json", "{\"info\":\""+  data    +"\"}");
}




void handleAuthorizedRequest(){
  if (!server.hasHeader("Authorization"))
  {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  String authHeader = server.header("Authorization");
  if (authHeader != "Bearer your_token")
  {
    server.send(403, "text/plain", "Forbidden");
    return;
  }

  server.send(200, "application/json", "{\"message\":\"Authorized access\"}");
}

 
// Manage not found URL
void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


// Define routing
void restServerRouting(){
  server.on("/", HTTP_GET, []()
            { server.send(200, F("text/html"),
                          F("Welcome to the REST Web Server")); });
  server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
  server.on("/secure", HTTP_GET, handleAuthorizedRequest);
  server.on("/postEndpoint", HTTP_POST, handlePostRequest); // Declare POST handler
  server.on("/postEndpointii", HTTP_POST, handlePostRequestii);
  server.on("/registermobile", HTTP_POST, handleRegisterMobile);
  server.on("/device",HTTP_GET,handleGetInfoDevice);
}


void setup(void)
{
  Serial.begin(115200);
  EEPROM.begin(1024); 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  String readeeprom = readStringFromEEPROM(0);
  Serial.println(readeeprom);
  if (readeeprom == "") {
      Serial.println("no hay nada en la eeprom");
      writeStringToEEPROM(0, "c474a857-fdd2-42db-9339-7421531303e0");
  } else { 
      Serial.println("si hay algo en la eeprom");
      Serial.println(readeeprom);
      DeviceId = readeeprom;

  }





  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
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
  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // get header
  const char *headerKeys[] = {"Cookie", "Content-Type", "X-test"};
  size_t headerKeysSize = sizeof(headerKeys) / sizeof(char *);
  server.collectHeaders(headerKeys, headerKeysSize);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
Serial.println(DeviceId);
    Serial.println("estamos en un loop");
  }

  server.handleClient();
}




