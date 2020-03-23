/**
* \file
* Uploading of string.
* ReadFunction option using functor.
* Setting custom headers.
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
 
/*
   anonymous namespace to prevent name clash in case other examples using the same global entities
   would be compiled in the same project
*/
namespace
{

char *data = NULL;

size_t readData(char *buffer, size_t size, size_t nitems)
{
	strncpy(buffer, data, size * nitems);
	return size * nitems;
}

} // namespace

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		std::cerr << "Example 2: Missing argument" << std::endl 
							<< "Example 2: Usage: example02 url string-to-send" 
							<< std::endl;
		return EXIT_FAILURE;
	}

	char *url = argv[1];
	data = argv[2];
	int size = strlen(data);

	char buf[50];
	try
	{
		curlpp::Cleanup cleaner;
		curlpp::Easy request;

		std::list<std::string> headers;
		headers.push_back("Content-Type: text/*"); 
		sprintf(buf, "Content-Length: %d", size); 
		headers.push_back(buf);

		using namespace curlpp::Options;
		request.setOpt(new Verbose(true));
		request.setOpt(new ReadFunction(curlpp::types::ReadFunctionFunctor(readData)));
		request.setOpt(new InfileSize(size));
		request.setOpt(new Upload(true));
		request.setOpt(new HttpHeader(headers));
		request.setOpt(new Url(url));

		request.perform();
	}
	catch (curlpp::LogicError & e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (curlpp::RuntimeError & e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
