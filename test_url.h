#ifndef TEST_URL
#define TEST_URL

#include <fstream>
#include <string>

class test_url
{
    private:
        std::fstream seen;
        //std::ofstream seen_w;
        std::fstream url;
        std::string *md5_buf;
        std::string *url_buf;
        int md5_cnt;
        int url_cnt;
        int curr_url;
        bool check_seen(std::string, std::string);
    public:
        test_url(/* args */);
        ~test_url();
        void retrieveUrl(char *);
        std::string getTopUrl();
};

#endif