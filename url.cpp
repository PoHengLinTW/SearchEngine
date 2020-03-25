#include <iostream>
#include <fstream>
#include <cstring>

void retrieveUrl(char *m_pBuffer)
{
  char *href = m_pBuffer, url_link[256], *tmp;
  std::ofstream url_file;
  url_file.open("url.txt");
  while(1)
  {
    int len = 0;
    if ((href=strstr(href, "href=\"http"))==NULL)
      break;
    //std::cout << href << std::endl << std::endl;
    tmp = href = href+strlen("href=\"");
    while(1)
    {
      if (*tmp=='\"' || *tmp=='\0') {
        *tmp = '\0';
        break;
      }
      tmp++;
      len++;
    }
    memcpy(url_link, href, len+1);
    //strcpy(url_link, href);
    *tmp = ' ';
    //std::cout << url_link << std::endl;
    url_file << url_link << std::endl;
  }
  url_file.close();
}