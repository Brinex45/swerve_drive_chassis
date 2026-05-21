#include <Wire.h>
#include<Encoder.h>
#include <ODriveUART.h>

ODriveUART odrive1(Serial1);
ODriveUART odrive2(Serial5);
ODriveUART odrive3(Serial3);
ODriveUART odrive4(Serial8);

int pos = 0;
int buf = 20;

// up 256
// down 1024
// left 2048
// right 512

#define SlaveUno 1
uint8_t highB;
uint8_t lowB;
uint8_t psbt[5] = {0};
uint16_t digitalPsbt = 0;
int x, y, w;



long cpr1 = 465098;
long cpr2 = 467189;
long cpr3 = 468765;
long cpr4 = 466072;

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

Encoder enco1(enco1A, enco1B);
Encoder enco2(enco2A, enco2B);
Encoder enco3(enco3A, enco3B);
Encoder enco4(enco4A, enco4B);

#define pwm 70

void getdata()
{
  Wire1.requestFrom(SlaveUno, 5);
  while ((Wire1.available() < 5));
  Wire1.readBytes(psbt, 5);
  x = map(psbt[0], 0, 255, -127, 128);
  y = map(psbt[1], 0, 255, 127, -128);
  w = map(psbt[2], 0, 255, -40, 40);


  digitalPsbt = psbt[4] | psbt[3] << 8;
  if (digitalPsbt != 0)
    Serial.println(digitalPsbt);


  if (x != 0 || y != 0) {
       Serial.print(String("x: ") + x);
       Serial.println(String("\t\ty: ") + y);
  }
   if (w != 0) Serial.println(String("\t\tw: ") + w);
  //    else  Serial.println(String("w: ") + w);
}

class Module{
  private:
    int index;
    int a;
    int Rpwm,Lpwm;
    long cpr;
    int CA = 0,SA;
    Encoder& enco;
    ODriveUART& odrive;
  public:
    int dir = 1;
    Module(int index, int Rpwm, int Lpwm, Encoder& enco,ODriveUART& odrive, long cpr) : enco(enco),odrive(odrive) {
      this-> index = index;
      this-> Rpwm = Rpwm;
      this-> Lpwm = Lpwm;
      this-> cpr = cpr;
    }

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

    void set_angle(int target_angle){
      // int flag = 1;
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
    int target_angle;
    void actuate(int TA){
      target_angle = short_angle(TA);
      set_angle(target_angle);
      // set_velocity();
    }

    void posup(){
      int flag = dir > 0 ? 1 : 0;
      if (flag) {
        a += buf;
        odrive.setPosition(a);
      }
      else{
        a -=buf;
        odrive.setPosition(a);
      }
    }

    void posdown(){
      int flag = dir > 0 ? 1 : 0;
      if (flag) {
        a -= buf;
        odrive.setPosition(a);
      }
      else{
        a +=buf;
        odrive.setPosition(a);
      }
    }
};

  Module Module1(1,r_pwm1,l_pwm1,enco1,odrive1,cpr1);
  Module Module2(2,r_pwm2,l_pwm2,enco2,odrive2,cpr2);
  Module Module3(3,r_pwm3,l_pwm3,enco3,odrive3,cpr3);
  Module Module4(4,r_pwm4,l_pwm4,enco4,odrive4,cpr4);


void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial5.begin(115200);
  Serial3.begin(115200);
  Serial8.begin(115200);
  Wire1.begin();

  odrive3.closedLoopState();
  odrive4.closedLoopState();

  odrive1.setPosition(0);
  odrive2.setPosition(0);
  odrive3.setPosition(0);
  odrive4.setPosition(0);

  delay(2000);
}
int ang = 0,f;
void loop() {
  getdata();
  // Serial.println(digitalPsbt);
  if(digitalPsbt == 256){
    // Serial.println("UP");
    Module1.posup();
    Module2.posup();
    Module3.posup();
    Module4.posup();
  }

  if(digitalPsbt == 1024){
    // Serial.println("DOWN");
    Module1.posdown();
    Module2.posdown();
    Module3.posdown();
    Module4.posdown();
  }

  // Serial.println((String)odrive1.getPosition()+ "\t"+odrive2.getPosition()+"\t"+odrive3.getPosition()+"\t"+odrive4.getPosition());

  if(digitalPsbt == 2048){
    Serial.println("LEFT");
    ang -= 90;
  }

  if(digitalPsbt == 512){
    Serial.println("RIGHT");
    ang += 90;
  }
    ang = (ang + 360)%360;

    Serial.println(ang);
    Module1.actuate(ang);
    Module2.actuate(ang);
    Module3.actuate(ang);
    Module4.actuate(ang);

    // Serial.println(enco4.read());
}


