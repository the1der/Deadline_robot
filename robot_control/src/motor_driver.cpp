#include "motor_driver.h"

MotorDriver::MotorDriver(int r_motor_f_pin,
                           int r_motor_b_pin,
                           int l_motor_f_pin,
                           int l_motor_b_pin,
                           int r_en, 
                           int l_en)
{
    this->l_en=l_en;
    this->r_en=r_en;
    l_motor.attach(l_motor_f_pin,l_motor_b_pin);
    r_motor.attach(r_motor_f_pin,r_motor_b_pin);
}

void MotorDriver::init()
{
    l_motor.Init();
    r_motor.Init();
    pinMode(r_en,OUTPUT);
    pinMode(l_en,OUTPUT);
}

void MotorDriver::forward()
{

    l_motor.forward();
    r_motor.forward();
}

void MotorDriver::backward()
{
    l_motor.backward();
    r_motor.backward();
}

void MotorDriver::stop()
{
    l_motor.stop();
    r_motor.stop();
}

void MotorDriver::turn_right()
{
    // stop();

    r_motor.backward();
    l_motor.forward();
}
void MotorDriver::turn_left()
{
    // stop();

    l_motor.backward();
    r_motor.forward();
}

void MotorDriver::set_speed(int speed)
{
    this->speed=speed;
}

void MotorDriver::set_l_speed(int speed)
{
    if(speed<0) speed=0;
    if(speed>250) speed=250;
    ledcWrite(l_en, speed);
}

void MotorDriver::set_r_speed(int speed)
{
    if(speed<0) speed=0;
    if(speed>250) speed=250;
    ledcWrite(r_en, speed);
}