#ifndef OBSTACLE_DETECTOR__H
#define OBSTACLE_DETECTOR__H

#include "Arduino.h"
#include "main.h"
#include "ESP32_Servo.h"

typedef enum {LEFT, RIGHT} side_t;


class ObstacleDetector
{
    int servo_pin, ultraSound_echo, ultraSound_trig, actualAngle;
    public:
        Servo servo;
        double frontDistance;
        ObstacleDetector(int servo_pin, int ultraSound_echo,int ultraSound_trig);
        void init();
        double get_distance();
        side_t check_sides();
        void radar();
};


#endif