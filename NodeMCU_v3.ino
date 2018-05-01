#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>

WiFiClient client;
ESP8266WebServer server(80);

const int led = D3; //For test LED pin D4, for real LED use pin D3
const int A = D2; //4
const int B = D1; //2
const int C = D0; //1 example = Y5 is C(1) B(0) A(1) (1 + 0 + 4)

int testCount = 0;
int pwm_val = 0;
float adc_resolution = 3.3/1024.0;
float res_ratio = 11.0/50.0;

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  delay(100);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  analogWrite(led, 0);
  analogWrite(D4, 0);
  
  WiFi.hostname("switch1");
  WiFiManager wifiManager;
  //wifiManager.resetSettings(); Use this if you mess up the AP
  //wifiManager.setTimeout(180);
  wifiManager.autoConnect();
  //----------------------------------------------------------------------------------------------------------------------
  Serial.println("WiFi connected..");

  if (!MDNS.begin("switch2")) {//http://switch1.local
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  
  Serial.println("mDNS responder started");
  server.begin();
  Serial.println("Webserver started...");
  Serial.print("URL: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  MDNS.addService("esp", "tcp", 8080);

  server.on("/", root);
  server.on("/sensor", getSensor);
  server.on("/status", getStatus);
  server.on("/power", showPower);
  server.on("/pwm_full", LED_PWM_Full);
  server.on("/pwm_mid", LED_PWM_50);
  server.on("/pwm_low", LED_PWM_25);
  server.on("/pwm_off", LED_PWM_Off);
  server.on("/test", testValues);
}

void LED_PWM_Full(){
  pwm_val = 1024;
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/html", "Success");
  analogWrite(led, pwm_val);
  //delay(2000);
}

void LED_PWM_50(){
  pwm_val = 512;
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", "Success");
  analogWrite(led, pwm_val);
}

void LED_PWM_25(){
  pwm_val = 256;
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", "Success");
  analogWrite(led, pwm_val);
  //delay(2000);
}

void LED_PWM_Off(){
  pwm_val = 0;
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", "Success");
  analogWrite(led, pwm_val);
  //delay(2000);
}

void getSensor(){
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  delay(1);
  String str = String(analogRead(A0));
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", str);
}

void getStatus(){
  int index = 0;
  if (pwm_val == 256) index = 1;
  else if (pwm_val == 512) index = 2;
  else if (pwm_val == 1024) index = 3;
  else index = 0;
  String str = String(index) + " Success";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", str);
}

float getVolts(){
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  delay(1);
  float reading = samples(100);
  float volts = (reading * adc_resolution) / res_ratio;
  return volts;
}

void showPower(){
  String power = getPower(1);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", power);
}

float samples(int sampleNumber){
  float reading = 0;
  for (int i = 0; i < sampleNumber; i++) {
    int reed = analogRead(A0);
    reading += reed;
    delay(1);
  }
  reading = reading / (float) sampleNumber;
  return reading;
}

String getPower(int choice){
  delay(1000);
  digitalWrite(A, HIGH);//HIGH
  digitalWrite(B, HIGH);//HIGH
  digitalWrite(C, LOW);//LOW
  delay(1);
  float reading = samples(100);
  float current = (reading * adc_resolution * 1000) / 15.0;
  float volts = getVolts();
  float powerVal = current * volts / 1000;
  String power = String(reading) + ", " + String(testCount) + ", " + String(powerVal) + "W, " + String(current) + "mA, " + String(volts) + "V";
  if (choice == 1){
    return String(powerVal) + "W";
  }
  else{
    return power;
  }
}

void testValues(){
  String power = "0.0";
  if (testCount < 1024){
    analogWrite(led, testCount);
    delay(1500);
    power = getPower(2);
    testCount += 50;
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
  server.send(200, "text/plain", "Success");
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
