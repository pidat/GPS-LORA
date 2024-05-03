#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>
#include <SoftwareSerial.h>
#include <FirebaseESP32.h>

#define PIN_LORA_CS     5
#define PIN_LORA_RST    14
#define PIN_LORA_DIO0   2

#define rxPin 16
#define txPin 17

#define BLYNK_TEMPLATE_ID "TMPL6qtxiQO_A"
#define BLYNK_DEVICE_NAME "GPS Device"
#define BLYNK_AUTH_TOKEN "pw1tr0sXNY2LzW1xXGBIp55G5cyNkKdg"
#define BLYNK_PRINT Serial

#define FIREBASE_HOST "https://doan2-1d56e-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "i0rR4OUaNmHYdAdKpqSbebWSAindp6aHaSXiyE6H"

#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_USER "PhatDat_09"
#define MQTT_PASSWORD "Datdeptrai1"
#define MQTT_TOPIC_PUB "IoT_P"
#define MQTT_TOPIC_SUB "IoT_S"
#define MQTT_CLIENTID "device1"

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "IoT Lab";
char pass[] = "IoT@123456";
const String PHONE = "+84984573598";

String call;
String device_id = "Iot";
String cap;
String lati;
String longi;
String textMessage;
String smsStatus,senderNumber,receivedDate,msg;

WiFiClient esp_client;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient mqtt_client(MQTT_SERVER, MQTT_PORT, callback, esp_client);

SoftwareSerial sim800(rxPin, txPin);
FirebaseData fb;

// Data Sending Time
unsigned long CurrentMillis, PreviousMillis, DataSendingTime = (unsigned long) 1000 * 10;


void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_connect() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    //    if (mqtt_client.connect(mqtt_clientId, mqtt_user, mqtt_password)) {
    if (mqtt_client.connect(MQTT_CLIENTID)) {
      Serial.println("MQTT Client Connected");
      mqtt_publish((char*)("Hi from " + device_id).c_str());
      // Subscribe
      mqtt_subscribe(MQTT_TOPIC_PUB);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqtt_publish(char * data) {
  mqtt_connect();
  if (mqtt_client.publish(MQTT_TOPIC_PUB, data))
    Serial.println("Publish \"" + String(data) + "\" ok");
  else
    Serial.println("Publish \"" + String(data) + "\" failed");
}
void mqtt_subscribe(const char * topic) {
  if (mqtt_client.subscribe(topic))
    Serial.println("Subscribe \"" + String(topic) + "\" ok");
  else
    Serial.println("Subscribe \"" + String(topic) + "\" failed");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String command;
  Serial.print("\n\nMessage arrived [");
  Serial.print(topic);
  Serial.println("] ");
  for (int i = 0; i < length; i++)
    command += (char)payload[i];

  if (command.length() > 0)
    Serial.println("Command receive is : " + command);

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, command);
  JsonObject obj = doc.as<JsonObject>();

//  String id = obj[String("device_id")];
//  String latitude = obj[String("lati")];
//  String longitude = obj[String("longi")];
//  Serial.println("\nCommand device_id is : " + id);
//  Serial.println("Command latitude is : " + latitude);
//  Serial.println("Command longitude is : " + longitude);

}
  
