#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PIN  D3
#define SS_PIN   D4
#define BUZZER   D8
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin 3
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;  
MFRC522::StatusCode status;      

int blockNum = 2;  
int a = 0, b = 0, c = 0, d = 0, e = 0;
int count = a + b + c + d +e;
const String total = "/";

//Array for storing data read from the block

byte bufferLength = 18;
byte readBlockData[18];

String card_holder_name;
const String sheet_url = "google sheets URL + exec?name=";

const uint8_t fingerprint[20] = {google sheet page fingerprint};

#define WIFI_SSID "Wifi ID"
#define WIFI_PASSWORD "Wifi password"

//setup() function //
void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)

  // Initialize serial communications //
  Serial.begin(9600); 
  //WiFi Connectivity
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
   
  pinMode(BUZZER, OUTPUT);

  //Initialize SPI bus
  SPI.begin();
   
}

 //loop() function
 
 void loop()
{
  
  //Initialize MFRC522 Module 
  mfrc522.PCD_Init();
  // Look for new cards 
  // Reset the loop if no new card is present on RC522 Reader 
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}
  // Select one of the cards 
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}
  // Read data from the same block 
  
  Serial.println();
  Serial.println(F("Reading last data from RFID..."));
  ReadDataFromBlock(blockNum, readBlockData);
 
  // Print the data read from block 
  Serial.println();
  Serial.print(F("Last data in RFID:"));
  Serial.print(blockNum);
  Serial.print(F(" --> "));
  for (int j=0 ; j<16 ; j++)
  {
    Serial.write(readBlockData[j]);
  }
  Serial.println();
 
  String directionString;
  String blockdata = String((char*)readBlockData);
  
  if(blockdata == "User_1"){      
     
       switch(a){
          case 0:
            a++;
            count++;          
            directionString = "/Check_in";
          break;
          case 1:               
            a--;
            count--;           
            directionString = "/Check_out";
          break;       
  
       }
  }
   if(blockdata == "User_2"){      
      
       switch(b){
          case 0:
            b++;
            count++;
            directionString = "/Check_in";  
          break;
          case 1:               
            b--;
            count--;         
            directionString = "/Check_out";  
          break;       
  
       }
  }
   if(blockdata == "User_3"){      
      
       switch(c){
          case 0:
            c++;
            count++;
            directionString = "/Check_in";  
          break;
          case 1:               
            c--;
            count--;         
            directionString = "/Check_out";  
          break;       
  
       }
  }
   if(blockdata == "User_4"){      
      
       switch(d){
          case 0:
            d++;
            count++;
            directionString = "/Check_in";  
          break;
          case 1:               
            d--;
            count--;         
            directionString = "/Check_out";  
          break;       
  
       }
  }
  if(blockdata == "User_5"){      
      
       switch(e){
          case 0:
            e++;
            count++;
            directionString = "/Check_in";  
          break;
          case 1:               
            e--;
            count--;         
            directionString = "/Check_out";  
          break;       
  
       }
  }

 //Buzzer activated 2 times for 2 miliseconds
  String countString = String(count);
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  delay(200);
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);

 //Display visitor count at OLED display 
  display.clearDisplay();
  display.setTextColor(WHITE);
 
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.print("Visitor Count");
  display.setTextSize(2);
  display.setCursor(60, 22);
  display.print(count);

 //Display direction of the user for two and a half seconds when card read
  display.setTextSize(1);
  display.setCursor(35, 50);
  display.print(directionString);
    
  display.display();
  delay(2500);

  display.clearDisplay();
  display.setTextColor(WHITE);
 
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.print("Visitor Count");
  display.setTextSize(2);
  display.setCursor(60, 22);
  display.print(count);

  display.display();
  delay(500);
 
  //Sending data to cloud
  if (WiFi.status() == WL_CONNECTED) {
   
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
   
    client->setInsecure();
   
    card_holder_name = sheet_url + String((char*)readBlockData)+ directionString + total + countString;
    card_holder_name.trim();
    Serial.println(card_holder_name);
    
    HTTPClient https;
    Serial.print(F("[HTTPS] begin...\n"));
     
    if (https.begin(*client, (String)card_holder_name)){
     
      // HTTP
      Serial.print(F("[HTTPS] GET...\n"));
      // start connection and send HTTP header
      int httpCode = https.GET();
 
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
      }     
      else      
      {Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());}     
      https.end();
      delay(1000);
    }

    else {
      Serial.printf("[HTTPS} Unable to connect\n");
    } 
  }
}

//ReadDataFromBlock() function
 
void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{ 
  
  // Prepare the key for authentication 
  // All keys are set to FFFFFFFFFFFFh at chip delivery from the factory 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  // Authenticating the desired data block for Read access using Key A 
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  
  if (status != MFRC522::STATUS_OK){
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  
  else {
    Serial.println("Authentication success");
  }
  
  // Reading data from the block 
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  
  else {
    Serial.println("Block was read successfully");  
  }
  
}
