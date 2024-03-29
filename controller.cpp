#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>
#include <iostream>

#define MAX_SPEED 20
#define TIME_STEP 32

using namespace webots;

int main(int argc, char **argv) {

  //inicjalizacja robota, silników oraz czujników
  Robot *robot = new Robot();

  Motor *left_back_motor=robot->getMotor("left_back");
  Motor *right_back_motor=robot->getMotor("right_back");
  
  right_back_motor->setPosition(INFINITY);
  left_back_motor->setPosition(INFINITY);
  
  right_back_motor->setVelocity(0.0);
  left_back_motor->setVelocity(0.0);
  
  DistanceSensor *sensor[5];
  std::string names[5]={"ir0","ir1","ir2","ir3","ir4"};
  for(int i=0; i<5; i++)
  {
    sensor[i]=robot->getDistanceSensor(names[i]);
    sensor[i]->enable(TIME_STEP);
  }
  
  int readings[5]={0,0,0,0,0}; //odczyty z czujników
  int coefficient[5]={-3000,-1000,0,1000,3000}; //wagi
  int k=0;
  double prev_err=0; //poprzedni błąd
  double err=0; //aktualny błąd
  double kp=12; //mnożnik współczynnika proporcjonalnego P
  double kd=4;  //mnożnik współczynnika różnicującego D
  double P=0;
  double D=0;
  double correction=0; //korekcja
  double l_speed=MAX_SPEED;
  double r_speed=MAX_SPEED;
  
  
  while (robot->step(TIME_STEP) != -1) {
  
   err=0;
   k=0;
   for(int i=0; i<5; i++)
    {
      if(sensor[i]->getValue()==1000) //wyczucie czarnej linii
        readings[i]=1;
      else 
      {
        readings[i]=0;
        k++;
      }
      err+=coefficient[i]*readings[i];
    }
 
   if(k==5) //jezeli nie ma żadnego odczytu czarnej linni, ten sam błąd
     err=prev_err;
       
   P=err*kp; //element proporcjonalny
   D=kd*(err-prev_err); //element różnicujący
   
   correction=(P-D)/1000;
   prev_err=err;
  
   l_speed=MAX_SPEED-correction;
   r_speed=MAX_SPEED+correction;
   
   //maksymalny obrót kół
   if(l_speed>200)
     l_speed=200;
   if(l_speed<-200)
     l_speed=-200;
   if(r_speed>200)
     r_speed=200;
   if(r_speed<-200)
     r_speed=-200;

   //meta - zatrzymanie
   if(readings[1]&&readings[3]&&readings[2])
   {
       if(readings[0]==0&&readings[4]==0)
       {
          l_speed=0;
          r_speed=0;
       }
   }
   
   right_back_motor->setVelocity(r_speed);
   left_back_motor->setVelocity(l_speed);
   
  };

  //wyczyszczenie pamięci
  delete robot;
  delete left_back_motor;
  delete right_back_motor;
  
  for(int i=0; i<5; i++)
  {
      delete sensor[i];
  }
  
  return 0;
}