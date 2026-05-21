#include <ODriveUART.h>
// #include <SoftwareSerial.h>

// SoftwareSerial odrive_serial(8, 9); // RX (ODrive TX), TX (ODrive RX)

ODriveUART odrive1(Serial1);
ODriveUART odrive2(Serial5);
ODriveUART odrive3(Serial3);
ODriveUART odrive4(Serial8);

// int vel , axis = 0;
// int pos;
  int vel1,vel2,vel3,vel4;
  int a1 = 45, a2 = 45, a3 = 45, a4 = 45;

void setup() {
  Serial8.begin(115200);
  Serial5.begin(115200);
  Serial3.begin(115200);
  Serial1.begin(115200);
  Serial.begin(115200);
  
  // odrive1.closedLoopState(); // Accessing Closed Loop
  // odrive2.closedLoopState(); // Accessing Closed Loop
  odrive3.closedLoopState(); // Accessing Closed Loop
  odrive4.closedLoopState(); // Accessing Closed Loop
  delay(2000);
}

void loop() {

  vel1 = 5*cos(a1);
  vel2 = -5*cos(a2);
  vel3 = -5*cos(a3);
  vel4 = 5*cos(a4);

  odrive1.SetVelocity(0,vel1);
  odrive2.SetVelocity(0,vel2);
  odrive3.SetVelocity(0,vel3);
  odrive4.SetVelocity(0,vel4);

  // Serial.print("Enter the Speed : ");
  // while(Serial.available()==0);
  // vel = Serial.parseInt();
  // odrive.SetVelocity(0,vel);
  // Serial.println(vel);

  
  // Serial.print("Enter the Speed : ");
  // while(Serial.available()==0);
  // vel = Serial.parseInt();
  // Serial.println(vel);
  // Serial.print("Enter the Position : ");
  // while(Serial.available()==0);
  // pos = Serial.parseInt();
  // Serial.println(pos);
  // odrive.setPosition(pos,vel);

}