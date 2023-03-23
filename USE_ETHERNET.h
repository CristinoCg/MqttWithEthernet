#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;
EthernetServer server(80);
//IPAddress ip(172, 16, 30, 253);

/*void configureNetwork(){
	Ethernet.begin(mac, ip);
}*/


bool configureNetwork(){
  if(Ethernet.begin(mac))
 {
    delay(10);
    return true;
    }
   else{
    return false;
    }
  }


IPAddress getIp(){

  return Ethernet.localIP();
}

#define MAINTAIN_DELAY 750
void maintain(){

  static unsigned long nextMaintain = millis() + MAINTAIN_DELAY;
  if( millis() > nextMaintain){
    nextMaintain = millis() + MAINTAIN_DELAY;
    int ret = Ethernet.maintain();
    if( ret == 1 || ret== 3){
      Serial.print("Falha ao manter o DHCP activo. Erro: ");
      Serial.println(ret);
    }
  }
  
}
