/*
  ESP8266 mDNS responder sample

  This is an example of an HTTP server that is accessible
  via http://esp8266.local URL thanks to mDNS responder.

  Instructions:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to the ESP8266 board
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Point your browser to http://esp8266.local, you should see a response.

 */


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <aREST.h>

const char* ssid = "E27B Hyperoptic 1Gbps Broadband";
const char* password = "gh2xbd6z";

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

aREST rest = aREST();

int power = 0;

void setup(void)
{  
  Serial.begin(115200);

  rest.set_id("1");
  rest.set_name("smart LED");
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");  

  //rest.variable("power",&power);
  
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

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("esp8266")) { //esp8266.local
    Serial.println("Error setting up MDNS responder!");
    while(1);
  }
  Serial.println("mDNS responder started");
  
  pinMode(D7,OUTPUT);
  
  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");
  
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}

void loop(void)
{
  //power += 1;
  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  unsigned long timeout = millis();
  while(!client.available()){
    if(millis() - timeout > 100) {
      Serial.println("Client timed out");
      //client.stop();
      return;
    }
  }

  
  Serial.println("another one");
  client.print("");
  rest.handle(client);
  delay(1);
  client.print("");
  Serial.println("another one1");
  client.stop();
  //if (power >= 500){
  //  power = 0;
  //  Serial.println("another one2");
  //}
}
