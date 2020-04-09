#ifndef TEST_URL
#define TEST_URL

#include <fstream>
#include <string>

class test_url
{
    private:
        std::fstream seen_url;
        std::fstream failed_url;
        std::fstream crawled_url;
        std::fstream uncrawled_url;
        std::string *md5_buf;
        std::string *seen_url_buf;
        std::string *uncrawled_url_buf;
        int md5_cnt;
        int url_cnt;
        int curr_url;
        int failed_cnt;
        int crawled_cnt;
        bool check_seen(std::string, std::string);
    public:
        test_url(/* args */);
        ~test_url();
        void retrieveUrl(char *);
        void addfailedUrl(std::string, std::string);
        void addcrawledUrl(std::string);
        std::string getTopUrl();
        int getFailedCnt();
        int getCrawledCnt();
        int getUncrawledCnt(); /* url_cnt - curr_url */
};

#endif