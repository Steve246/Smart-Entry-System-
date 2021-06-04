#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);



const int BUZZ_PIN = 5 ; 
const int GATE_PIN = 3;
const int BUTTON_PIN = 12;
int tombol;


/***************************************/
long durasi;
long jarak;
const int trig = 9;

const int echo = 10;
const int sipeed = 11;
/***************************************/

uint64_t openGateMillis = 0;


void Clear()
{
    lcd.clear();
}
void LcdAtas(String text)
{
  lcd.setCursor(0, 0);
  lcd.print(text);
}

void LcdBawah(String text)
{
  lcd.setCursor(0, 1);
  lcd.print(text);
}

void Siren()
{
  for (int hz = 440; hz < 1000; hz++) {
    tone(BUZZ_PIN, hz, 50);
    delay(5);
  }
  
  for (int hz = 1000; hz > 440; hz--) {
    tone(BUZZ_PIN, hz, 50);
    delay(5);
  }
  analogWrite(BUZZ_PIN, 0);
}


void Beep()
{
  digitalWrite(BUZZ_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZ_PIN, LOW);
  delay(100);
}

void Beep2()
{
  digitalWrite(BUZZ_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZ_PIN, LOW);
  delay(100);
  digitalWrite(BUZZ_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZ_PIN, LOW);
}

void OpenGate()
{
 openGateMillis = millis()+5000;
 digitalWrite(GATE_PIN, LOW);
 delay(500);
}

void CloseGate()
{
  openGateMillis = 0;
  digitalWrite(GATE_PIN, HIGH);
  delay(500);
}

String data = "";

void setup() {
  // put your setup code here, to run once:
pinMode(GATE_PIN, OUTPUT);
pinMode(BUZZ_PIN, OUTPUT);
pinMode(BUTTON_PIN, INPUT);
 pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
    pinMode(sipeed,OUTPUT);
digitalWrite(GATE_PIN, HIGH);
digitalWrite(BUZZ_PIN, LOW);
digitalWrite(sipeed, HIGH);
 Serial.begin(9600); 

lcd.begin(); // Init with pin default ESP8266 or ARDUINO
// lcd.begin(0, 2); //ESP8266-01 I2C with pin 0-SDA 2-SCL
// Turn on the blacklight and print a message.
  lcd.backlight();
  LcdAtas("Loading");

//  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
 // data ="Ready";
}



void loop() {

if (openGateMillis > 0 && openGateMillis < millis())
  {
    CloseGate();
  }

tombol = digitalRead (BUTTON_PIN);

if (tombol == HIGH){
  OpenGate();
}
  
while (Serial.available()) {
data+= char(Serial.read());
}

data.trim();
Serial.println(data);


if(data=="1"){
  Beep();
}

else if (data == "2"){
  Beep2();
}

else if (data == "3"){
  Siren();
}

else if (data == "4"){
  Clear();
  LcdAtas("Scan Kartu");
}

else if (data == "5"){
  LcdBawah("OK");
}

else if (data == "6"){
  Clear();
  LcdAtas("Dekatkan wajah");
  LcdBawah("Anda");
}

else if (data == "7"){
  Clear();
  LcdAtas("Silahkan coba");
  LcdBawah("lagi");
}

else if (data == "8"){
  LcdBawah("Diterima");
}

else if (data == "9"){
  Clear();
  LcdAtas("Cek masker");
  Beep();

jarak=100;
for( uint64_t jarakMillis = millis();jarak > 50;){
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  
  durasi = pulseIn(echo,HIGH);
  jarak = durasi*0.017;

  if(jarak<=50){
    digitalWrite(sipeed,LOW);
    delay (1000);
    break;
  }
  if(millis()-jarakMillis>13000){
    return;
  }
}
}

else if (data == "10"){
  Clear();
  LcdAtas("Akses Diterima");
  LcdBawah("Silahkan masuk");
  OpenGate();
  Beep2();
  digitalWrite(sipeed, HIGH);
  
}

else if(data=="11"){
  Clear();
  LcdAtas("Suhu > 37.5°C !!!");
  Siren();
  LcdBawah("Akses ditolak");
}


else if (data == ""){
}

else {
  Clear();
  LcdAtas(data);
}

 data ="";
delay(500); 
}
