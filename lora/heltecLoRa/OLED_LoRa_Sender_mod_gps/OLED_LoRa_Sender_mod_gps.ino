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
#include <TinyGPSPlus.h>


//(CM) uart read taken from https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/38
//HardwareSerial Serial2(2);
#define RXD2 23 //16 is used for OLED_RST !
#define TXD2 17
#define BAND 915E6  //you can set band here directly,e.g. 868E6,915E6
#define pin_fs_elrs 36  //input pin to read PWM output from flight controler (to determine if rc link lost)
#define pin_fs_ghost 38  //input pin to read PWM output from flight controler (to determine if rc link lost)
#define Fbattery    4200  //The default battery is 3700mv when the battery is fully charged.


//LoRa vars
unsigned int counter = 0;
String rssi = "RSSI --";
String packSize = "--";
String packet;

//GPS vars
static const uint32_t GPSBaud = 9600;
float lat = 000000, lng = 000000;
float last_good_lat = 0, last_good_lng = 0;
int gps_age = 0;
int last_gps_age = 0;

//failsafe vars
int fs_elrs = 1;
int fs_ghost = 1;


//Vbat measurement vars
float XS = 0.0030;      //The returned reading is multiplied by this XS to get the battery voltage.
uint16_t MUL = 1000;
uint16_t MMUL = 100;
int vbat_delay = 60;

/*GPS infos: https://navspark.mybigcommerce.com/content/NMEA_Format_v0.1.pdf
  GN -> Both GPS and Beidou sats. 2 messages of GNGSA, one for each.
  GP -> Only GPS sats. Single GPGSA message.
  BD -> Only Beidou sats. Single BDGSA message.
  GL -> Only Glonass sats. Single GLGSA message.
*/
TinyGPSPlus gps;


void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void displayInfo() {
  Serial.print(F("Location: ")); 
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else Serial.print(F("INVALID"));
  
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else Serial.print(F("INVALID"));
  
  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else Serial.print(F("INVALID"));
  
  Serial.println();
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial2.available())
      gps.encode(Serial2.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}

