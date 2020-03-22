/**
* \file
* Write function using free function as a callback.
* 
*/


#include <cstdlib>
#include <cstring>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#define MAX_FILE_LENGTH 20000

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
size_t WriteMemoryCallback(char* ptr, size_t size, size_t nmemb)
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


void print() 
{
	std::cout << "Size: " << m_Size << std::endl;
	std::cout << "Content: " << std::endl << m_pBuffer << std::endl;
}


int main(int argc, char *argv[])
{
	m_pBuffer = (char*) malloc(MAX_FILE_LENGTH * sizeof(char));

	if(argc != 2)
	{
		std::cerr << "Example 05: Wrong number of arguments" << std::endl 
			<< "Example 05: Usage: example05 url" 
			<< std::endl;
		return EXIT_FAILURE;
	}
	char *url = argv[1];

	try
	{
		curlpp::Cleanup cleaner;
		curlpp::Easy request;

		// Set the writer callback to enable cURL 
		// to write result in a memory area
		curlpp::types::WriteFunctionFunctor functor(WriteMemoryCallback);
		curlpp::options::WriteFunction *test = new curlpp::options::WriteFunction(functor);
		request.setOpt(test);

		// Setting the URL to retrive.
		request.setOpt(new curlpp::options::Url(url));
		request.setOpt(new curlpp::options::Verbose(true));
		request.perform();

		print();
	}
	catch ( curlpp::LogicError & e )
	{
		std::cout << e.what() << std::endl;
	}
	catch ( curlpp::RuntimeError & e )
	{
		std::cout << e.what() << std::endl;
	}
}