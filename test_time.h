/* std output pattern
std::cout 
    << Month[tm_local->tm_mon-1] << " "
    << tm_local->tm_mday << ","
    << tm_local->tm_year+1900 << " "
    << tm_local->tm_hour << ":"
    << tm_local->tm_min << ":"
    << tm_local->tm_sec << std::endl; 
*/
#ifndef TEST_TIME
#define TEST_TIME

#include <iostream>
#include <ctime>


static std::string Month[12] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

class test_time
{
    private:
        /* data */
        char *curr_time;
        time_t init_time;
        tm tm_start;
        tm *tm_local;
        void update();
    public:
        test_time(/* args */);
        ~test_time();
        char *getStartTime();
        char *getCurrTime();
        double diff();
};

#endif