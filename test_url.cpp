#include <iostream>
#include <fstream>
#include <cstring>

#include "test_url.h"
#include "md5.h"

#define max_seen_cnt 1000

test_url::test_url(/* args */)
{
    buf = new std::string[max_seen_cnt];
    cnt = 0;
    seen.open("seenDB.txt", std::fstream::in);
    url.open("urlDB.txt", std::fstream::out | std::fstream::app);
    /* read file to buf and then close file */
    if (seen) {
        while (std::getline(seen, buf[cnt])) {
            if (cnt == max_seen_cnt-1) {
                std::cerr << "cnt reach max_seen_cnt in reading file" << std::endl;
                break;
            }
            cnt++;
        }
    }
    seen.close();
}

test_url::~test_url()
{
    seen.open("seenDB.txt", std::fstream::out);
    for (int i=0; i<=cnt; i++) 
    {
        seen << buf[i] << std::endl;
    }
    seen.close();
    url.close();
    // delete buf;
}

bool test_url::check_seen (std::string check_md5)
{
    //std::ifstream seen_rw;
    for (int i=0; i<=cnt; i++)
    {
        if (buf[i].compare(check_md5)==0) //seen
            return false;
    }

    // not seen
    if (cnt >= max_seen_cnt) { // exceed the limit
        std::cerr << "cnt reach max_seen_cnt in adding md5 to buffer" << std::endl;
        return false;
    }
    else { // if buf[0] & buf[1] have data, cnt is 2 => new data should store in buf[2] & cnt will be 3
        buf[cnt++] = check_md5; 
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
            url << stmp << std::endl;
        }
        // seen
    }
}