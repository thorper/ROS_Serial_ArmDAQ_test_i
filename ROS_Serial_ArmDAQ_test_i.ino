/* 
 * rosserial ADC Example
 * 
 * This is a poor man's Oscilloscope.  It does not have the sampling 
 * rate or accuracy of a commerical scope, but it is great to get
 * an analog value into ROS in a pinch.
 */

//#define USE_USBCON
#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif
//#define USE_USBCON
#include <ros.h>
//#include <rosserial_arduino/Adc.h>
#include <induced/AAS.h>
#include <ros/time.h>
#include <std_msgs/Time.h>

ros::NodeHandle nh;

//rosserial_arduino::Adc adc_msg;
std_msgs::Time timeNow;
induced::AAS arm_msg;
ros::Publisher p("arm_0", &arm_msg);

const byte interruptPin = 2;
volatile unsigned long rise_t = 0;
volatile unsigned long rise_old =0;
volatile unsigned long dt =0;
volatile int RPM;
volatile int nPoles = 8;

//unsigned long voltage;
//unsigned long current;
//unsigned long thrust;
//unsigned long ethrust;

void setup()
{ 
  pinMode(13, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  nh.initNode();
  attachInterrupt(digitalPinToInterrupt(interruptPin), whatTime, RISING);
  nh.advertise(p);
//  nh.getHardware()->setBaud(115200);
//  Serial.begin(57600);

}

//We average the analog reading to elminate some of the noise
int averageAnalog(int pin){
  int v=0;
  for(int i=0; i<4; i++) v+= analogRead(pin);
  return v/4;
}


void loop()
{
  arm_msg.header.stamp = nh.now();
  arm_msg.header.frame_id = "motors";
  arm_msg.voltage = ((5.0 * averageAnalog(0))/10)*(9/5)+32;
  arm_msg.current = averageAnalog(1);
  arm_msg.thrust = averageAnalog(2);
  arm_msg.ethrust = 0.5*averageAnalog(1);
  arm_msg.rpm = RPM;

//  voltage = 

  p.publish(&arm_msg);
  nh.spinOnce();
//  Serial.print(" V: ");
//  Serial.print(arm_msg.voltage);
//  Serial.print(" A: ");
//  Serial.print(arm_msg.current);
//  Serial.print(" T: ");
//  Serial.print(arm_msg.thrust);
//  Serial.print(" eT: ");
//  Serial.print(arm_msg.ethrust);
//  Serial.print(" RPM: ");
//  Serial.print(arm_msg.rpm);
//  Serial.print(" rise_t: ");
//  Serial.print(rise_t);
//  Serial.print(" rise_old: ");
//  Serial.print(rise_old);
//  Serial.print(" dt: ");
//  Serial.println(dt);

  
}

void whatTime(){
  //use micros() for >100 hz may overrun for slower frequencies
  //use millis() for <100 hz accuracy suffers vs micros() but no overflow
  rise_t = micros(); //watch out micros() may overrun for large delays.
  dt = rise_t - rise_old;
  RPM = 1000000*60/(nPoles*dt);
  //RPM = 1000000*60/(8*dt);
//  RPM = 1000000*60/(8*(rise_t - rise_old));
  rise_old = rise_t;

}

