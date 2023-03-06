#ifndef ENCODER__H
#define ENCODER__H

#include <Arduino.h>
#include <FreeRTOS.h>

class Encoder_pair
{

    private:
        int l_encoder_pin, r_encoder_pin;

    public:
        bool counters_en;
        int  right_count, left_count;
        Encoder_pair(int l_encoder_pin, int r_encoder_pin);
        void init();
        void reset_counters();
        void disable_counters();
        void enable_counters();
};



#endif