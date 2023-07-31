#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 1

#include <FastLED.h>
#include <ESP8266WiFi.h>

#define NUM_LEDS 8
#define LED_PIN D3
#define CLK_PIN D2

CRGB leds[NUM_LEDS];

//Parametre de connexion
const char* ssid     = "";
const char* password = "";

const char* host = "192.168.0.15";


void setup() {
  
  Serial.begin(9600);
  
  delay(3000);
  //FASTLED
  FastLED.addLeds<APA102, LED_PIN, CLK_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  FastLED.setBrightness(5);
  

  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }

FastLED.show();

//Wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);

    WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());



}

int value = 0;
int eventsGet = 0;
CRGB colorTab[8] = {CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::Pink, CRGB::Purple, CRGB::Brown, CRGB::Cyan, CRGB::Green};
String numMusic = "2";

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void showOrHide(String event, int note){

}


void doPiano(int event, int note ){

    leds[note-1] = colorTab[event*note];
   FastLED.show();
  
}


void AreYouReady(){
  
  for(int i = 0; i < NUM_LEDS; i++){
    delay(500);
    leds[i] = CRGB(0, 0, 255);
      FastLED.show();
  }
  delay(100);
   for(int i = 0; i < NUM_LEDS; i++){
    delay(500);
    leds[i] = CRGB::Black;
      FastLED.show();
  }


  leds[0] = CRGB(0, 0, 255);
    FastLED.show();
  delay(1000);
  
  leds[1] = CRGB(0, 255, 255);
    FastLED.show();
  delay(1000);
  leds[2] = CRGB::Green;
    FastLED.show();
  delay(1000);

   for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
      FastLED.show();
  }
   delay(500);
  

  
}


void loop() {

  //D'abord eteindre les leds ensuite gerer les ON OFF dans deux fonction differente
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 8080;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/musics/"+ numMusic;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
 String valActive1 = "";
 String tempo1 = "";
 String note1 = "";

 String valActive2 = "";
 String tempo2 = "";
 String note2 = "";

 AreYouReady();
   while(client.available()){
    String line = client.readStringUntil('/');

      String str = getValue(line,'[',1);
      if(eventsGet == 0){

         valActive1 = getValue(str, ',', 0);
         tempo1 = getValue(str, ',', 1);
         note1 = getValue(str, ',', 2);        
      } else if(eventsGet == 1){
         valActive2 = getValue(str, ',', 0);
         tempo2 = getValue(str, ',', 1);
         note2 = getValue(str, ',', 2);  
      }

      
      if(str != ""){
          Serial.print(str);
          Serial.print('\n'); 
        /*  Serial.print('\n');  
          Serial.print(valActive.toInt());
          Serial.print('\n');  
          Serial.print(tempo.toInt());
          Serial.print('\n');  
          Serial.print(note.toInt());
          Serial.print('\n');  */
    
          eventsGet++;

          if(eventsGet == 2){
            eventsGet = 0;
             Serial.flush();
             Serial.print(tempo1);
             Serial.print('\n'); 
            delay(tempo1.toInt());
            doPiano(valActive1.toInt(), note1.toInt());
            Serial.print(tempo2);
            Serial.print('\n'); 
            if(numMusic == "2"){
              delay(tempo2.toInt());
            }else {
              delay(tempo2.toInt()*2);  
            }
            
            doPiano(valActive2.toInt(), note2.toInt());     
          }
    
      }


  }
  
  Serial.println();
  Serial.println("closing connection");
}
