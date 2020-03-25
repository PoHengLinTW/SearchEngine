#ifndef TEST_URL
#define TEST_URL

#include <fstream>
#include <string>

class test_url
{
    private:
        std::ifstream seen_r;
        std::ofstream seen_w;
        std::fstream url;
        std::string *buf;
        int cnt;
        bool check_seen(std::string);
    public:
        test_url(/* args */);
        ~test_url();
        void retrieveUrl(char *);
};

#endif