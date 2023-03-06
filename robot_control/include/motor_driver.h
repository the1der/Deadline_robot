#ifndef MOTOR_DRIVER__H
#define MOTOR_DRIVER__H

#include "motor.h"
#include "Arduino.h"

class MotorDriver
{
    private:
        Motor r_motor, l_motor;
        int r_en, l_en;

    public:
        int speed;
        MotorDriver(int r_motor_f_pin,
                     int r_motor_b_pin,
                     int l_motor_f_pin,
                     int l_motor_b_pin,
                     int r_en, 
                     int l_en);

        void init();
        void forward();
        void backward();
        void stop();
        void turn_left();
        void turn_right();
        void set_speed(int speed);
        void set_l_speed(int speed);
        void set_r_speed(int speed);
        
        void dodge_left(int angle);
        void dodge_right(int angle);

};


#endif