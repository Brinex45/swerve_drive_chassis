#include<Encoder.h>

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

class Module{
  private:
    int index;
    int Rpwm,Lpwm;
    long cpr;
    int CA = 0,SA;
    Encoder& enco;
  public:
    int dir = 1;
    Module(int index, int Rpwm, int Lpwm, Encoder& enco, long cpr) : enco(enco){
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

    void actuate(int TA){
      int target_angle = short_angle(TA);
      set_angle(target_angle);
      // set_velocity();
    }
};

  Module Module1(1,r_pwm1,l_pwm1,enco1,cpr1);
  Module Module2(2,r_pwm2,l_pwm2,enco2,cpr2);
  Module Module3(3,r_pwm3,l_pwm3,enco3,cpr3);
  Module Module4(4,r_pwm4,l_pwm4,enco4,cpr4);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(l_pwm1, OUTPUT);
  pinMode(r_pwm1, OUTPUT);
  pinMode(l_pwm2, OUTPUT);
  pinMode(r_pwm2, OUTPUT);
  pinMode(l_pwm3, OUTPUT);
  pinMode(r_pwm3, OUTPUT);
  pinMode(l_pwm4, OUTPUT);
  pinMode(r_pwm4, OUTPUT);

}
int a;
void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    a = Serial.parseInt();
    // Serial.println(a);
  }
  Module1.actuate(a);
  Module2.actuate(a);
  Module3.actuate(a);
  Module4.actuate(a);

  Serial.println((String)Module1.dir+ "\t"+ Module2.dir +"\t"+ Module3.dir+"\t"+Module4.dir);
}
