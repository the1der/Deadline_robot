#ifndef MOTOR__H
#define MOTOR__H

class Motor 
{
    private:
        bool is_initialized=false;
        int f_pin, b_pin,speed;

    public:
        void Init();
        void attach(int f_pin,int b_pin);
        void forward();
        void backward();
        void stop();
};


#endif