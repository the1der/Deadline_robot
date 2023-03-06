#include "encoder.h"

Encoder_pair::Encoder_pair(int l_encoder_pin, int r_encoder_pin)
{
    this->l_encoder_pin=l_encoder_pin;
    this->r_encoder_pin=r_encoder_pin;
}

void Encoder_pair::init()
{
    //set interrupt
    pinMode(l_encoder_pin,0);
    pinMode(r_encoder_pin,0);

    counters_en=true;
    left_count=0;
    right_count=0;

}



void Encoder_pair::reset_counters()
{
    left_count=0;
    right_count=0;
}

void Encoder_pair::disable_counters()
{
    counters_en=false;
}

void Encoder_pair::enable_counters()
{
    counters_en=true;
}
