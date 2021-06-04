#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>

#include <WiFiClientSecure.h>


/**********************/
#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();


/***************************************/
long durasi;
#define trig D3
#define echo D8

  float temp;
/**************/
#define SS_PIN D4
#define RST_PIN D0    // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint  = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6"; // for https

//***********Things to change*******************
const char* ssid = "Home";
const char* password = "home5678";
String GOOGLE_SCRIPT_ID = "AKfycbzbHiXTx-SUX3m8kBZ5U-QIOwH3sQGXWEb8FNRGcg"; // Replace by your GAS service id
//const String unitName = "headquarter"; // any name without spaces and special characters
//***********Things to change*******************


WiFiClientSecure client;


/**********************SETUP************/
void setup() {

  Serial.begin(9600); 

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

 // Serial.println("Started");
 // Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
 //   Serial.print(".");
  }

  // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details


   mlx.begin();  

 pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);

}


/******************************************************************/
void masker ()
{
  int mask =0;
  Serial.println("9");
  delay(500);
  
  for( uint64_t maskMillis = millis();mask<1000;){
   mask= analogRead(A0);
 

    if(millis()- maskMillis>15000){
          Serial.println("7");
          delay(1000);
          Serial.println("2");
          delay(500);
          break;
    }

      if (mask > 1000){
       Serial.println("10");
          delay(1000);
           Serial.println("2");
          delay(1000);
          break;
       }
       
     delay(1000);
  } 
  }

byte readCard[4];

void HandleDataFromGoogle(String data)
{
  int ind = data.indexOf(":");
  String access = data.substring(0, ind);
  int nextInd = data.indexOf(":", ind + 1);
  String name = data.substring(ind + 1, nextInd);
//  String text = data.substring(nextInd + 1, data.length());

  Serial.println(name);
  delay(500);
  Serial.println("1");
  delay(1000);
  if (access=="-1")
  {
    Serial.println("denied");
    delay(500);
    Serial.println("3"); 
    delay(500);
 //   Serial.println("Ready");
  }
  
  else if(access=="ok")
  {
    
    Serial.println("8");
    delay(1000);
    masker();
  }
}


    
/*************************************SLAVE****************************************************/




/*****************************************************************************************/

void loop() {

   delay(500);
Serial.println(F("4"));
  
  while(!mfrc522.PICC_IsNewCardPresent()) {
//    return;
delay(500);
  }

  // Select one of the cards
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
 while (!mfrc522.PICC_ReadCardSerial()) {
  //  return;
    delay(500);
  }

  
  String uid = "";
 
  
  for (uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
  //  Serial.print(readCard[i], HEX);
    uid += String(readCard[i],HEX);
  }
  
  Serial.println("5");
    delay(500);

float jarak=100;
for( uint64_t jarakMillis = millis();jarak > 13;){
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  
  durasi = pulseIn(echo,HIGH);
  jarak = durasi*0.017;
 // Serial.print("jarak : ");
 // Serial.println(jarak);
    delay(500);
     Serial.println("6 ");
    delay(1000);
    
  if(millis()-jarakMillis>10000){
  Serial.println("7");
    delay(1000);
   Serial.println("2");
    delay(500);
    return;
  }

  if(jarak<=13){
    temp = (mlx.readObjectTempC()*0.1379)+32.183;
    Serial.print("suhu: ");
    Serial.println(temp);
    delay(1000);
    Serial.println("1");
    delay(750);
    break;
  }
  delay (1000);
}

if (temp > 37.5){
  Serial.println("11");
  delay(5000);
  return;
}

//  Beep();
//  LcdClearAndPrint("Please wait...");
  String data = sendData("&uid=" + uid + "&temp=" + temp,NULL);
  HandleDataFromGoogle(data);


  


  mfrc522.PICC_HaltA();
}

/****************************************************************************************************************/
String sendData(String params, char* domain) {
  //google scripts requires two get requests 
    Serial.println("loading...");
  delay(500);
  
  bool needRedir = false;
  if (domain == NULL)
  {
    domain=(char*)host;
    needRedir = true;
    params = "/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + params;
  }
  
//  Serial.println(*domain);
  String result = "";
  client.setInsecure();
//  Serial.print("connecting to ");
//  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return "";
  }

  if (client.verify(fingerprint, domain)) {
  }

//  Serial.print("requesting URL: ");
//  Serial.println(params);

  client.print(String("GET ") + params + " HTTP/1.1\r\n" +
    "Host: " + domain + "\r\n" +
    "Connection: close\r\n\r\n");

 // Serial.println("request sent");
  while (client.connected()) {

    String line = client.readStringUntil('\n');
    //Serial.println(line);
    if (needRedir) {

    int ind = line.indexOf("/macros/echo?user");
    if (ind > 0)
    {
//      Serial.println(line);
      line = line.substring(ind);
      ind = line.lastIndexOf("\r");
      line = line.substring(0, ind);
 //     Serial.println(line);
      result = line;
    }
    }

    if (line == "\r") {
 //     Serial.println("headers received");
      break;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if(!needRedir)
    if (line.length() > 5)
      result = line;
    //Serial.println(line);
    
    }
  if (needRedir)
    return sendData(result, "script.googleusercontent.com");
  else return result;

}
