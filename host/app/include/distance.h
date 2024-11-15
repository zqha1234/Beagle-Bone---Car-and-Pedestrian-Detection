#ifndef DISTANCE_H_
#define DISTANCE_H_
#include <string>
using namespace std;

// calculate the distance accroding to the objec name and height in the camera frame
// use pin-hole camera model method
double Distance_Cal(string obj, double h);
// double getDistance(void);

#endif