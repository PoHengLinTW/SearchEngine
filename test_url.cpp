#include <iostream>
#include <fstream>
#include <cstring>

#include "test_url.h"
#include "md5.h"

#define max_seen_cnt 10000
#define max_crawl_cnt 1000

/* max_crawl_cnt should be equal to MAX_CRAWL_LIMIT 
 * in crawler.cpp. Maybe it will be implenmented when
 * I create console system
 */

test_url::test_url(/* args */)
{
    /* initial */
    md5_buf = new std::string[max_seen_cnt];
    uncrawled_url_buf = new std::string[max_crawl_cnt];
    seen_url_buf = new std::string[max_seen_cnt];

    md5_cnt = 0;
    url_cnt = 0;
    curr_url = 0;
    failed_cnt = 0;
    crawled_cnt = 0;

    seen_url.open("seen_urlDB.txt", std::fstream::in);
    failed_url.open("failed_urlDB.txt", std::fstream::out | std::fstream::app);
    crawled_url.open("crawled_urlDB.txt", std::fstream::out | std::fstream::app);
    uncrawled_url.open("uncrawled_urlDB.txt", std::fstream::in); // out | std::fstream::app);
    /* read file to buf and then close file */
    if (seen_url) {
        while(seen_url >> md5_buf[md5_cnt] >> seen_url_buf[md5_cnt])
        {
            md5_cnt++;
            if (md5_cnt >= max_seen_cnt) {
                std::cerr << "seen_cnt reach max limit in reading file" << std::endl;
                break;
            }
        }
    }
    if (uncrawled_url) {
        while (std::getline(uncrawled_url, uncrawled_url_buf[url_cnt]))
        {
            if (url_cnt == max_crawl_cnt-1) {
                std::cerr << "url_cnt reach crawl in reading file" << std::endl;
                break;
            }
            url_cnt++;
        }
    }
    seen_url.close();
    uncrawled_url.close();
}

test_url::~test_url()
{
    seen_url.open("seen_urlDB.txt", std::fstream::out);
    for (int i=0; i<=md5_cnt; i++) 
    {
        seen_url << md5_buf[i] << " "
            << seen_url_buf[i] << std::endl;
    }
    uncrawled_url.open("uncrawled_urlDB.txt", std::fstream::out);
    for (int i=curr_url; i<url_cnt; i++) 
    {
        uncrawled_url << uncrawled_url_buf[i] << std::endl;
    }
    seen_url.close();
    failed_url.close();
    crawled_url.close();
    uncrawled_url.close();
    // delete buf;
}

bool test_url::check_seen (std::string check_md5, std::string check_url)
{
    for (int i=0; i<=md5_cnt; i++)
    {
        /* seen */
        if (md5_buf[i].compare(check_md5)==0) 
            return false;
    }

    /* not seen */
    /* exceed the limit */
    if (md5_cnt >= max_seen_cnt || url_cnt >= max_crawl_cnt) { 
        std::cerr << "cnt reach max_seen_cnt in adding md5 to buffer" << std::endl;
        return false;
    }
    else { 
        /* if buf[0] & buf[1] have data, 
         * cnt is 2 => new data should store 
         * in buf[2] & cnt will be 3
         */
        md5_buf[md5_cnt] = check_md5; 
        seen_url_buf[md5_cnt] = check_url;
        uncrawled_url_buf[url_cnt] = check_url;
        md5_cnt += 1;
        url_cnt += 1;
    }
    return true;
}

void test_url::retrieveUrl(char *m_pBuffer)
{
    char *href = m_pBuffer, url_link[256], *tmp;
    while (1)
    {
        int len = 0;
        if ((href = strstr(href, "href=\"http")) == NULL)
            break;

        tmp = href = href + strlen("href=\"");
        while (1)
        {
            if (*tmp=='\"' || *tmp=='\0' || *tmp=='<' || *tmp=='>' || *tmp==' ')
            {
                *tmp = '\0';
                break;
            }
            tmp++;
            len++;
        }

        memcpy(url_link, href, len + 1); // don't use strcpy
        *tmp = ' ';
        std::string stmp(url_link);
        std::string md5tmp = md5(stmp);
        /* if seen -> write into buf, if not continue */
        check_seen(md5tmp, stmp);
        // seen
    }
}

void test_url::addfailedUrl(std::string curr_url, std::string reason)
{
    failed_url
        << md5(curr_url) << " "
        << curr_url << " "
        << reason << std::endl;
}

void test_url::addcrawledUrl(std::string curr_url)
{
    crawled_url
        << md5(curr_url) << " "
        << curr_url << std::endl;
}

std::string test_url::getTopUrl()
{
    return uncrawled_url_buf[curr_url++];
}