#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

WiFiClient client;
ESP8266WebServer server(80);

const int led = D4; //For test LED pin D4, for real LED use pin D3

void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;
  //wifiManager.resetSettings(); Use this if you mess up the AP
  wifiManager.setTimeout(180);
  wifiManager.autoConnect();
  //----------------------------------------------------------------------------------------------------------------------
  Serial.println("WiFi connected..");
  server.begin();
  Serial.println("Webserver started...");
  Serial.print("URL: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
  pinMode(led, OUTPUT);
  
  //Define what the server should do when a client connects
  server.on("/", root);
  server.on("/power", showPower);
  server.on("/pwm_full", LED_PWM_Full);
  server.on("/pwm_mid", LED_PWM_50);
  server.on("/pwm_low", LED_PWM_25);
  server.on("/pwm_off", LED_PWM_Off);
}

void LED_PWM_Full(){
  server.send(200, "text/html", "Success");
  digitalWrite(led, HIGH);
}

void LED_PWM_50(){
  server.send(200, "text/plain", "Success");
  analogWrite(led, 512);
}

void LED_PWM_25(){
  server.send(200, "text/plain", "Success");
  analogWrite(led, 255);
}

void LED_PWM_Off(){
  server.send(200, "text/plain", "Success");
  digitalWrite(led, LOW);
}

void showPower(){
  server.send(200, "text/plain", "JK.. I have nothing to show you");
}

void root(){
  server.send(200, "text/plain", "Welcome to the wonderful world of Mala.. We have cookies but they're from 3 years ago.. Eat at own risk!!");
}

void loop() {
  server.handleClient();
}
