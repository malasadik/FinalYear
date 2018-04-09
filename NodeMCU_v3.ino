#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

WiFiClient client;
ESP8266WebServer server(80);

const int led = D3; //For test LED pin D4, for real LED use pin D3
const int A = D2; //4
const int B = D1; //2
const int C = D0; //1 example = Y5 is C(1) B(0) A(1) (1 + 0 + 4)

int testCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);

  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  analogWrite(led, 0);
  analogWrite(D4, 0);
  
  WiFiManager wifiManager;
  //wifiManager.resetSettings(); Use this if you mess up the AP
  //wifiManager.setTimeout(180);
  wifiManager.autoConnect();
  //----------------------------------------------------------------------------------------------------------------------
  Serial.println("WiFi connected..");
  server.begin();
  Serial.println("Webserver started...");
  Serial.print("URL: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  server.on("/", root);
  server.on("/power", showPower);
  server.on("/pwm_full", LED_PWM_Full);
  server.on("/pwm_mid", LED_PWM_50);
  server.on("/pwm_low", LED_PWM_25);
  server.on("/pwm_off", LED_PWM_Off);
  server.on("/test", testValues);
}

void LED_PWM_Full(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/html", "Success");
  analogWrite(led, 512);
}

void LED_PWM_50(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", "Success");
  analogWrite(led, 90);
}

void LED_PWM_25(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", "Success");
  analogWrite(led, 50);
}

void LED_PWM_Off(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", "Success");
  analogWrite(led, 0);
}

float getVolts(){
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  float reading = analogRead(A0);
  float volts = reading*15.0/1024.0;
  return volts;
}

void showPower(){
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  float reading = analogRead(A0);
  float val = (reading*3.3)/1024.0;
  float volts = getVolts();
  float powerVal = val * volts;
  String power = String(reading) + ", " + String(powerVal) + "W, " + String(val) + "A, " + String(volts) + "V";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", power);
}

float samples(int sampleNumber){
  float reading = 0;
  for (int i = 0; i < sampleNumber; i++) {
    int reed = analogRead(A0);
    Serial.println(reed);
    reading += reed;
  }
  reading = reading / sampleNumber;
  //Serial.println(testCount);
  return reading;
}

String getPower(){
  digitalWrite(A, LOW);//HIGH
  digitalWrite(B, LOW);//HIGH
  digitalWrite(C, LOW);//LOW
  float reading = samples(100);
  float current = (reading*3.3)/1024.0;
  //float volts = getVolts();
  //float powerVal = val * volts;
  String power = String(reading) + ", " + String(testCount) + ", " + String(current) + "W, ";// + String(val) + "A, " + String(volts) + "V";
  return power;
}

void testValues(){
  String power = "0.0";
  if (testCount < 1024){
    analogWrite(led, testCount);
    delay(2000);
    power = getPower();
    testCount += 100;
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", power);
}

void root(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", "Welcome to the wonderful world of Mala.. We have cookies but they're from 3 years ago.. Eat at own risk!!");
}

void loop() {
//  if (testCount < 1024){
//    analogWrite(led, testCount);
//    delay(1000);
//    Serial.println(getPower());
//    testCount += 10;    
//  }
  server.handleClient();
}
