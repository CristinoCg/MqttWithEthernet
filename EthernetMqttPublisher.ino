#include "USE_ETHERNET.h"
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <TinyGPS.h>


//Sensor de temperatura
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

//GPS
TinyGPS gps;
SoftwareSerial NeoGpsSerial(3,4);


//MQTT CONFIGS
const char* broker = "broker.hivemq.com";
const int intervalo =200;
unsigned int timePublished;
char topic[3][20] = {"/batimento","/localizacao","/temperatura"};  
PubSubClient psclient(client);

//Sensor de batimento
int batimento = 0;

void setup() {
  
  Serial.begin(9600);
  NeoGpsSerial.begin(9600);
  mlx.begin();

  Serial.print("Teste da Tiny GPS library v:");Serial.print(TinyGPS::library_version());
  Serial.println();

  
  if(!configureNetwork()){
    Serial.println("Falha ao configurar a rede!");
    while(1)
      delay(0);
   }
  Serial.print("Endereço local:");
  Serial.println(Ethernet.localIP());
  Serial.print("Ip remoto:");
  Serial.println(client.remoteIP());
  
  
  Serial.println("Sucesso!");
  psclient.setServer(broker, 1883);
    
}

void loop() {
  
  
  bool newData = false;
  float flat, flon;


  
  for(unsigned long start = millis(); millis()-start < 1000;)
    {
      
      while(NeoGpsSerial.available()){
        int c = NeoGpsSerial.read();
        Serial.write(c);
        
        if(gps.encode(c)){
          
          newData = true;
          
          }
        }
    }
  if(newData){
    
    gps.f_get_position(&flat, &flon);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    
  }
  
  
  if(millis() - timePublished > intervalo){
    
    if(psclient.connected() && newData){
      
      flat = flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat;
      flon = flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon;
      String localizacao = String(("LAT:%2f",flat)) + String(("LON:2%f",flon));
      psclient.publish(topic[1],localizacao.c_str());
      }
    if(!psclient.connected()){
        Serial.println("\nA reconectar!");
        psclient.connect("subscriberLocal");
        
    }
        
        
    if(psclient.connected()){
      
      int val = analogRead(0);
      Serial.print("Valor da temperatura:");Serial.println(mlx.readObjectTempC());
      Serial.print("Valor da batimento:");Serial.println(val/100);
      psclient.publish(topic[2], String(mlx.readObjectTempC()).c_str());
      psclient.publish(topic[0], String(val/10).c_str());
      Serial.println("Chegou no envio do MQTT");
      timePublished = millis();
      
    }
   
    
  }

  if(psclient.connected())
      psclient.loop();

}
