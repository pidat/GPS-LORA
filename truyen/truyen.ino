#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
//#include <NeoSWSerial.h>
#include <LoRa.h>
#include <SPI.h>

#define echopin 6 // echo pin
#define trigpin 7 // Trigger pin
#define led A5

int maximumRange = 150; // maximum height of tank in cm
long duration;
float distance; // height of water level in cm
float actual_height; // actual height in cm
float cap;
int count = 0;

const String PHONE = "+84984573598";
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
String out;
String LoRaMessage = "";

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial sim800(A2, A4);
void setup()
{
  Serial.begin(115200);
  sim800.begin(9600);
  Serial.println("SIM800L serial initialize");
  sim800.listen();
  sim800.println("AT");
  updateSerial();
//  sim800.println("ATD+ +84984573598;"); 
//  delay(20000); // wait for 20 seconds...
//  sim800.println("ATH"); //hang up
//  sim800.write(26);

  pinMode (led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.println("Lora Sender");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  ss.begin(GPSBaud);
//  ss.listen();
}

void loop()
{

  ss.listen();
  updateSerial();
//   Free of rubbish
    pinMode (trigpin, OUTPUT);
    digitalWrite(trigpin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigpin,HIGH);
    delayMicroseconds(5);
//    digitalWrite(trigpin,LOW);

    pinMode (echopin, INPUT );
    duration =  pulseIn (echopin,HIGH);
    distance = (duration/2)/29.4 ;
//    distance= duration* 0.034/2;
    delay(200);
    
    Serial.print("dis: ");
    Serial.println(distance);
    actual_height = maximumRange - distance;
    cap = ((actual_height/maximumRange)*100);
    Serial.println(cap);
   
   if (cap >= 90.0)
    {
    Serial.println("Trash bin is full");
    digitalWrite(led, HIGH);
    sim800.listen();
    sim800.println("AT");
    updateSerial();
    callWarning();
    }
   else
    {
    sim800.listen();
    sim800.println("AT");
    updateSerial();
    Serial.println("Still empty");
    digitalWrite(led, LOW);
    count = 0;
    }

    Serial.println(count);
    LoRaMessage = String(cap)+"/"+String(out)+"`"+int(count);
// Set up LoRa

  LoRa.beginPacket();
  LoRa.println(LoRaMessage);
  LoRa.endPacket();
  
// Set up GPS

  smartDelay(1000);
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
//    ss.listen();
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
//    ss.listen();
    while (ss.available())
      gps.encode(ss.read());
//__________________________________________________________________________________
String str = "";
char buff[12];
//      Serial.print(gps.time.hour());
//      str = str + gps.time.hour();
//      str = str + ":";
//      Serial.print(gps.time.minute());
//      str = str + gps.time.minute();
//      str = str + ":";      
//      Serial.print(gps.time.second());
//      str = str + gps.time.second();
//      str = str + ",";      
//      Serial.print(gps.satellites.value());
//      str = str + gps.satellites.value();
//      str = str + " " + ",";       
      Serial.print(gps.location.lat(), 6);
      float lat = gps.location.lat();
      dtostrf(lat, 5, 6, buff);
      str = str + buff;
      str = str + ",";            
      Serial.print(-gps.location.lng(), 6);
      float lng = gps.location.lng();
      dtostrf(lng, 5, 6, buff);               
      str = str + buff;
      str = " " + str;       
//      Serial.print(gps.altitude.meters());
//      str = str + gps.altitude.meters();      
      out = str;
      Serial.println("");
  } while (millis() - start < ms);

    delay(1000);
}

void callWarning()
{
    sim800.println("AT");
    sim800.println("ATD+ +84984573598;"); 
    delay(20000); // wait for 20 seconds...
    sim800.println("ATH"); //hang up
    sim800.write(26);
}

void updateSerial()
{
  sim800.listen();
  while (Serial.available())
  {
    sim800.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (sim800.available())
  {
    Serial.write(sim800.read());//Forward what Software Serial received to Serial Port
  }
  ss.listen();
  while (ss.available())
  gps.encode(ss.read());
}

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
//
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