void setup() 
{
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";

  Serial.begin(115200);
  
  Serial.println("LoRa Receiver");
  LoRa.setPins (PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0);
  LoRa.setSPIFrequency (20000000);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  else {
    Serial.print("LoRa initialized with frequency ");
    Serial.println(433E6);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
//  Blynk.begin(auth, ssid, pass);
  sim800.begin(9600);
  setup_wifi();
  mqtt_connect();
    
} 

void loop() {
  // try to parse packetar
//  Blynk.run();

//    while(sim800.available()){
//    parseData(sim800.readString());
//  }
////*************************************************
   
  
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // received a packet
    Serial.print("Received packet:  ");
    
    // read packet
     while(LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);      
      
      int pos1 = LoRaData.indexOf("/");
      int pos2 = LoRaData.indexOf(",");
      int pos3 = LoRaData.indexOf("`");
      cap = LoRaData.substring(0,pos1);
      lati = LoRaData.substring(pos1 +2, pos2);
//      longi = LoRaData.substring(pos2 +1, LoRaData.length()+1);
      longi = LoRaData.substring(pos2 +1, pos3); 
//      call =  LoRaData.substring(pos3 +1);
      
//  float capFloat = atoi(cap);;
    Serial.println(cap);
    Serial.println(lati); 
    Serial.println(longi);
//    Serial.println(call);
    Firebase.setString(fb, "/cap/dis", cap);
    Firebase.setString(fb, "/cap/lati", lati);
    Firebase.setString(fb,"/cap/longi", longi);
}   
    String pkt = "{";
      pkt += "\"device_1\": " + String(cap) + "%";
      pkt += "\"latitude\": " +String(lati) + " ";
      pkt += "\"longitude\": " + String(longi) + " ";
      pkt += "}";
      mqtt_publish((char*) pkt.c_str());
    if (!mqtt_client.loop())
      mqtt_connect();
//    send_data();  
//    Blynk.virtualWrite(V0, cap);
//    Blynk.virtualWrite(V1, lati);
//    Blynk.virtualWrite(V2, longi);
    
}
//    if(call == "1"){
//    callWarning();
//}  
}
//void parseData(String buff){
//  Serial.println(buff);
//
//  unsigned int len, index;
//  //////////////////////////////////////////////////
//  //Remove sent "AT Command" from the response string.
//  index = buff.indexOf("\r");
//  buff.remove(0, index+2);
//  buff.trim();
//  //////////////////////////////////////////////////
//  
//  //////////////////////////////////////////////////
//  if(buff != "OK"){
//    index = buff.indexOf(":");
//    String cmd = buff.substring(0, index);
//    cmd.trim();
//    
//    buff.remove(0, index+2);
//    
//    if(cmd == "+CMTI"){
//      //get newly arrived memory location and store it in temp
//      index = buff.indexOf(",");
//      String temp = buff.substring(index+1, buff.length()); 
//      temp = "AT+CMGR=" + temp + "\r"; 
//      //get the message stored at memory location "temp"
//      sim800.println(temp); 
//    }
//    else if(cmd == "+CMGR"){
//      extractSms(buff);
//      
//      
//      if(senderNumber == PHONE){
//        callWarning();
//      }
//    }
//  //////////////////////////////////////////////////
//  }
//  else{
//  //The result of AT Command is "OK"
//  }
//}
//
//void extractSms(String buff){
//   unsigned int index;
//   Serial.println(buff);
//    index =buff.indexOf(",");
//    smsStatus = buff.substring(1, index-1); 
//    buff.remove(0, index+2);
//    
//    senderNumber = buff.substring(0, 13);
//    buff.remove(0,19);
//   
//    receivedDate = buff.substring(0, 20);
//    buff.remove(0,buff.indexOf("\r"));
//    buff.trim();
//    
//    index =buff.indexOf("\n\r");
//    buff = buff.substring(0, index);
//    buff.trim();
//    msg = buff;
//    buff = "";
//    msg.toLowerCase();
//}
//void sendLocation()
//{
//  if(msg == "get location")
//{
//    sim800.print("AT+CMGF=1\r");
//    delay(1000);
//    sim800.print("AT+CMGS=\""+PHONE+"\"\r");
//    delay(1000);
//    sim800.print("http://maps.google.com/maps?q=loc:");
//    sim800.print(lati);
//    sim800.print(",");
//    sim800.print(longi);
//    sim800.write(0x1A);
//}
//     smsStatus = "";
//     senderNumber="";
//     receivedDate="";
//     msg="";  
//}
