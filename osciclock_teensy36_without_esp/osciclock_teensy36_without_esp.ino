// oscilloscope clock with teensy 3.6
// weigu.lu v2 18/12/17

#include <TimeLib.h> //https://www.pjrc.com/teensy/td_libs_Time.html

const byte dac1 = A21;
const byte dac2 = A22;
const float pi = 3.14159265;
const byte res = 12;       // dac resolution in bit
int r = (pow(2,res)-2)/2;  // max circle radius
int rs = r*0.80;           // radius seconds
int rm = r*0.85;           // radius minutes
int rh = r*0.65;           // radius hours
int rst = r*0.95;          // radius strokes
int rstm = r*0.90;         // radius main strokes
int hands = 5;             // fraction where to split the hand
int handam = 12;           // angle (width) of hand (min)
int handah = 20;           // angle (width) of hand (hour)

void setup() {
  setSyncProvider(getTeensy3Time); // set the time library to use RTC
  analogWriteResolution(res);
}

void loop() {
  circle();    // function to make a circle
  sec_line(second());
  min_line(minute());
  hour_line(hour(),minute(),second());
  display_weigu(r*15/10,r/32,r/64);
}

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void point(int x,int y) {
  analogWrite(dac1, y);
  analogWrite(dac2, x);
}

void line (int x1,int y1,int x2,int y2) { // draw a line between 2 points
  int d = sqrt(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1)));
  int steps = d/4;
  for(int i=0;i<steps;i++) {
    point(x1+i*(x2-x1)/steps,y1+i*(y2-y1)/steps);
  }
}

void dline (int x1,int y1,int x2,int y2) { // draw a line back and forth
  int d = sqrt(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1)));
  int steps = d/4;
  for(int i=0;i<steps;i++) {
    point(x1+i*(x2-x1)/steps,y1+i*(y2-y1)/steps);
  }
  for(int i=0;i<steps;i++) {  //draw line backwards to avoid ghosts
    point(x2+i*(x1-x2)/steps,y2+i*(y1-y2)/steps);
  }
}

void hand (int x1,int y1,int x2,int y2, int angle) { // draw a hand between 2 points
  int d = sqrt(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1)));
  int ds = d/hands;
  int ao = round( atan2 (y2-y1,x2-x1)*180/pi );
  int a1 = ao+angle;
  int a2 = ao-angle;
  int p1x = x1+ds*cos(a1*pi/180);
  int p1y = y1+ds*sin(a1*pi/180);
  int p2x = x1+ds*cos(a2*pi/180);
  int p2y = y1+ds*sin(a2*pi/180);
  line (x1,y1,p1x,p1y);
  line (p1x,p1y,x2,y2);
  line (x2,y2,p2x,p2y);
  line (p2x,p2y,x1,y1);
}

void circle() { //function to draw a circle
  for(int i=0;i<=360;i++) {
    int circlex = r*cos(i*pi/180)+r;
    int circley = r*sin(i*pi/180)+r;
    point(circlex,circley);
    if (i%6==0) {  // min lines
      if (i%30==0) {  // 5 min lines
        dline(circlex,circley,rstm*cos(i*pi/180)+r,rstm*sin(i*pi/180)+r);
      }
      else {
        dline(circlex,circley,rst*cos(i*pi/180)+r,rst*sin(i*pi/180)+r);
      }
    }
  }
}

void  sec_line(int second) {
  int angle = ((60-second)*6+90)%360;
  dline(r,r,rs*cos(angle*pi/180)+r,rs*sin(angle*pi/180)+r);
}

void  min_line(int minute) {
  int angle = ((60-minute)*6+90)%360;
  hand(r,r,rm*cos(angle*pi/180)+r,rm*sin(angle*pi/180)+r,handam);
}

void  hour_line(int hour, int minute, int second) {
  if (hour>12) {
    hour = hour-12;
  }
  int hours = hour*3600 + minute*60 + second;
  int angle = ((43200-hours)/120+90)%360;
  hand(r,r,rh*cos(angle*pi/180)+r,rh*sin(angle*pi/180)+r,handah);
}

void display_weigu(int x1, int y1, int s) {
  int h = 5*s; // height of chars
  int w = 5*s; // width of chars
  int d = 2*s; // distance between chars
  line(x1,y1,x1,y1+h);
  line(x1,y1,x1+w/2,y1+h/2);
  line(x1+w/2,y1+h/2,x1+w,y1);
  line(x1+w,y1,x1+w,y1+h);
  x1 = x1+w+d;
  w = w*3/5;
  line(x1,y1,x1,y1+h);
  line(x1,y1+h,x1+w,y1+h);
  line(x1+w,y1+h,x1+w,y1+h/2);
  line(x1+w,y1+h/2,x1,y1+h/2);
  line(x1,y1,x1+w,y1);
  x1 = x1+w+d;
  line(x1,y1,x1,y1+h*3/4);
  point(x1,y1+h);
  point(x1+1,y1+h);
  point(x1+1,y1+1+h);
  point(x1,y1+1+h);
  x1 = x1+d;
  line(x1+w,y1,x1,y1);
  line(x1,y1,x1,y1+h);
  line(x1,y1+h,x1+w,y1+h);
  line(x1+w,y1+h,x1+w,y1);
  line(x1+w,y1,x1+w,y1-h/2);
  line(x1+w,y1-h/2,x1,y1-h/2);
  x1 = x1+w+d;
  line(x1,y1,x1,y1+h);
  line(x1,y1,x1+w,y1);
  line(x1+w,y1,x1+w,y1+h);
  x1 = x1+w+d;
  point(x1,y1);
  point(x1+1,y1);
  point(x1+1,y1+1);
  point(x1,y1+1);
  x1 = x1+d;
  line(x1,y1,x1,y1+1.5*h);
  x1 = x1+d;
  line(x1,y1,x1,y1+h);
  line(x1,y1,x1+w,y1);
  line(x1+w,y1,x1+w,y1+h);
}
