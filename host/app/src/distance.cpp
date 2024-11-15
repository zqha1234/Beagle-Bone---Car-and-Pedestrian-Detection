#include "../include/distance.h"
#include <string>

using namespace std;
static double distance_ = 0.0;

// camera's focal length
double focal_length = 649.24;  //[cm]
double person_height = 170;    //[cm]
double car_height = 150;       //[cm]

// calculate the distance accroding to the objec name and height in the camera frame
// use pin-hole camera model method
double Distance_Cal(string obj, double h) {
    if (obj == "person") {
        distance_ = focal_length * person_height / h;
    } else if (obj == "car") {
        distance_ = focal_length * car_height / h;
    }
    return distance_; // unit[cm]
}

// double getDistance(void) {
//     return distance_;
// }