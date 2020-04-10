// cython: bridge of python and C
// ADD depth of roots
// multithread -> using atomic
// gumbo-parser


#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>


#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include "rmTag.h"
#include "test_url.h"
#include "test_time.h"

#define MAX_FILE_LENGTH 50000
/* extern variable */
int MAX_CRAWL_LIMIT = 1000;
int MAX_SEEN_CNT = 10000;
bool stop_flag = false;

/* for debug */
bool debug_flag = false;

static std::fstream output_file;
static char *m_pBuffer = NULL;
static size_t m_Size = 0;
static test_time *tm = new test_time();
static test_url *tu = new test_url();
static std::string curr_url;
static std::string url_ip; /* ip for current url */
static unsigned int total_site_size = 0;
static unsigned int total_url_length = 0;

void *Realloc(void *ptr, size_t size)
{
    if (ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
};

// Callback must be declared static, otherwise it won't link...
static size_t WriteMemoryCallback(char *ptr, size_t size, size_t nmemb)
{
    // Calculate the real size of the incoming buffer
    size_t realsize = size * nmemb;
    // (Re)Allocate memory for the buffer
    m_pBuffer = (char *)Realloc(m_pBuffer, m_Size + realsize);
    // Test if Buffer is initialized correctly & copy memory
    if (m_pBuffer == NULL)
    {
        realsize = 0;
    }
    //memset(&(m_pBuffer[m_Size]), '\0', realsize);
    memcpy(&(m_pBuffer[m_Size]), ptr, realsize);
    m_Size += realsize;
    // return the real size of the buffer...
    return realsize;
};

void write()
{
    output_file 
        << "fetch time: " << tm->getCurrTime() << std::endl
        << "Size: " << m_Size << std::endl
        << "Url:" << curr_url << std::endl
        << "Ip:" << url_ip << std::endl
        << "Content: " << std::endl
            << m_pBuffer << std::endl;
}

void getIpfromUrl()
{
    std::string tmp = curr_url;
    size_t location;
    if (curr_url.find("https://") != std::string::npos) {
        /* erase https:// */
        tmp.erase(0, 8);
    }
    else if (curr_url.find("http://") != std::string::npos) {
        /* erase https:/ */
        tmp.erase(0, 7);
    }
    else {
        std::cerr << "url error when trying to get ip" << std::endl
            << "url type is not https or http" << std::endl;
        url_ip = "";
        return;
    }

    /* remove the text after www.xxx.com includin '/' */
    location = tmp.find("/");
    if (location!=std::string::npos)
        tmp.erase(tmp.begin()+location, tmp.end());

    /* retrieve ip from url */
    hostent *record = gethostbyname(tmp.c_str());
    if (!record) {
        std::cerr << "not found record" << std::endl;
        url_ip = "";
        return;
    }
    in_addr *addr = (in_addr*) record->h_addr_list[0];
    url_ip = inet_ntoa(*addr);
}

void dashboard()
{
    /* Begin time
     * Last time
     * Total crawled cnt
     * Crawled cnt in last hour 
     * Crawled cnt in last day
     * Failure cnt and rate
     * Number in queue (uncrawled)
     * Average site size
     * Average url length
     * Average link cnt
     */
    system("clear");

    int len=125;

    /* Row 1*/
    std::cout << "|" << std::string(len, '=') << "|" << std::endl;
    std::cout 
        << "|" << " Begin time         "
        << "|" << " Current time       "
        << "|" << " Last time          "
        << "|" << " Total Crawled cnt  "
        << "|" << " Failure cnt / rate "
        << "|" << "                    "
        << "|" << std::endl;
    
    std::cout << "|" << std::string(len, '-') << "|" << std::endl;

    std::cout 
        << "|" << std::setw(20) << tm->getStartTime() // Begin time
        << "|" << std::setw(20) << tm->getCurrTime() // Begin time
        << "|" << std::setw(16) << tm->diff() << " sec" // Last time
        << "|" << std::setw(20) << tu->getCrawledCnt() // Total Crawled cnt
        << "|" << std::setw(20) << tu->getFailedCnt() // Failure cnt / rate
        << "|" << std::setw(20) << ""
        << "|" << std::endl;

    /* Row 2 */
    std::cout << "|" << std::string(len, '=') << "|" << std::endl;
    std::cout
        << "|" << " Number in queue    "
        << "|" << " Average site size  "
        << "|" << " Average url length "
        << "|" << " Average link cnt   "
        << "|" << " Max Crawl Limit    "
        << "|" << " Max Num in SeenDB  "
        << "|" << std::endl;

    std::cout << "|" << std::string(len, '-') << "|" << std::endl;
    std::cout 
        << "|" << std::setw(20) << tu->getUncrawledCnt() // Number in queue
        << "|" << std::setw(20) << total_site_size/tu->getCrawledCnt() // Average site size
        << "|" << std::setw(20) << total_url_length/tu->getCrawledCnt() // Average url length
        << "|" << std::setw(20) << "0" // Average link cntt / rate
        << "|" << std::setw(20) << MAX_CRAWL_LIMIT // Max Crawl Limit
        << "|" << std::setw(20) << MAX_SEEN_CNT // Max Num in SeenDB
        << "|" << std::endl;
    
    /* Row 3 */
    std::cout << "|" << std::string(len, '=') << "|" << std::endl;
    std::cout
        << "|" << std::setw(len) << " Current Url "
        << "|" << std::endl;

    std::cout << "|" << std::string(len, '-') << "|" << std::endl;
    std::cout 
        << "|" << std::setw(len) << curr_url // Current Url
        
        << "|" << std::endl;

    /* Row 4 */
    std::cout << "|" << std::string(len, '=') << "|" << std::endl;
    std::cout
        << "|" << " Current Ip         "
        << "|" << " Current Site size  "
        << "|" << " Distance from root "
        << "|" << "                    "
        << "|" << "                    "
        << "|" << "                    "
        << "|" << std::endl;

    std::cout << "|" << std::string(len, '-') << "|" << std::endl;
    std::cout
        << "|" << std::setw(20) << url_ip // Current Ip
        << "|" << std::setw(20) << m_Size // Current site size
        << "|" << std::setw(20) << "0" // Distance from root
        << "|" << std::setw(20) << ""
        << "|" << std::setw(20) << ""
        << "|" << std::setw(20) << ""
        << "|" << std::endl;

    std::cout << "|" << std::string(len, '=') << "|" << std::endl;
    //sleep(1);
}

int main(int, char **) /* I/O for save data, using dataa batch to control I/O count */
{
    output_file.open("test.txt", std::fstream::out | std::fstream::app);

    int crawl_cnt=0;

    for (int i=0; i<MAX_CRAWL_LIMIT; i++)
    {
        /* get next url from uncrawled DB*/
        curr_url = tu->getTopUrl();

        try
        {
            // initial process
            curlpp::Cleanup myCleanup;
            curlpp::Easy myRequest;

            // create curl options
            curlpp::options::Url myUrl(curr_url);//std::string("https://star.ettoday.net/news/718757"));
            curlpp::types::WriteFunctionFunctor functor(WriteMemoryCallback);
            curlpp::options::WriteFunction *write_func = new curlpp::options::WriteFunction(functor);

            // bind options
            myRequest.setOpt(write_func);
            myRequest.setOpt(myUrl);
            // myRequest.setOpt(new curlpp::options::Verbose(true));

            // perform
            myRequest.perform();
        }
        catch (curlpp::LogicError &e)
        {
            /* 
             * writing failed crawled url info
             * format: md5 url reason
             */
            tu->addfailedUrl(curr_url, std::string(e.what()));
            continue;
        }
        catch (curlpp::RuntimeError &e)
        {
            /* 
             * writing failed crawled url info
             * format: md5 url reason
             */
            tu->addfailedUrl(curr_url, std::string(e.what()));
            continue;
        }

        /* crawling successfully */
        tu->addcrawledUrl(curr_url);

        /* no url to crawl */
        if (m_Size == 0)
            return -1;
        else {
            total_site_size += m_Size;
            total_url_length += curr_url.length();
        }
        
        if (stop_flag) /* stop signal activated*/
            break;
        
        if (debug_flag) {
            std::cout << ++crawl_cnt << std::endl;
            std::cout << "rm script" << std::endl;
        }
        rmTag(m_pBuffer, "script");
        
        if (debug_flag)
            std::cout << "rm style" << std::endl;
        rmTag(m_pBuffer, "style");
        
        if (debug_flag)
            std::cout << "Get ip from url" << std::endl;
        getIpfromUrl();

        if (debug_flag) {
            std::cout << "write info to file" << std::endl;
            write();
        }

        if (debug_flag)
            std::cout << "retrieve URL" << std::endl;
        tu->retrieveUrl(m_pBuffer);

        if (debug_flag)
            std::cout << "retrieve body" << std::endl;
        // retrieveBody();

        /* dashboard will show up only when not in debug mode */
        if (!debug_flag)
            dashboard();
    }
    
    output_file.close();
    std::cout << "delete tu" << std::endl;
    delete tu;
    std::cout << "delete tm" << std::endl;
    delete tm;
    free(m_pBuffer);
    return 0;
}