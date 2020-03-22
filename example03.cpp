/**
* \file
* DebugFunction option using functor as a callback.
*/


#include <cstdlib>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

class MyWindow
{
public:
	int writeDebug(curl_infotype, char *data, size_t size)
	{
		fprintf(stderr, "Debug: ");
		fwrite(data, size, 1, stderr);
		return size;
	}
};

int main(int argc, char *argv[])
{

	if(argc != 2)
	{
		std::cerr	<< "Example 3: Wrong number of arguments" << std::endl 
							<< "Example 3: Usage: example3 url" 
							<< std::endl;
		return EXIT_FAILURE;
	}
	char *url = argv[1];

	MyWindow myWindow;

	try 
	{
		curlpp::Cleanup cleaner;
		curlpp::Easy request;

		using namespace curlpp::Options;
		request.setOpt(Verbose(true));
		request.setOpt(DebugFunction(curlpp::types::DebugFunctionFunctor(&myWindow, &MyWindow::writeDebug)));
		request.setOpt(Url(url));

		request.perform();
	}

	catch ( curlpp::LogicError & e ) 
	{
		std::cout << e.what() << std::endl;
	}

	catch ( curlpp::RuntimeError & e )
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}