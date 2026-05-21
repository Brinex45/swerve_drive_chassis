#include <Wire.h>
#include <Encoder.h>
#include <ODriveUART.h>

#define pwm 70

#define r_pwm1 9
#define l_pwm1 8

#define l_pwm2 7
#define r_pwm2 6

#define l_pwm3 5
#define r_pwm3 4

#define l_pwm4 3
#define r_pwm4 2

//Encoder Pins
int enco1A = 26;
int enco1B = 27;
//Encoder Pins
int enco2A = 33;
int enco2B = 32;
//Encoder Pins
int enco3A = 38;
int enco3B = 37;
//Encoder Pins
int enco4A = 31;
int enco4B = 30;

int velocity;
int axis = 0;

Encoder enco1(enco1A, enco1B);
Encoder enco2(enco2A, enco2B);
Encoder enco3(enco3A, enco3B);
Encoder enco4(enco4A, enco4B);

ODriveUART odrive1(Serial1);
ODriveUART odrive2(Serial5);
ODriveUART odrive3(Serial3);
ODriveUART odrive4(Serial8);

long cpr1 = 466104;
long cpr2 = 464239;
long cpr3 = 463951;
long cpr4 = 466490;

// long cpr = 464928;

// int leftX , leftY, oMega;
int mg, TA, CA, SA;
int dir = 0;
int flag = 0;
int vel = 0;
int x = 0, y = 5;
int px = 0, py = 0;
int total_mg = 0;

#define vel_const 0.8

byte psData[3] = {127,127,127};

void cal(int X, int Y){
  TA = (atan2(Y, X) * RAD_TO_DEG);
  TA = (TA + 360)%360;

  mg = sqrt(pow(X - px,2)+pow(Y - py,2));
  Serial.println((String)"TA" + TA + "\tMG" + mg);
}

class Module{
private:
  int index;
  int Rpwm, Lpwm;
  int cpr;
  int CA = 0,SA;
  Encoder& enco;
  ODriveUART& odrive;

public:
  Module(int index, int Rpwm, int Lpwm, Encoder& enco, ODriveUART& odrive, long cpr) : enco(enco), odrive(odrive){
    this-> index = index;
    this-> Rpwm = Rpwm;
    this-> Lpwm = Lpwm;
    this-> cpr = cpr;
  }

  // void cal(){
  //   if(abs(oMega) > 0)
  //     TA = 45 + (90 * index);
  // }

  void motor(int r, int l){
    analogWrite(Rpwm, r);
    analogWrite(Lpwm, l);
  }

  int t2a(){
    long tick = enco.read();
    tick = tick % cpr;
    tick = tick < 0 ? tick + cpr : tick;
    CA = map(tick, 0, cpr, 0, 360);
    return CA % 360;
  }

  // calculating velocity for bldc;
  int velocity(){
    return mg * vel_const * dir;
  }

  int short_angle(int TA) {
    int target_angle;
    CA = t2a();
    if(abs(TA - CA) <=90 || 270 <= abs(TA - CA)){
      target_angle = TA;
      dir = 1;
      return target_angle;
    }
    else{ 
      target_angle = TA - 180;
      target_angle = target_angle < 0 ? target_angle + 360 : target_angle;
      dir = -1;
      return target_angle;
    }
  }

  // setting the shortest angle by breaking the 360 barrier;
  void set_angle(int target_angle){
    int pArc, nArc;
    pArc = target_angle - CA;
    pArc = pArc >= 0 ? pArc : 360 + pArc;

    nArc = 360 - pArc;
    // if(flag == 1){  
      // Serial.println("aaaaaa");
      if(target_angle == CA){
        motor(0, 0);    
        // dir = 1;
      }
      else if (pArc < nArc && ((nArc - pArc) < (360 - 5))) {
        motor(pwm, 0);  
        // dir = -1;
      }
      else if(nArc < pArc && ((pArc - nArc) < (360 - 5))){
        motor(0, pwm);  
      }
    // }
      else {
        motor(0, 0);  
        // dir = 1;
      }
  }

  // setting the velocity of bldc;
  void set_velocity(){
    if(mg > 0){
      odrive.SetVelocity(axis, velocity());
    }
    else{
      odrive.SetVelocity(axis, 0);
    }
  }

  void navigate_up(){
    int flag = dir > 0? 1 : 0;
    if (flag) {
      total_mg += mg;
      odrive.setPosition(total_mg);
    }
    else {
      total_mg -= mg;
      odrive.setPosition(total_mg);
    }
  }

  void navigate_down(){
    int flag = dir > 0? 1 : 0;
    if (flag) {
      total_mg -= mg;
      odrive.setPosition(total_mg);
    }
    else {
      total_mg += mg;
      odrive.setPosition(total_mg);
    }
  }

  // making dc and bldc work togathe  r;
  void actuate(){
    int target_angle = short_angle(TA);
    set_angle(target_angle);
    int f = abs(TA - CA) <= 90 || abs(TA - CA) >= 270 ? 1:0;
    if (f) {
      navigate_up();
    }
    else{
      navigate_down();
    } 

  }

  void checkOdrive(){

    Serial.println("Waiting for ODrive");
    Serial.print(index);  
    while (odrive.getState() == AXIS_STATE_UNDEFINED) {
      delay(10);
    }

    Serial.print("DC voltage: ");
    Serial.println(odrive.getParameterAsFloat("vbus_voltage"));

    Serial.println("Enabling closed loop control...");
    while (odrive.getState() != AXIS_STATE_CLOSED_LOOP_CONTROL) {
      odrive.clearErrors();
      odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL);
      delay(10);
    }
  }
};

Module Module1(1,r_pwm1,l_pwm1,enco1,odrive1,cpr1);
Module Module2(2,r_pwm2,l_pwm2,enco2,odrive2,cpr2);
Module Module3(3,r_pwm3,l_pwm3,enco3,odrive3,cpr3);
Module Module4(4,r_pwm4,l_pwm4,enco4,odrive4,cpr4);

void setup(){
  Wire1.begin();
  Serial.begin(115200);

  Serial1.begin(115200);
  Serial5.begin(115200);
  Serial3.begin(115200);
  Serial8.begin(115200);

  Module4.checkOdrive();
  Module3.checkOdrive();

  pinMode(l_pwm1, OUTPUT);
  pinMode(r_pwm1, OUTPUT);
  pinMode(l_pwm2, OUTPUT);
  pinMode(r_pwm2, OUTPUT);
  pinMode(l_pwm3, OUTPUT);
  pinMode(r_pwm3, OUTPUT);
  pinMode(l_pwm4, OUTPUT);
  pinMode(r_pwm4, OUTPUT);
  delay(2000);
}

void loop(){
  // getData();

  cal(x,y);

  Module1.actuate();
  Module2.actuate();
  Module3.actuate();
  Module4.actuate();
  // Serial.println("efqef");
  // Serial.println(Module1.velocity());

  px = x;
  py = y;

}