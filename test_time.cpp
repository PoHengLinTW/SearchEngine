#include <string>
#include "test_time.h"

test_time::test_time(/* args */)
{
    init_time = NULL;
    curr_time = (char *)malloc(sizeof(char) * (strlen("MMM DD,YYYY HH:MM:SS") + 1));
}

test_time::~test_time()
{
    free(curr_time);
}

void test_time::update()
{
    init_time = time(NULL);
    tm_local = localtime(&init_time);
    sprintf(curr_time, "%s %d,%d %d:%d:%d"
        , Month[tm_local->tm_mon-1].c_str()
        , tm_local->tm_mday
        , tm_local->tm_year+1900
        , tm_local->tm_hour
        , tm_local->tm_min
        , tm_local->tm_sec);
}

char *test_time::getCurrTime()
{
    update();
    return curr_time;
}