#ifndef MAIN__H
#define MAIN__H

    #include "Arduino.h"
//Motor pins
#define RF_MOTOR 14
#define RB_MOTOR 27
#define LF_MOTOR 26
#define LB_MOTOR 25
#define R_EN_pin  13
#define L_EN_pin  12 //lest

//Servo
#define SERVO_PIN 4

//Ultrasound
#define TRIG_PIN 18     //green
#define ECHO_PIN 19     //brown

//buzzer
#define BUZZ_PIN  15

//encoders
#define R_ENCODER 33
#define L_ENCODER 32

#define ANGLE_RANGE 15
#define DODGE_ANGLE 0.5
#define DODGE_DISTANCE 30

#define KD 1
#define  OBSTACLE_DISTANCE 40

typedef struct 
{
    double x,y;
} distance_t;


typedef struct 
{
    double x,y,x0;
    
} init_data_t;

typedef enum 
{
    forward,
    find_path,
    big_left,
    big_right,
    dodge_left,
    dodge_right,
    stop,
    face_found,
} main_states_t;
typedef enum 
{
    normal_mode,
    left_dodging_mode,
    right_dodging_mode,
    left_to_normal,
    right_to_normal,
    end_loop,
} alt_states_t;

#endif