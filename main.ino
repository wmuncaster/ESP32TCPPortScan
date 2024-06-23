#include <WiFi.h>
#include <AsyncTCP.h>


//
//    munchovie
//
//    fast ip port scanner - this example finds all port 80 listeners on 192.168.0. LAN
//
//    using ESP32 wroom 
//    AsyncTCP by https://github.com/dvarrel/AsyncTCP
//

const char* ssid = "";
const char* password = "";
const uint16_t port = 80;
#define _MAXHosts 254
bool ready = false;

int hosts[_MAXHosts+1];  

void setup() {
    Serial.begin(115200);
    delay(20);

    Serial.printf("Connecting to %s ", ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(500);
    }
    Serial.println(" connected... wait for it!");

   
    for (int i = 1; i <= _MAXHosts; i++) {
        hosts[i] = i;
        AsyncClient* client = new AsyncClient();
        String host = "192.168.0." + String(i);
    
        client->onConnect([](void *arg, AsyncClient *asyncClient){
            AsyncClient *client = asyncClient;
            int hostIndex = *(int*)arg;
            hosts[hostIndex] |= 512;
            client->close();
            delete client;
        }, &hosts[i]);

        client->onError([](void *arg, AsyncClient *asyncClient, int8_t error){
            AsyncClient *client = asyncClient;
            int hostIndex = *(int*)arg;
            hosts[hostIndex] |= 1024; // Mark error
            delete client;
        }, &hosts[i]);            

        client->connect(host.c_str(), port);
   
    }

    ready = true;

}

void loop() { 

    if ( ready ){
      int connects = 0;
      int errors = 0;
      for ( int x=1; x<_MAXHosts+1; x++ ){
          if ( (hosts[x] & 512 ) == 512 ) {
              connects++;
          }
          if ( (hosts[x] & 1024 ) == 1024 ) {
              errors++;
          }           
      }

      Serial.print( "ips scanned: " );
      Serial.println( connects + errors );
      
      delay(5000);

      if ( connects + errors == _MAXHosts  ){
         Serial.println( "\n" );
         for ( int x=1; x<_MAXHosts+1; x++ ){
            if ( (hosts[x] & 512 ) == 512 ){
                Serial.print("192.168.0.");
                Serial.print(x);
                Serial.println(" <---- Port is open!");
            }
         }
         Serial.println("DONE!");
         while( true ){ delay(60000);}
      }
    }

}
