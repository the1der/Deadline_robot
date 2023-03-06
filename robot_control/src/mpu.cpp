#include "mpu.h"

  void Mpu::resetGyroAngles() 
  {
        mpu.getEvent(&a, &g,&temp);
        anglesData.pitcha  = (atan(a.acceleration.y / sqrt(pow(a.acceleration.x, 2) + pow(acc.z, 2))) * 180 / PI);
        anglesData.rolla  = (atan(-1 * a.acceleration.x / sqrt(pow(a.acceleration.y, 2) + pow(acc.z, 2))) * 180 / PI);
        anglesData.yawa = 0;
  }

  void Mpu::IMU_calib()
  { 
    acc.error.x = 0;
    acc.error.y = 0;

    for (int c=0 ;c< ERROR_SAMPLE;c++)
    {
        mpu.getEvent(&a, &g,&temp);

        acc.x = (a.acceleration.x);
        acc.y = (a.acceleration.y);
        acc.z = (a.acceleration.z);
        // Sum all readings
        acc.error.x += (atan(acc.y / sqrt(pow(acc.x, 2) + pow(acc.z, 2))) * 180 / PI) ;  // acc.error.x ~(0.58) See the calculate_IMU_error()custom function for more details
        acc.error.y += (atan(-1 * acc.x / sqrt(pow(acc.y, 2) + pow(acc.z, 2))) * 180 / PI) ; // acc.error.y ~(-1.58)
    }
    
    //Divide the sum by 200 to get the error value
    acc.error.x /= ERROR_SAMPLE;
    acc.error.y /= ERROR_SAMPLE;
    
    
    gyro.error.x = 0;
    gyro.error.y = 0;
    gyro.error.z = 0;
    // Read gyro values 200 times
    for (int c=0 ;c< ERROR_SAMPLE;c++)
    {
        mpu.getEvent(&a, &g,&temp);

        gyro.x = g.gyro.x;
        gyro.y = g.gyro.y;
        gyro.z = g.gyro.z;

        // Sum all readings
        gyro.error.x = gyro.error.x + (gyro.x);
        gyro.error.y = gyro.error.y + (gyro.y);
        gyro.error.z = gyro.error.z + (gyro.z);


    }
    
    //Divide the sum by 200 to get the error value
    gyro.error.x /= ERROR_SAMPLE;
    gyro.error.y /= ERROR_SAMPLE;
    gyro.error.z /= ERROR_SAMPLE;
    
    Serial.println("imu calibrated");
    vTaskDelay(100/portTICK_PERIOD_MS );
  }

void Mpu::init()
{
    if (!mpu.begin()) 
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1) delay(10); //infinite Loop   
    }

    Serial.println("MPU6050 Found!");

    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);    
    mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);

    delay(100);
    IMU_calib();
    resetGyroAngles();

    Serial.println("MPU initialized.. .");
    data_sem=xSemaphoreCreateRecursiveMutex();
}


void Mpu::calculate()
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);


    gyro.x = g.gyro.x - gyro.error.x;
    gyro.y = g.gyro.y - gyro.error.y;
    gyro.z = g.gyro.z - gyro.error.z; 

    acc.x = (a.acceleration.x);
    acc.y = (a.acceleration.y);
    acc.z = (a.acceleration.z);

    acc.angle.x = (atan(acc.y / sqrt(pow(acc.x, 2) + pow(acc.z, 2))))  - acc.error.x;  // acc.error.x ~(0.58) See the calculate_IMU_error()custom function for more details
    acc.angle.y = (atan(-1 * acc.x / sqrt(pow(acc.y, 2) + pow(acc.z, 2)))) - acc.error.y; // acc.error.y ~(-1.58)
    

    previousTime = currentTime;        // Previous time is stored before the actual time read
    currentTime = millis();            // Current time actual time read
    elapsedTime = (currentTime - previousTime) / 1000;


    anglesData.pitcha = 0.8 * (anglesData.pitcha  + ((gyro.x * elapsedTime))) + 0.2 * acc.angle.x ;
    anglesData.rolla = 0.8 * (anglesData.rolla + ((gyro.y * elapsedTime))) + 0.2 * acc.angle.y;
    anglesData.yawa = anglesData.yawa + (gyro.z * elapsedTime);

}

anglesData_t Mpu::get_angelesData()
{
    anglesData_t ret_data;

    xSemaphoreTake(data_sem,portMAX_DELAY);
    ret_data=anglesData;
    xSemaphoreAltGive(data_sem);

    return ret_data;
}