#include <string>
#include "format.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hr = 0;
    int min = 0;
    int sec = 0;

    //Hours Exist, set 3 vars
    if (seconds / 60 >= 1 && ((seconds / 60) / 60 > 0)){
        min = seconds/ 60;
        sec = seconds % 60;

        hr = min / 60;
        min = min % 60;
    }
    //Minutes Exist
    else if (seconds / 60 >= 1){
       min = seconds/ 60;
       sec = seconds % 60;
    }
    //Seconds only
    else {
        sec = seconds;
    };

    //formating final string output to be HH:MM:SS
    std::string ans;
    std::stringstream sh;
    std::stringstream sm;
    std::stringstream ss;
    sh <<std::setw(2) << std::setfill('0') << hr;
    sm <<std::setw(2) << std::setfill('0') << min;
    ss <<std::setw(2) << std::setfill('0') << sec;
    ans = sh.str() + ":" + sm.str() + ":" + ss.str();
    return ans; 
}