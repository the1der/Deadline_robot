#ifndef MPU__H
#define MPU__H

    #include <Adafruit_MPU6050.h>
    #include <Adafruit_Sensor.h>
    #include <Wire.h>
    #include <FreeRTOS.h>

#define ERROR_SAMPLE 3000

typedef struct 
{
    float x,y,z;
} Angle;

typedef struct 
{
    float x,y,z;
} Error;

typedef struct
{
        float rolla, pitcha, yawa;
} anglesData_t;

typedef struct 
{
    float x,y,z;
    Angle angle;
    Error error;
} Acc;

typedef struct 
{
    float x,y,z;
    Angle angle;
    Error error;
} Gyro;



class Mpu
{
    private:
        Adafruit_MPU6050 mpu;
        sensors_event_t a, g,temp;
        Acc acc;
        Gyro gyro;
        float elapsedTime, currentTime, previousTime;
        SemaphoreHandle_t data_sem;

    public:
        anglesData_t anglesData;



    void resetGyroAngles();
    void IMU_calib();
    void init();
    void calculate();
    anglesData_t get_angelesData();
};


#endif