#include<Encoder.h>

#define A 32
#define B 33
#define Rpwm 6
#define Lpwm 7
#define pwm 70
#define tick 464928

// bts1 r-9 l-8
// bts2 r-6 l-7
// bts3 r-4 l-5
// bts4 r-2 l-3

Encoder enco(A, B);
int val=0, read=0;

void setup() {
  Serial.begin(115200);
  Serial.print("Serial started!!\n");
  pinMode(Rpwm, OUTPUT);
  pinMode(Lpwm, OUTPUT);
  enco.write(0);
}

int t2a(int etick){
  return map(etick, 0, tick, 0, 360);
}

void motor(int a, int b){
  analogWrite(Rpwm, a);
  analogWrite(Lpwm, b);
}

void loop() {
  if(Serial.available() > 0){
    val = Serial.parseInt();
    Serial.print(val);
    Serial.print("\t");
  }
  read = t2a(enco.read());

  if(val > 359){
    val -=360;
  }

  Serial.print(read);
  Serial.println("\t");
  if(val == read){
    motor(0, 0);
    Serial.print(read);
    Serial.println("\t");
  }

  if(val < read){
    motor(pwm , 0);
    Serial.println(read);
  }
  if(val > read){
    motor(0, pwm);
    Serial.println(read);
  }


}
