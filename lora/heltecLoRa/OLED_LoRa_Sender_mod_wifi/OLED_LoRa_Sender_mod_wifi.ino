/*
  This is a simple example show the Heltec.LoRa sended data in OLED.

  The onboard OLED display is SSD1306 driver and I2C interface. In order to make the
  OLED correctly operation, you should output a high-low-high(1-0-1) signal by soft-
  ware to OLED's reset pin, the low-level signal at least 5ms.

  OLED pins to ESP32 GPIOs via this connecthin:
  OLED_SDA -- GPIO4
  OLED_SCL -- GPIO15
  OLED_RST -- GPIO16
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  https://heltec.org
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/


#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

bool deviceConnected = false;
bool oldDeviceConnected = false;

const char* ssid = "BELL912";
const char* password = "jeanclaude2020";
String First_data = "ABCD";
unsigned int counter = 0;


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

//(CM)
#define WIFI_LED 100

WebServer server(80);
String htmlS ="<html>\
  <head>\
    <title>Wireless_Bridge</title>\
      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  </head>\
  <style type=\"text/css\">\
    .header{display: block;margin-top:10px;text-align:center;width:100%;font-size:10px}\
    .input{display: block;text-align:center;width:100%}\
    .input input{height: 20px;width: 300px;text-align:center;border-radius:15px;}\
    .input select{height: 26px;width: 305px;text-align:center;border-radius:15px;}\
    .btn,button{width: 305px;height: 40px;border-radius:20px; background-color: #000000; border:0px; color:#ffffff; margin-top:20px;}\
  </style>\
  <script type=\"text/javascript\">\
    function myrefresh()\
    {\
      window.location.reload();\
    }\
window.onload=function(){\
      setTimeout('myrefresh()',5000);\
      }   \
  </script>\
  <body>\
    <div style=\"width:100%;text-align:center;font-size:25px;font-weight:bold;margin-bottom:20px\">Wireless_Bridge</div>\
      <div style=\"width:100%;text-align:center;\">\
        <div class=\"header\"><span>(Note 1: The default refresh time of this page is 10s. If you need to modify the refresh time, you can modify it in the 'setTimeout' function.)</span></div>\
        <div class=\"header\"><span>(Note 2: The refresh time needs to be modified according to the data sending frequency.)</span></div>\
        <div class=\"header\"><span>Data: ";      
  String htmlF = "</span></div>\
      </form>\
    </div>\
  </body>\
</html>";

String htmlW = "<html>\
  <head>\
    <title>Wireless_Bridge</title>\
      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  </head>\
  <style type=\"text/css\">\
    .header{display: block;margin-top:10px;text-align:center;width:100%;font-size:10px}\
    .input{display: block;text-align:center;width:100%}\
    .input input{height: 20px;width: 300px;text-align:center;border-radius:15px;}\
    .input select{height: 26px;width: 305px;text-align:center;border-radius:15px;}\
    .btn,button{width: 305px;height: 40px;border-radius:20px; background-color: #000000; border:0px; color:#ffffff; margin-top:20px;}\
  </style>\
  <script type=\"text/javascript\">\
window.onload=function(){\
      document.getElementsByName(\"server\")[0].value = \"\";\
      }   \
  </script>\
  <body>\
    <div style=\"width:100%;text-align:center;font-size:25px;font-weight:bold;margin-bottom:20px\">Wireless_Bridge</div>\
      <div style=\"width:100%;text-align:center;\">\
        <div class=\"header\"><span>(Note : The data received by Bluetooth needs to be viewed with a Bluetooth debugging tool.)</span></div>\
        <form method=\"POST\" action=\"\" onsubmit=\"\">\
          <div class=\"header\"><span>DATA</span></div>\
          <div class=\"input\"><input type=\"text\"  name=\"server\" value=\"\"></div>\
        <div class=\"header\"><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submit\"></div>\
      </form>\
    </div>\
  </body>\
</html>";


String Page_data="";
String symbol=":";
void ROOT_HTML()
{
  
  Page_data =Page_data+ (String)counter + symbol +"<br>";
  String html = htmlS + Page_data + htmlF;
  server.send(200,"text/html",html);
}  

bool WiFiDownLink = false;
uint32_t WiFidonwlinkTime;
String Write_data;
void ROOT_HTMLW()
{
  if(server.hasArg("server"))
  {
    Serial.println(server.arg("server"));
    Write_data=server.arg("server");
    WiFiDownLink = true;
    WiFidonwlinkTime = millis();
  }
    server.send(200,"text/html",htmlW);
} 


#include "heltec.h"
#include "images.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6


String rssi = "RSSI --";
String packSize = "--";
String packet ;

void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void setup()
{
  Serial.begin(115200);
  pinMode(WIFI_LED, OUTPUT);
  digitalWrite(WIFI_LED, HIGH); 
  delay(1000); 
  digitalWrite(WIFI_LED, LOW);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    server.on("/", ROOT_HTML);
    server.on("/w", ROOT_HTMLW);
    server.begin();
    MDNS.addService("http", "tcp", 80);
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("View page IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Write page IP address: ");
    Serial.print(WiFi.localIP());
    Serial.println("/w");
  } else {
    Serial.println("WiFi Failed");
  }

   //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
 
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();
  
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->display();
  delay(1000);
}


bool State = false;
void loop()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  
  Heltec.display->drawString(0, 0, "Sending packet: ");
  Heltec.display->drawString(90, 0, String(counter));
  Heltec.display->display();

  //(CM) Set LoRa FREQUENCY, SF and BANDWIDTH
  LoRa.setFrequency(915E6);
  LoRa.setSpreadingFactor(7);                     //Default: 7, Values: Between 6 and 12
  LoRa.setSignalBandwidth(7.8E3);       //Default: 125E3, Values: 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3

  //

  // send packet
  LoRa.beginPacket();
  
/*
 * LoRa.setTxPower(txPower,RFOUT_pin);
 * txPower -- 0 ~ 20
 * RFOUT_pin could be RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
 *   - RF_PACONFIG_PASELECT_PABOOST -- LoRa single output via PABOOST, maximum output 20dBm
 *   - RF_PACONFIG_PASELECT_RFO     -- LoRa single output via RFO_HF / RFO_LF, maximum output 14dBm
*/
  LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

  server.handleClient();
  if(WiFiDownLink)
  {
    WiFiDownLink = false;
    digitalWrite(WIFI_LED,HIGH);
  }
  else if(digitalRead(WIFI_LED) && (millis()-WiFidonwlinkTime)> 1000)
  {
    digitalWrite(WIFI_LED,LOW);
  }
}
