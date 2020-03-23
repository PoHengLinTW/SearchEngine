#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#define MAX_FILE_LENGTH 50000

std::ofstream output_file;
char *m_pBuffer = NULL;
size_t m_Size = 0;

void* Realloc(void* ptr, size_t size)
{
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
};

// Callback must be declared static, otherwise it won't link...
static size_t WriteMemoryCallback(char* ptr, size_t size, size_t nmemb)
{
  // Calculate the real size of the incoming buffer
  size_t realsize = size * nmemb;  
  // (Re)Allocate memory for the buffer
  m_pBuffer = (char*) Realloc(m_pBuffer, m_Size + realsize);
  // Test if Buffer is initialized correctly & copy memory
  if (m_pBuffer == NULL) {
    realsize = 0;
  }
  memcpy(&(m_pBuffer[m_Size]), ptr, realsize);
  m_Size += realsize;
  // return the real size of the buffer...
  return realsize;
};

void write()
{
    output_file << "Size : " << m_Size << std::endl;
    output_file << "Content : " << std::endl << m_pBuffer << std::endl;
}

/* remove from <tag> to </tag>*/
void rmTag(char *tag)
{
  /* replace newline with  space */
  for (int i=0; i<strlen(m_pBuffer); i++)
    if(m_pBuffer[i]=='\n')
      m_pBuffer[i] = ' ';
  
  char front_tag[20], back_tag[20];
  sprintf(front_tag, "<%s", tag);   // create <tag for strstr
  sprintf(back_tag, "</%s>", tag);  // create </tag> for strstr
  char *front_ptr = m_pBuffer, *back_ptr=NULL;
  int time=0;
  while((front_ptr = strstr(front_ptr, front_tag))!=NULL)
  {
    std::cout << time++ << std::endl;
    char *tmp = front_ptr;
    front_ptr++;
    *tmp = '\0';
    if ((back_ptr = strstr(front_ptr, back_tag))==NULL) {
      //error
      std::cout << "error" << std::endl;
      break;
    }
    std::cout << time++ << std::endl;
    strcat(tmp, back_ptr+strlen(back_tag));
    front_ptr--;
  }
  // std::cout << m_pBuffer << std::endl;
}

void retrieveUrl()
{
  char *href = m_pBuffer, url_link[128], *tmp;
  std::ofstream url_file;
  url_file.open("url.txt");
  while((href=strstr(href, "href=\"http"))!=NULL)
  {
    tmp = href = href+strlen("href=\"");
    while(1)
    {
      if (*tmp=='\"') {
        *tmp = '\0';
        break;
      }
      tmp++;
    }
    strcpy(url_link, href);
    *tmp = ' ';
    //std::cout << url_link << std::endl;
    url_file << url_link << std::endl;
  }
  url_file.close();
}

int main(int, char **) /* I/O for save data, using dataa batch to control I/O count */
{
    m_pBuffer = (char*) malloc(MAX_FILE_LENGTH * sizeof(char));
    //output_file.open("test.txt");
    try
    {
        // initial process
        curlpp::Cleanup myCleanup;
        curlpp::Easy myRequest;

        // create curl options
        curlpp::options::Url myUrl(std::string("https://www.google.com"));
        curlpp::types::WriteFunctionFunctor functor(WriteMemoryCallback);
        curlpp::options::WriteFunction *write_func = new curlpp::options::WriteFunction(functor);

        // bind options
        myRequest.setOpt(write_func);
        myRequest.setOpt(myUrl);
        // myRequest.setOpt(new curlpp::options::Verbose(true));

        // perform
        myRequest.perform();
    }
    catch ( curlpp::LogicError & e )
	{
		std::cout << e.what() << std::endl;
	}
	catch ( curlpp::RuntimeError & e )
	{
		std::cout << e.what() << std::endl;
	}
    std::cout << "rm script" << std::endl;
    rmTag("script");
    std::cout << "rm style" << std::endl;
    rmTag("style");
    std::cout << "retrieve URL" << std::endl;
    retrieveUrl();
    std::cout << "write()" << std::endl;
    //write();
    //output_file.close();
    free(m_pBuffer);
    return 0;
}