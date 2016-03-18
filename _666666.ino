#include <dht11.h>
#include <SPI.h>
#include <Ethernet.h>
 
 
char state = '0';
char c[10] = "";
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,1,177);
 
EthernetClient client;

int i ;
 
char server[] = "1.wechatfurnishing.applinzi.com";
 
unsigned long lastConnectionTime = 0;          
boolean lastConnected = false;                 
//const unsigned long postingInterval = 150L*100L;  
const unsigned long postingInterval = 1500;

unsigned long lastSensorTime = 0;
const unsigned long SensorInterval = 400;

dht11 DHT11;

#define DHT11PIN 2

String stringData="";
static char dtostrfbuffer[15];
static char buf[150];

float sensor_tem, sensor_hum;

void setup(){
  // 设置串口通信波特率
  Serial.begin(9600);
  delay(1000);
  Ethernet.begin(mac, ip);
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  pinMode(7, OUTPUT);  

}
  
void loop(void){ 

  if(lastSensorTime > millis())lastSensorTime = millis();
  if(millis() - lastSensorTime > SensorInterval) {

    int chk = DHT11.read(DHT11PIN);
  
    Serial.print("Read sensor: ");
    switch (chk)
    {
      case DHTLIB_OK: 
                  Serial.println("OK"); 
                  break;
      case DHTLIB_ERROR_CHECKSUM: 
                  Serial.println("Checksum error"); 
                  break;
      case DHTLIB_ERROR_TIMEOUT: 
                  Serial.println("Time out error"); 
                  break;
      default: 
                  Serial.println("Unknown error"); 
                  break;
    }
  
    Serial.print("Humidity : ");
    Serial.println((float)DHT11.humidity, 2);
  
    Serial.print("Temperature : ");
    Serial.println((float)DHT11.temperature, 2);
      
    lastSensorTime = millis();
    //delay(500);
  
    if(state == '0'){
      digitalWrite(7, LOW);      
    }else if(state == '1'){
      digitalWrite(7, HIGH);
    }
    
    while(client.available()) {
      for(i=0;i<10;i++) { 
        c[i] = client.read();
        if(strstr(c,"{1}") != NULL){
          state = '1';
          break;
        }else if(strstr(c,"{0}") != NULL){
          state = '0';
          break;
        }else{
          state = 'n';
        }
        
      }  
    }
  
    for(i=0;i<10;i++){
      c[i] = '\0';    
    }
    Serial.print("state=");
    Serial.println(state);
  
    sensor_tem = DHT11.temperature;   
    sensor_hum = DHT11.humidity; 
  
    stringData+="{\"temperature\":";
    dtostrf(sensor_tem, 4, 2, dtostrfbuffer);
    stringData+=dtostrfbuffer;
    stringData+=",\"humidity\":";
    dtostrf(sensor_hum, 4, 2, dtostrfbuffer);
    stringData+=dtostrfbuffer;
    stringData+="}";
      
  
    stringData.toCharArray(buf, stringData.length()+1);
    stringData="";
  }
 
  if (!client.connected() && lastConnected) {
    Serial.println("disconnecting.");
    client.stop();
  }
  //Serial.println("ss");
  Serial.println((millis()-lastConnectionTime));
  if(lastConnectionTime > millis())lastConnectionTime = millis();
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    if (client.connect(server, 80)) {

      // send the HTTP PUT request:
      //client.print("GET /downup.php?token=doubleq&data=");
      client.print("GET /downup.php?token=wechat&json=");
      client.print(buf);
      client.println(" HTTP/1.1");
      client.println("Host: 1.wechatfurnishing.applinzi.com");
      client.println("User-Agent: arduino-ethernet");
      client.println("Connection: close");
      client.println();
      //Serial.println('2');
      lastConnectionTime = millis();
    }else {
      Serial.println("connection failed");
      Serial.println("disconnecting.");
      client.stop();
    }
  }
  lastConnected = client.connected();
}

