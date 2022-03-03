
#include "heltec.h"
#include "images.h"

#define pin_fs_elrs 36  //input pin to read PWM output from flight controler (to determine if rc link lost)
#define pin_fs_ghost 37  //input pin to read PWM output from flight controler (to determine if rc link lost)


int fs_elrs = 0;
int fs_ghost = 0;

void setup() {

  Serial.begin(115200);
  // PWM RC read pin init
  pinMode(pin_fs_elrs, INPUT);
  pinMode(pin_fs_ghost, INPUT);
  
}

void loop() {

  fs_elrs = digitalRead(pin_fs_elrs);
  fs_ghost = digitalRead(pin_fs_ghost);
  Serial.print("failsafe elrs/ghost: ");
  Serial.print(fs_elrs);
  Serial.print("\t");
  Serial.println(fs_ghost);

}