void setup() {
  // GPS uart init
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // GPI pins for failsafe bits
  //gpio_reset_pin(37);
  pinMode(pin_fs_elrs, INPUT);
  pinMode(pin_fs_ghost, INPUT);


  //Vbat measurement
  analogSetCycles(8);                   // Set number of cycles per sample, default is 8 and provides an optimal result, range is 1 - 255
  analogSetSamples(1);                  // Set number of samples in the range, default is 1, it has an effect on sensitivity has been multiplied
  analogSetClockDiv(1);                 // Set the divider for the ADC clock, default is 1, range is 1 - 255
  analogSetAttenuation(ADC_11db);       // Sets the input attenuation for ALL ADC inputs, default is ADC_11db, range is ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
  analogSetPinAttenuation(36,ADC_11db); // Sets the input attenuation, default is ADC_11db, range is ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
  analogSetPinAttenuation(37,ADC_11db);
                                        // ADC_0db provides no attenuation so IN/OUT = 1 / 1 an input of 3 volts remains at 3 volts before ADC measurement
                                        // ADC_2_5db provides an attenuation so that IN/OUT = 1 / 1.34 an input of 3 volts is reduced to 2.238 volts before ADC measurement
                                        // ADC_6db provides an attenuation so that IN/OUT = 1 / 2 an input of 3 volts is reduced to 1.500 volts before ADC measurement
                                        // ADC_11db provides an attenuation so that IN/OUT = 1 / 3.6 an input of 3 volts is reduced to 0.833 volts before ADC measurement
  pinMode(21, OUTPUT);
  pinMode(37, INPUT);
  digitalWrite(21, LOW);
  adcAttachPin(37);

  
  //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

  // Set LoRa FREQUENCY, SF and BANDWIDTH
  LoRa.setFrequency(915E6);
  LoRa.setSpreadingFactor(12);                     //Default: 7, Values: Between 6 and 12
  //LoRa.setSignalBandwidth(62.5E3);       //Default: 125E3, Values: 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3
  //LoRa.setSyncWord(0x34);           // ranges 0-0xFF, default 0x34, line 172 of https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/libdeps/RadioLib/src/modules/SX127x/SX127x.h
  //LoRa.setPreambleLength(8);       //Default 8
  //LoRa.setCodingRate4(8);

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

void loop() {
  //Measure Vbat
  adcStart(37);
  while(adcBusy(37));
  Serial.printf("Battery power in GPIO 37: ");
  Serial.println(analogRead(37));
  uint16_t vbat  =  analogRead(37)*XS*MUL;
  adcEnd(37);

  //Check if failsafe on rx
  fs_elrs = digitalRead(pin_fs_elrs);
  fs_ghost = digitalRead(pin_fs_ghost);
  Serial.print("failsafe elrs/ghost: ");
  Serial.print(fs_elrs);
  Serial.print("\t");
  Serial.println(fs_ghost);

  //Print gps data to Serial
  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  Serial.println();
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
  Serial.println(F("No GPS data received: check wiring"));


  //Draw to OLED screen
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  if (!gps.location.isValid()){
    gps_age = millis()/1000 - last_gps_age;
    if (gps_age>59) {
      int gps_age_m = gps_age/60;
      int gps_age_s = gps_age%60;
      Heltec.display->drawString(60, 0, "age: " +String(gps_age_m)+"m"+String(gps_age_s));
    }
    else {
      Heltec.display->drawString(60, 0, "age: " +String(gps_age)+"s");
    }
  }
  else {
    last_gps_age = millis()/1000;
    gps_age = 0;
    last_good_lat = lat;
    last_good_lng = lng; 
  }
  Heltec.display->drawString(0, 0,  "Count: "+String(counter));
  Heltec.display->drawString(0, 10, "lat: "+String(last_good_lat));
  Heltec.display->drawString(0, 20, "lng: "+String(last_good_lng));
  Heltec.display->drawString(0, 30, "nsats: "+String(gps.satellites.value()));
  Heltec.display->drawString(0, 40, "hdop: "+String(gps.hdop.hdop()));
  Heltec.display->drawString(0, 50, "Failsafe elrs--ghost: "+String(fs_elrs)+"--"+String(fs_ghost));
  Heltec.display->drawString(70, 30, "VBat: "+String((vbat/100000)));
  Heltec.display->drawString(0, 60, "msg: "+String(packet));
  Heltec.display->display();


  // Start LoRa packet sending
  /*
   * LoRa.setTxPower(txPower,RFOUT_pin);
   * txPower -- 0 ~ 20
   * RFOUT_pin could be RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
   *   - RF_PACONFIG_PASELECT_PABOOST -- LoRa single output via PABOOST, maximum output 20dBm
   *   - RF_PACONFIG_PASELECT_RFO     -- LoRa single output via RFO_HF / RFO_LF, maximum output 14dBm
  */
  LoRa.enableCrc();
  LoRa.beginPacket();
  LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST);
  
  if (gps.location.isValid()){
    lat = gps.location.lat()*10000;
    lng = gps.location.lng()*10000;
  }
  String nsats = "00"; 
  if (gps.satellites.value()<10) nsats = "0"+String(gps.satellites.value());
  String gps_time = String(gps.time.hour())+String(gps.time.minute())+String(gps.time.second());
  //String packet = gps_time+String(lat)+String(lng)+nsats;
  String packet = String(lat)+String(lng)+String(fs_elrs)+String(fs_ghost)+String(vbat);
  LoRa.print(packet);
  LoRa.endPacket();
  Serial.println(packet);

  counter++;
  if (vbat<4){
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1);
    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
    delay(1);    
  }
  else {
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1);
    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
    delay(1);    
  }
}
