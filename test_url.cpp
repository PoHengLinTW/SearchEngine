#include <iostream>
#include <fstream>
#include <cstring>

#include "test_url.h"
#include "md5.h"

#define seencnt 1000

test_url::test_url(/* args */)
{
    // buf = new std::string[seencnt];
    cnt = 0;
    // seen.open("seenDB.txt", std::fstream::in | std::fstream::out | std::fstream::app);
    url.open("urlDB.txt", std::fstream::in | std::fstream::out | std::fstream::app);
    /* read file to buf and then close file */
    /*while (std::getline(seen, buf[cnt])) {
        cnt++;
    }*/
}

test_url::~test_url()
{
    // seen.close();
    url.close();
    // free(buf);
}

bool test_url::check_seen (std::string check_md5)
{
    std::string tmp;
    cnt = 0;
    //std::ifstream seen_rw;
    seen_r.open("seenDB.txt", std::fstream::in);
    while (std::getline(seen_r, tmp)) {
        cnt++;
        if (tmp.compare(check_md5)==0) { // seen
            seen_r.close();
            return false;
        }
    }
    seen_r.close();
    // not seen
    seen_w.open("seenDB.txt", std::fstream::out | std::fstream::app);
    std::cout << check_md5 << std::endl;
    seen_w << check_md5 << std::endl;
    seen_w.close();
    /*for (int i=0; i < cnt; i++)
    {
        if(buf[i].compare(check_md5)==0) //seen
            return false;
    }*/
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
        //std::cout << href << std::endl << std::endl;
        tmp = href = href + strlen("href=\"");
        while (1)
        {
            if (*tmp == '\"' || *tmp == '\0')
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
        if (check_seen(md5tmp)) { // true -> not seen
            // seen << md5tmp << std::endl;
            // std::cout << md5tmp << std::endl;
            url << stmp << std::endl;
        }
        // seen
    }
}