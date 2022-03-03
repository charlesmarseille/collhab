/*
  This is a simple example show the Heltec.LoRa recived data in OLED.

  The onboard OLED display is SSD1306 driver and I2C interface. In order to make the
  OLED correctly operation, you should output a high-low-high(1-0-1) signal by soft-
  ware to OLED's reset pin, the low-level signal at least 5ms.

  OLED pins to ESP32 GPIOs via this connecthin:
  OLED_SDA -- GPIO4
  OLED_SCL -- GPIO15
  OLED_RST -- GPIO16
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  This project is also available on GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series

 (CM) LoRa settings: https://github.com/HelTecAutomation/Heltec_ESP32/blob/master/src/lora/API.md
  
*/
#include "heltec.h" 
#include "images.h"

#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6
String rssi = "RSSI --";
String packSize = "--";
String packet ;

void logo(){
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void LoRaData(){
  int mess_len = 14;
  String mess_num = packet(0,6);
  int latl = 6;
  int nsatsl = 2;
  
  int lat = packet(0,latl);
  int lng = packet(latl,latl*2);
  int nsats = pcaket(latl*2,latl*2+nsatsl);
  
  String lat = packet(numshape,numshape+lat_shape);
  String lng = packet(numshape+lat_shape,numshape+lat_shape)
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet);
  Heltec.display->drawString(0 , 10, " bytes" + packSize);
  Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet);
  Heltec.display->drawString(0, 0, rssi);  
  Heltec.display->display();
}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  Serial.println(packet);
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  LoRaData();
}

void setup() { 
   //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
 
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();
  
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->drawString(0, 10, "Wait for incoming data...");
  Heltec.display->display();
  delay(1000);
  //LoRa.onReceive(cbk);

  //(CM) Set LoRa FREQUENCY, SF and BANDWIDTH
  LoRa.setFrequency(915E6);
  LoRa.setSpreadingFactor(12);                     //Default: 7, Values: Between 6 and 12
  //LoRa.setSignalBandwidth(62.5E3);       //Default: 125E3, Values: 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3
  //LoRa.setSyncWord(0x34);           // ranges 0-0xFF, default 0x34, line 172 of https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/libdeps/RadioLib/src/modules/SX127x/SX127x.h
  //LoRa.setPreambleLength(8);       //Default 8
  //LoRa.setCodingRate4(8);
  //LoRa.enableCrc();

  //
  LoRa.receive();
}


void loop() {
  int packetSize = LoRa.parsePacket();
  Serial.println(packetSize);
  if (packetSize) { cbk(packetSize);  }
  delay(10);
}
