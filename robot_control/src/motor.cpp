#include "motor.h"
#include "Arduino.h"
#define MOVE


void Motor::Init()
{
    pinMode(f_pin,OUTPUT);
    pinMode(b_pin,OUTPUT);
    is_initialized=true;
}

void Motor::attach(int f_pin,int b_pin)
{
    this->b_pin=b_pin;
    this->f_pin=f_pin;
}

void Motor::forward()
{
#ifdef MOVE
    digitalWrite(b_pin,0);
    digitalWrite(f_pin,1);
#endif
}

void Motor::backward()
{
#ifdef MOVE
    digitalWrite(f_pin,0);
    digitalWrite(b_pin,1);
#endif

}

void Motor::stop()
{
#ifdef MOVE
    digitalWrite(b_pin,0);
    digitalWrite(f_pin,0);
#endif

}