#include "obstacle_detector.h"


ObstacleDetector::ObstacleDetector(int servo_pin, int ultraSound_echo,int ultraSound_trig)
{
    this->servo_pin=servo_pin;
    this->ultraSound_echo=ultraSound_echo;
    this->ultraSound_trig=ultraSound_trig;
}


void ObstacleDetector::init()
{
    servo.attach(servo_pin,500,2500);
    pinMode(ultraSound_echo, INPUT);
    pinMode(ultraSound_trig, OUTPUT);

    servo.write(90);
}

double ObstacleDetector::get_distance()
{
    double distance, duration;

    digitalWrite(ultraSound_trig, LOW);
    delayMicroseconds(2);

    digitalWrite(ultraSound_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultraSound_trig, LOW);

    duration = pulseIn(ultraSound_echo, HIGH);

    distance = duration * 0.034 / 2;

    if(distance<1) distance=OBSTACLE_DISTANCE;
    if(distance>100) distance=100;
    this->frontDistance=distance;
    return distance;
}

side_t ObstacleDetector::check_sides()
{
    double l_distance=0, r_distance=0;
    servo.write(ANGLE_RANGE);
    delay(50);



    for(int i=0;i<6;i++)
    { 
        servo.write(i*15);
        vTaskDelay(100/portTICK_PERIOD_MS);
        l_distance+=get_distance();     //or right
    }

    for(int i=10;i>4;i--)
    {
        r_distance+=get_distance();     //or right
        vTaskDelay(100/portTICK_PERIOD_MS);
        servo.write(i*15);
    }
    if (l_distance<r_distance) return RIGHT;
    else return LEFT;
}




