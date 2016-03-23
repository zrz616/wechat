#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "MERCURY_211";
const char* password = "993993993z";

const char* host = "1.wechatfurnishing.applinzi.com";
const int httpPort = 80;

String line;
char c[10];
const char * p_s = line.c_str();
char state = '0';
int i;

unsigned long lastSensorTime = 0;
const unsigned long SensorInterval = 400;

dht11 DHT11;
#define LED_OUT 13
#define DHT11PIN 16

String stringData="";
static char dtostrfbuffer[15];
static char buf[150];

float sensor_tem, sensor_hum;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(LED_OUT, OUTPUT);
  
  /*String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
*/
}

void loop() {
  WiFiClient client;
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
  
    if(state == '0'){
      digitalWrite(LED_OUT, LOW);      
    }else if(state == '1'){
      digitalWrite(LED_OUT, HIGH);
    }

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

    Serial.print("connecting to ");
    Serial.println(host);
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }
    String url = "/downup.php?token=wechat&json=";
    Serial.print("requesting URL: ");
    Serial.println(url);
  
    client.print(String("GET ") + url + buf + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
                 "Connection: close\r\n\r\n");
  
    Serial.println("request sent");
  
    while (client.connected()) {
      line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    line = client.readString();
    for(i=0;i<10;i++,p_s++){
      c[i] = *p_s;  
    }
    p_s = line.c_str();
    
  
    if(strstr(c,"{1}") != NULL){
      state = '1';   
    }else if(strstr(c,"{0}") != NULL){
      state = '0';
    }else{
      state = 'n';
    }
    
    Serial.print("state=");
    Serial.println(state);
  }

}