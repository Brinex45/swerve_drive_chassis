#include <Wire.h>
#include <Encoder.h>
#include <ODriveUART.h>

#define Rpwm_3 4
#define Lpwm_3 5
#define encoA_3 38
#define encoB_3 37

#define cpr 464928
#define pwm 70

#define vel_const 0.5

Encoder enco3(encoA_3, encoB_3);
ODriveUART odrive3(Serial3);

int leftX , leftY, oMega;
int mg, TA, CA, SA;
int dir = 0;
int flag = 0;
int vel = 0;

byte psData[3] = {127,127,0};

// converting encoder ticks to angle;
int t2a(long long tick){
  int ang;
  tick = tick % cpr;
  tick = tick < 0 ? tick + cpr : tick;
  ang = map(tick, 0, cpr, 0, 360);
  ang = ang % 360;
  return ang;
}

// calculating velocity for bldc;
int velocity(){
  return mg * vel_const * dir;
}

// encoder motor direction;
void motor(int r, int l){
  analogWrite(Rpwm_3, r);
  analogWrite(Lpwm_3, l);
}
  
// getting raw data required for computing and calculating
void getData() {

// reciving data from PS4 ---(Bluetooth)---> UNO(HOSTSHIELD)(SPI) ---(I2C)---> Teensy ;
  Wire1.requestFrom(8, 3);
  while (Wire1.available()>2) {
    
    Wire1.readBytes(psData, 3);
    leftX = psData[0];
    leftY = psData[1];
    oMega = psData[2];
    leftX = map(leftX, -1, 255, -127, 127);
    leftY = map(leftY, -1, 255, 127, -127);
    oMega = map(oMega,  0, 255, 127, -127);
  }
    // calculating angle of ps4 joystick using its X and Y co-ordinate;
    TA = (atan2(leftX,leftY) * RAD_TO_DEG);
    TA = (TA + 360)%360;
    
    // calculating magnitude of ps4 joystick using its X and Y co-ordinate;
    mg = sqrt(pow(leftX,2)+pow(leftY,2));
    flag = mg>50?1:0;
    mg*=flag;

    // reading ticks from encoder and getting current angle ;
    long long tick = enco3.read();
    CA = t2a(tick);
}

// algorithm for getting shortest angle ;
int short_angle() {

  int target_angle;

    if(abs(TA - CA) <=90 || 270 <= abs(TA - CA)){
      target_angle = TA;
      return target_angle;
    }
    else{
      target_angle = TA - 180;
      target_angle = target_angle < 0 ? target_angle + 360 : target_angle;
      return target_angle;
    }
}

// setting the shortest angle by breaking the 360 barrier;
void set_angle(int target_angle){

  int pArc, nArc;
  pArc = target_angle - CA;
  pArc = pArc >= 0 ? pArc : 360 + pArc;

  nArc = 360 - pArc;
  if(flag == 1){  
    // Serial.println("aaaaaa");
    if(target_angle == CA){
      motor(0, 0);    
      dir = 1;
    }
    else if (pArc < nArc && ((nArc - pArc) < (360 - 5))) {
      motor(pwm, 0);  
      dir = -1;
    }
    else if(nArc < pArc && ((pArc - nArc) < (360 - 5))){
      motor(0, pwm);  
      dir = -1;
    }
  }
    else {
      motor(0, 0);  
      dir = 1;
    }
}

// setting the velocity of bldc;
void set_velocity(){
  if(mg > 0){
    odrive3.SetVelocity(0, velocity());
  }
  else{
    odrive3.SetVelocity(0, 0);
  }
}

// making dc and bldc work togather;
int x;
void actuate(){
    x = short_angle();
    set_angle(x);
    set_velocity();
}

void setup(){
  Wire1.begin();
  Serial.begin(115200);
  Serial3.begin(115200);
  odrive3.closedLoopState();

  pinMode(Rpwm_3, OUTPUT);
  pinMode(Lpwm_3, OUTPUT);

}
void loop(){
  getData();
  actuate();

 Serial.println((String)/*short angle "+ short_angle()  +*/"TA "+ TA +"\tCA "+ CA + "\tvelocity "+ velocity()); 
  // if(Serial.available() > 0){
  //   x = Serial.parseInt();
  // }

  // set_angle(x);
}