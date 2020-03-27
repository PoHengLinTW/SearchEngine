#include <iostream>
#include <fstream>
#include <cstring>

#include "test_url.h"
#include "md5.h"

#define max_seen_cnt 10000

/* max_crawl_cnt should be equal to MAX_CRAWL_LIMIT 
 * in crawler.cpp. Maybe it will be implenmented when
 * I create console system
 */
#define max_crawl_cnt 1000

test_url::test_url(/* args */)
{
    md5_buf = new std::string[max_seen_cnt];
    url_buf = new std::string[max_crawl_cnt];
    md5_cnt = 0;
    url_cnt = 0;
    curr_url = 0;
    seen.open("seenDB.txt", std::fstream::in);
    url.open("urlDB.txt", std::fstream::in); // out | std::fstream::app);
    /* read file to buf and then close file */
    if (seen) {
        while (std::getline(seen, md5_buf[md5_cnt])) {
            if (md5_cnt == max_seen_cnt-1) {
                std::cerr << "md5_cnt reach max_seen_cnt in reading file" << std::endl;
                break;
            }
            md5_cnt++;
        }
    }
    if (url) {
        while (std::getline(url, url_buf[url_cnt]))
        {
            if (url_cnt == max_crawl_cnt-1) {
                std::cerr << "url_cnt reach crawl in reading file" << std::endl;
                break;
            }
            url_cnt++;
        }
    }
    seen.close();
    url.close();
}

test_url::~test_url()
{
    seen.open("seenDB.txt", std::fstream::out);
    for (int i=0; i<=md5_cnt; i++) 
    {
        seen << md5_buf[i] << std::endl;
    }
    url.open("urlDB.txt", std::fstream::out);
    for (int i=0; i<url_cnt; i++) 
    {
        url << url_buf[i] << std::endl;
    }
    seen.close();
    url.close();
    // delete buf;
}

bool test_url::check_seen (std::string check_md5, std::string check_url)
{
    //std::ifstream seen_rw;
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
        md5_buf[md5_cnt++] = check_md5; 
        url_buf[url_cnt++] = check_url;
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
        // std::cout << href << std::endl << std::endl;
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
        std::cout << "memcpy" << std::endl;
        memcpy(url_link, href, len + 1); // don't use strcpy
        std::cout << url_link << std::endl;
        *tmp = ' ';
        std::string stmp(url_link);
        std::string md5tmp = md5(stmp);
        /* if seen -> write into buf, if not continue */
        check_seen(md5tmp, stmp);
        // seen
    }
}

std::string test_url::getTopUrl()
{
    return url_buf[curr_url++];
}