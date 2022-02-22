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



#include "heltec.h"
#include "images.h"

using namespace std;


//(CM) uart read taken from https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/38
//HardwareSerial Serial2(2);
#define RXD2 23 //16 is used for OLED_RST !
#define TXD2 17

#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6

unsigned int counter = 0;
String rssi = "RSSI --";
String packSize = "--";
String packet;

void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

vector<String> read_gps()
{
  Serial.println("start gps data reading");
  vector<String> gps_lines;
  vector<String> gps_lines_mod;
  while (Serial2.available()) {
    String gps_stream  = Serial2.readStringUntil('\n');
    //Serial.println(gps_stream);
    gps_lines.push_back(gps_stream);
  }

  /*GPS infos: https://navspark.mybigcommerce.com/content/NMEA_Format_v0.1.pdf
  GN -> Both GPS and Beidou sats. 2 messages of GNGSA, one for each.
  GP -> Only GPS sats. Single GPGSA message.
  BD -> Only Beidou sats. Single BDGSA message.
  GL -> Only Glonass sats. Single GLGSA message.
  */
  for (int i = 0; i< sizeof(gps_lines); i++){
    String gps_prefix = gps_lines[i].substring(1,6);
    if (gps_prefix == "GNRMC"){                                 //Time, date, position, course and speed data provided by a GNSS navigation receiver. Format:$--RMC,hhmmss.sss,x,llll.lll,a,yyyyy.yyy,a,x.x,u.u,xxxxxx,,,v*hh<CR><LF>
      gps_lines_mod[i] = gps_lines[i].substring(6);
    }
    else if (gps_prefix == "GNVTG"){                            //The actual course and speed relative to the ground. Format:$--VTG,x.x,T,y.y,M,u.u,N,v.v,K,m*hh<CR><LF>
      gps_lines_mod[i] = gps_lines[i].substring(6);
    }
    else if (gps_prefix == "GNGGA"){                            //Time, position and fix related data for a GPS receiver. Format:$--GGA,hhmmss.ss,llll.lll,a,yyyyy.yyy,a,x,uu,v.v,w.w,M,x.x,M,,zzzz*hh<CR><LF>
      gps_lines_mod[i] = gps_lines[i].substring(6);
    }
    else if (gps_prefix == "GNGSA"){                             //GPS receiver operating mode, satellites used in the navigation solution reported by the GGA or GNS sentence and DOP values. Format:$--GSA,a,x,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,u.u,v.v,z.z*hh<CR><LF>
      gps_lines_mod[i] = gps_lines[i].substring(6);
    }
    else if (gps_prefix == "GPGSV"){                             //Number of satellites (SV) in view, satellite ID numbers, elevation, azimuth, and SNR value. Four satellites maximum per transmission. Format:$--GSV,x,u,xx,uu,vv,zzz,ss,uu,vv,zzz,ss,…,uu,vv,zzz,ss*hh<CR><LF>
      gps_lines_mod[i] = gps_lines[i].substring(6);
    }
    else if (gps_prefix == "GLGSV"){
      gps_lines_mod[i] = gps_lines[i].substring(6);
    }
    else if (gps_prefix == "GLGLL"){                             //Latitude and longitude of vessel position, time of position fix and status. Format:$--GLL,llll.lll,a,yyyyy.yyy,b,hhmmss.sss,A,a*hh<CR><LF>
      gps_lines_mod[i] = gps_lines[i].substring(6);
    }
    else{
      Serial.println("No GPS prefix match");
    }
    for (int i=0; i<=sizeof(gps_lines_mod); i++){
      Serial.println(gps_lines_mod[i]);
    }
    return gps_lines_mod;
    
  }
}  

void setup()
{
  //(CM) GPS uart init
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
   //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

   //(CM) Set LoRa FREQUENCY, SF and BANDWIDTH
  LoRa.setFrequency(915E6);
  LoRa.setSpreadingFactor(12);                     //Default: 7, Values: Between 6 and 12
  //LoRa.setSignalBandwidth(62.5E3);       //Default: 125E3, Values: 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3
  //LoRa.setSyncWord(0x34);           // ranges 0-0xFF, default 0x34, line 172 of https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/libdeps/RadioLib/src/modules/SX127x/SX127x.h
  //LoRa.setPreambleLength(8);       //Default 8
  //LoRa.setCodingRate4(8);
  //LoRa.enableCrc();
 
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

void loop()
{
  vector<String> gps_data;
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  
  Heltec.display->drawString(0, 0, "Sending packet: ");
  Heltec.display->drawString(90, 0, String(counter));
  Heltec.display->display();


  //(CM) GPS read from stream
  gps_data = read_gps();

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
  LoRa.print("magog ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1);                       // wait for a second
}
