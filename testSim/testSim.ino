#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial sim800(A2,A4); //SIM800L Tx & Rx is connected to Arduino #3 & #2
const String PHONE = "+84984573598";
String smsStatus,senderNumber,receivedDate,msg;

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200);

  //Begin serial communication with Arduino and SIM800L
  sim800.begin(9600);

  sim800.println("AT"); //Once the handshake test is successful, i t will back to OK
//  updateSerial();

  sim800.println("ATD+ +84984573598;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
  updateSerial();
  delay(20000); // wait for 20 seconds...
  sim800.println("ATH"); //hang up
//  updateSerial();
}

void loop()
{
  while(sim800.available())
  {
  parseData(sim800.readString());
  }
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    sim800.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (sim800.available())
  {
    Serial.write(sim800.read());//Forward what Software Serial received to Serial Port
  }
}

void parseData(String buff){
  Serial.println(buff);

  unsigned int len, index;
  //////////////////////////////////////////////////
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //////////////////////////////////////////////////
  
  //////////////////////////////////////////////////
  if(buff != "OK"){
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
    
    buff.remove(0, index+2);
    
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length()); 
      temp = "AT+CMGR=" + temp + "\r"; 
      //get the message stored at memory location "temp"
      sim800.println(temp); 
    }
    else if(cmd == "+CMGR"){
      extractSms(buff);
      
      if(senderNumber == PHONE){
        doAction();
        //delete all sms
        sim800.println("AT+CMGD=1,4");
        delay(1000);
        sim800.println("AT+CMGDA= \"DEL ALL\"");
        delay(1000);
      }
    }
  //////////////////////////////////////////////////
  }
  else{
  //The result of AT Command is "OK"
  }
}

//************************************************************
void extractSms(String buff){
   unsigned int index;
   
    index = buff.indexOf(",");
    smsStatus = buff.substring(1, index-1); 
    buff.remove(0, index+2);
    
    senderNumber = buff.substring(0, 13);
    buff.remove(0,19);
   
    receivedDate = buff.substring(0, 20);
    buff.remove(0,buff.indexOf("\r"));
    buff.trim();
    
    index =buff.indexOf("\n\r");
    buff = buff.substring(0, index);
    buff.trim();
    msg = buff;
    buff = "";
    msg.toLowerCase();
}

void doAction(){
  if(msg == "Hi"){  
    sim800.print("AT+CMGF=1\r");
    delay(1000);
    sim800.print("AT+CMGS=\""+PHONE+"\"\r");
    delay(1000);
    sim800.print("Hello");
    delay(100);
    sim800.write(26); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");
  }
  
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";  
}

//void Reply(String text)
//{
//    sim800.print("AT+CMGF=1\r");
//    delay(1000);
//    sim800.print("AT+CMGS=\""+PHONE+"\"\r");
//    delay(1000);
//    sim800.print(text);
//    delay(100);
//    sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
//    delay(1000);
//    Serial.println("SMS Sent Successfully.");
//}
