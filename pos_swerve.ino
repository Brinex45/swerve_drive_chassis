#include <ODriveUART.h>
int a = 0;
ODriveUART odrive1(Serial1);
ODriveUART odrive2(Serial5);
ODriveUART odrive3(Serial3);
ODriveUART odrive4(Serial8);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial5.begin(115200);
  Serial3.begin(115200);
  Serial8.begin(115200);

  odrive3.closedLoopState();
  odrive4.closedLoopState();

  millis_delay(2000);

  position_to_all(10);

}

// 7 
// 90
// 40
// -90
// 40
// -90
// 40
// -90
// 40


void loop() {
  // rotate(90);
  position_to_all(10);
  millis_delay(3000);

  rotate(90);
  millis_delay(5000);

  position_to_all(20);
  millis_delay(5000);

  rotate(90);
  millis_delay(5000);

  position_to_all(30);
  millis_delay(5000);

  rotate(90);
  millis_delay(5000);

  position_to_all(10);
  millis_delay(5000);

  rotate(90);
  millis_delay(5000);

  // position_to_all(40);
  // millis_delay(3000);

  // rotate(-90);
  // millis_delay(3000);

  // position_to_all(40);
  // millis_delay(3000);

  // rotate(-90);
  // millis_delay(3000);

  // position_to_all(40);
  // millis_delay(3000);

  // rotate(-90);
  // millis_delay(3000);

  // position_to_all(40);
  // millis_delay(3000);

}

void position_to_all(int a){

  odrive3.setPosition(a);
  odrive4.setPosition(a);
  odrive1.setPosition(a);
  odrive2.setPosition(a);
}

void rotate(int deg){
  int dir = deg >= 0 ? 1 : -1;
  int z = 73;
  int deg_rot = map(deg, 0, 360, 0, z);

  odrive3.setPosition(dir * -deg_rot);
  odrive4.setPosition(dir * -deg_rot);
  odrive1.setPosition(dir * deg_rot);
  odrive2.setPosition(dir * deg_rot);

}

void millis_delay(int micro){
  int ct;
  int pt = ct =  millis();
  while(ct-pt <= micro){
    ct = millis();
  }
}
  // odrive1.setPosition(1);
  // odrive2.setPosition(1);
  // odrive3.setPosition(1);
  // odrive4.setPosition(1);