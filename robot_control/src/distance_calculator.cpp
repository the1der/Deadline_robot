#include "distance_calculator.h"


void DistanceCalculator::calc_distance()
{
    //distance=steps/21*wheel_ratio*cos(yaw);
}

void DistanceCalculator::calc_reset()
{
    distance=0;
}

double DistanceCalculator::get_distance()
{
    return distance;
}