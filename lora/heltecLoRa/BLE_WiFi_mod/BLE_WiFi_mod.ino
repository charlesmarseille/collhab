/*
 * HelTec Automation(TM) Wireless Bridge dedicated test code, witch includ
 * follow functions:
 *
 * - Transfer WiFi and BLE message to each other;
 * 
 * Because WiFi and BLE need working at the same time, this example can only
 * run with Heltec Wireless Bridge:
 * https://heltec.org/project/wireless-bridge/
 * 
 * Detail description about this demo code, please refer to this document:
 * 
 *
 * by Aaron.Lee from HelTec AutoMation, ChengDu, China
 * 成都惠利特自动化科技有限公司
 * www.heltec.cn
 *
 * this project also realess in GitHub:
 * https://github.com/HelTecAutomation/Heltec_ESP32
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
      setTimeout('myrefresh()',10000);\
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
  
  Page_data =Page_data+ symbol +"<br>";
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


void setup() {
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
}

bool State = false;
void loop() {
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
