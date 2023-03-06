#include <Arduino.h>
#include "main.h"
#include "mpu.h"
#include "motor_driver.h"
#include "encoder.h"
#include "FreeRTOS.h"
#include "cmath"
#include "obstacle_detector.h"

#define MOVE

#define init_speed 180

//Modules
MotorDriver motorDriver= MotorDriver(RF_MOTOR, RB_MOTOR, LF_MOTOR, LB_MOTOR, R_EN_pin, L_EN_pin); 
Encoder_pair encoder_pair= Encoder_pair(L_ENCODER, R_ENCODER);
ObstacleDetector obstacleDetector= ObstacleDetector(SERVO_PIN, ECHO_PIN, TRIG_PIN);
Mpu mpu;
char main_states_p[8][20] = {"forward", "find_path", "big_left", "big_right", "dodge_left", "dodge_right", "stop", "face_found"};



//global variables
main_states_t main_states=stop,current_state;
double desired_angle;
int angle_PI_2=0;
bool callibreate=true;
double error_angle;


//functions declaration

//tasks
void main_task(void * parameter);
void pid_callibrate(void * parameter);
void communicate_task(void * parameter);
void angle_task(void * parameter);
void radar_task(void * parameter);

//other
void encoder_left_isr();
void encoder_right_isr();
void turn_left(double angle);
void turn_right(double angle);

