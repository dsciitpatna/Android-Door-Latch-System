//#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
//#include <FirebaseArduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
//#define FIREBASE_HOST "switching-theory-project.firebaseio.com"
//#define FIREBASE_AUTH "9aapt2mD4TQyRKRfPkT7ZmljeOBezh1JJ14npu6f"
#define WIFI_SSID "Redmi"  
#define WIFI_PASSWORD "abcdefgh"
#define LED 16 //new add
#define SERVER_KEY "AAAALoLXjtk:APA91bEZ9CjsAWXSViJqVTcY_mhFOwrCqAj7ZPzpsMJCvd3ZTaGK2519mUP7XUEQp8sW7x_KYtdZEz6auZWoSheMYsoVFwps-mixu8q1X2gEQcxanWYrLQsdDd_JJVrCyfZk-WbptERc"


String host = "switching-theory-project.firebaseio.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "B6 F5 80 C8 B1 DA 61 C1 07 9D 80 42 D8 A9 1F AF 9F C8 96 7D";

// defines pins numbers
const int reedSwitchPin = 0;  //D3
const int solenoidPin = 5; //D1

//defines door status 
bool  doorStatus=1;     //0 means door is closed

// defines variables
long duration;
int distance;
bool sensor_status;//new add
bool solenoid_status;
bool reedSwitchStatus;

void setup() {
Serial.begin(9600);
// connect to wifi.
WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
Serial.print("connecting");
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(500);
}
Serial.println();
Serial.print("connected: ");
Serial.println(WiFi.localIP());
//Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

pinMode(solenoidPin, OUTPUT); // Sets the solenoidPin as an Output
pinMode(reedSwitchPin, INPUT); // Sets the echoPin as an Input
pinMode(LED,OUTPUT);     //new add
}



void loop() {
  sensor_status = getSensorStatus();
  solenoid_status = getReadSwitchStatus();
  Serial.println(solenoid_status);
  Serial.println(solenoid_status);
  //Serial.print("Sensor status");
  //Serial.println(sensor_status);
  if(sensor_status==1)
     digitalWrite(LED,LOW);
  else digitalWrite(LED,HIGH);   
if(solenoid_status == 0){
   Serial.println("working");
  digitalWrite(solenoidPin, HIGH);
  delay(20);
}
else{
  digitalWrite(solenoidPin, LOW);
  delay(20);
}
if (sensor_status == 1) {
// Clears the trigPin
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
delayMicroseconds(10);

// Reads the reed switch status
reedSwitchStatus = digitalRead(reedSwitchPin);

delay(20);
if(reedSwitchStatus == LOW){
        //Serial.println(distance);
      delay(20);  
      Serial.println("Door closed");
      setDoorStatus(false);
      doorStatus=1;
    }
    else
    {
      
        //Serial.println(distance);
      delay(20);  
      Serial.println("Door open");
      setDoorStatus(true);
      if (doorStatus==1) {
        //Serial.println("Notification");
        doorStatus=0;
        sendNotification();
      }
    }
}
  delay(200);
}

void sendNotification() {
     HTTPClient http;    //Declare object of class HTTPClient
  
     http.begin("http://fcm.googleapis.com/fcm/send");      //Specify request destination
     http.addHeader("Content-Type","application/json");  //Specify content-type header
     http.addHeader("Authorization","key=AAAALoLXjtk:APA91bEZ9CjsAWXSViJqVTcY_mhFOwrCqAj7ZPzpsMJCvd3ZTaGK2519mUP7XUEQp8sW7x_KYtdZEz6auZWoSheMYsoVFwps-mixu8q1X2gEQcxanWYrLQsdDd_JJVrCyfZk-WbptERc");  //Specify content-type header
  
     String data = "{\n";
     data += "\"notification\": {\n";
     data += "\"title\" : \"DOOR ALERT!\",\n";
     data += "\"body\": \"WARNING: Your door is open.\",\n";
     data += "\"sound\" : \"notification.mp3\"\n";
     data += "}\n";
     data += ",\n";
     data += "\"to\" : \"/topics/door_status\"\n";
     data += "}\"\n";
  
     //Serial.println(data);
     
     int httpCode = http.POST(data);   //Send the request
     String payload = http.getString();                  //Get the response payload
   
     Serial.println(httpCode);   //Print HTTP return code
     Serial.println(payload);    //Print request response payload
     http.end();
}

bool getSensorStatus() {
     HTTPClient http;    //Declare object of class HTTPClient
  
     http.begin("https://switching-theory-project.firebaseio.com/sensor_status.json",fingerprint);      //Specify request destination
     
     int httpCode = http.GET();   //Send the request
     String payload = http.getString();                  //Get the response payload
   
     Serial.println(httpCode);   //Print HTTP return code
     Serial.println(payload);    //Print request response payload
     http.end();
   
   return payload.equals("true");
   
}

bool getReadSwitchStatus() {
     HTTPClient http;    //Declare object of class HTTPClient
  
     http.begin("https://switching-theory-project.firebaseio.com/read_switch_status.json",fingerprint);      //Specify request destination
     
     int httpCode = http.GET();   //Send the request
     String payload = http.getString();                  //Get the response payload
   
     Serial.println(httpCode);   //Print HTTP return code
     Serial.println(payload);    //Print request response payload
     http.end();
   
   return payload.equals("true");
   
}

void setDoorStatus(bool status) {
     HTTPClient http;    //Declare object of class HTTPClient
  
     http.begin("https://switching-theory-project.firebaseio.com/.json",fingerprint);      //Specify request destination
     http.addHeader("Content-Type","application/json");  //Specify content-type header
     http.addHeader("X-HTTP-Method-Override","PATCH");     

     String data = status ? "{\"door_open\":true}" : "{\"door_open\":false}";

     http.addHeader("Content-Length",String(data.length()));

     Serial.println(data);
     int httpCode = http.POST(data);   //Send the request
     String payload = http.getString();                  //Get the response payload
   
     Serial.println(httpCode);   //Print HTTP return code
     Serial.println(payload);    //Print request response payload
     http.end();
}
