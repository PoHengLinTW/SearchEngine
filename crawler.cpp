// ADD fetch time, success rate, fail rate

#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <netdb.h>
#include <arpa/inet.h>

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

static std::fstream output_file;
static char *m_pBuffer = NULL;
static size_t m_Size = 0;
static test_time *tm = new test_time();
static test_url *tu = new test_url();
static std::string curr_url;
static std::string url_ip; /* ip for current url */

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
        
        if (stop_flag) /* stop signal activated*/
            break;
        
        std::cout << ++crawl_cnt << std::endl;
        std::cout << "rm script" << std::endl;
        rmTag(m_pBuffer, "script");
        std::cout << "rm style" << std::endl;
        rmTag(m_pBuffer, "style");
        
        getIpfromUrl();
        std::cout << "write()" << std::endl;
        write();
        std::cout << "retrieve URL" << std::endl;
        tu->retrieveUrl(m_pBuffer);
        std::cout << "retrieve body" << std::endl;
        // retrieveBody();
        
    }
    
    output_file.close();
    std::cout << "delete tu" << std::endl;
    delete tu;
    std::cout << "delete tm" << std::endl;
    delete tm;
    free(m_pBuffer);
    return 0;
}