void setup() 
{
    Serial.begin(9600);
    motorDriver.init();

    //set analog write
    pinMode(12,OUTPUT);
    pinMode(13,OUTPUT);

    ledcSetup(12,30000,8);
    ledcSetup(13,30000,8);

    ledcAttachPin(13,12);
    ledcAttachPin(12,13);


    pinMode(BUZZ_PIN,OUTPUT);
    ledcAttachPin(BUZZ_PIN,8);
    ledcSetup(8,30000,8);
    ledcWriteTone(8,0);


    mpu.init();
    encoder_pair.init();
    attachInterrupt(L_ENCODER, encoder_left_isr, RISING);
    attachInterrupt(R_ENCODER, encoder_right_isr, RISING);

    obstacleDetector.init();
    mpu.calculate();
    desired_angle= mpu.anglesData.yawa;

    Serial.println(desired_angle);


    Serial.println("creating tasks");

    motorDriver.set_speed(init_speed);
    motorDriver.set_l_speed(init_speed);
    motorDriver.set_r_speed(init_speed);




void radar_task(void * parameter);

    
    xTaskCreate(main_task, "main task", 1200, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(pid_callibrate, "pid task", 2000, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(angle_task, "angle task", 2000, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(communicate_task, "communication task", 768, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(radar_task, "communication task", 800, NULL, tskIDLE_PRIORITY, NULL);

}

void loop() 
{

    vTaskDelay(200/portTICK_PERIOD_MS);

    Serial.print("state ");
    Serial.print(main_states_p[main_states]);

    Serial.print(" - Angle ");
    Serial.print(mpu.anglesData.yawa);
    Serial.print(" - Desired angle ");
    Serial.print(desired_angle+angle_PI_2*PI/2);

    Serial.print(" - Distance ");
    Serial.println(obstacleDetector.get_distance());




}



void main_task(void * parameter)
{
    while(1)
        {
        vTaskDelay(10/portTICK_PERIOD_MS);
        switch (main_states)
            {

            case forward:
                motorDriver.forward();
                break;

            case find_path:
                motorDriver.stop();
                if(obstacleDetector.check_sides()==LEFT)
                {
                    main_states=big_left;
                }
                else 
                {
                    main_states=big_right;
                }
                break;

            case dodge_left:
                motorDriver.stop();
                turn_left(DODGE_ANGLE*PI/2);
                angle_PI_2+=DODGE_ANGLE;
                motorDriver.stop();
                main_states=forward; 
                break;

            case dodge_right:
                motorDriver.stop();
                turn_right(DODGE_ANGLE*PI/2);
                angle_PI_2-=DODGE_ANGLE;
                motorDriver.stop();
                main_states=forward;

            case big_left:
                turn_left(PI/2);
                angle_PI_2+=PI/2;
                main_states=forward;
                break;

            case big_right:
                turn_right(PI/2);
                angle_PI_2-=PI/2;
                main_states=forward;
                break;

            case stop:
                motorDriver.stop();
                break;

            case face_found:
                motorDriver.stop();
                for(int i=0;i<3;i++)
                {
                    ledcWriteTone(8,750);
                    vTaskDelay(500/portTICK_PERIOD_MS);

                    ledcWriteTone(8,0);
                    vTaskDelay(500/portTICK_PERIOD_MS);
 
                }
                main_states=forward;
                break;
            }
        }
}


void pid_callibrate(void * parameter)
{
    while(1)
    {
        if(main_states==forward)
        {    
            vTaskDelay(20/portTICK_PERIOD_MS);
            // mpu.calculate();
            error_angle=desired_angle+angle_PI_2*PI/2;
            error_angle-= mpu.anglesData.yawa;

            //+right  -left
            motorDriver.set_l_speed(motorDriver.speed+KD*error_angle);
            motorDriver.set_r_speed(motorDriver.speed-KD*error_angle);
            //maybe fix refresh rate
        }
    }
}


void encoder_left_isr()
{
    if(encoder_pair.counters_en)
    {
        // delay(10);
        if (digitalRead(L_ENCODER)==1)
            encoder_pair.left_count++;
    }
}

void encoder_right_isr()
{
    if(encoder_pair.counters_en)
    {
        // delay(10);
        if (digitalRead(R_ENCODER)==1)
            encoder_pair.right_count++;  
    } 
}

void turn_left(double angle)
{
    motorDriver.stop();
    callibreate=false;
    while(mpu.anglesData.yawa<(desired_angle+angle_PI_2*PI/2+angle))
    {
        motorDriver.turn_right();
    }
    motorDriver.stop();
    callibreate=true;
}

void turn_right(double angle)
{
    motorDriver.stop();
    callibreate=false;

    while(mpu.anglesData.yawa>(desired_angle+angle_PI_2*PI/2-angle))
    {
        motorDriver.turn_left();
    }
    motorDriver.stop();
    callibreate=true;
}

void communicate_task(void * parameter)
{
    char r;
    while(1)
    {
        vTaskDelay(10/portTICK_PERIOD_MS);
        if(Serial.available()) 
        {
            r=Serial.read();
            switch (r)
            {
            case 's':
                if(main_states==stop)   main_states=forward;
                break;
            
            case 'p':
                main_states=stop;
                break;

            case 'f':
                main_states=face_found;
                break;

            case 'r':
                main_states=forward;
                break;
            }
        }
    }
}

void angle_task(void * parameter)
{
    while(1)
    {
        vTaskDelay(15/portTICK_PERIOD_MS);
        mpu.calculate();
    }
}

void radar_task(void * parameter)
{
    bool detected=false;
    while(1)
    {
        if(main_states==forward)
        {
            detected=false;
            if(!detected)
            {
                for(int i=3;i<9;i++)
                { 
                    obstacleDetector.servo.write(i*15);
                    if(obstacleDetector.get_distance()<OBSTACLE_DISTANCE)
                    {
                        main_states=stop;
                        vTaskDelay(100/portTICK_PERIOD_MS);
                        if(i<3) main_states=dodge_right; 
                        else    if(i>7) main_states=dodge_left;
                                else main_states=find_path;
                        detected=true;
                        break;
                        
                    }
                    vTaskDelay(100/portTICK_PERIOD_MS);
                }
            }
            if(!detected)
            {
                for(int i=9;i>2;i--)
                {

                    obstacleDetector.servo.write(i*15);
                    if(obstacleDetector.get_distance()<OBSTACLE_DISTANCE)
                    {
                        main_states=stop;
                        vTaskDelay(100/portTICK_PERIOD_MS);
                        if(i<3) main_states=dodge_right; 
                        else    if(i>7) main_states=dodge_left;
                                else main_states=find_path;
                        detected=true;
                        Serial.print("detected: ");
                        Serial.println(i);
                        break;
                    }
                    vTaskDelay(100/portTICK_PERIOD_MS);

                }
            }
        }

        vTaskDelay(15/portTICK_PERIOD_MS);
    }
}

