#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#define MAX_FILE_LENGTH 20000

size_t 
FileCallback(FILE *f, char* ptr, size_t size, size_t nmemb)
{
	return fwrite(ptr, size, nmemb, f);
};


int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		std::cerr << argv[0] << ": Wrong number of arguments" << std::endl 
			<< argv[0] << ": Usage: " << " url file" 
			<< std::endl;

		return EXIT_FAILURE;
	}

	char *url = argv[1];
	char *filename = argv[2];
	FILE * file = fopen(filename, "w");
	if (!file)
	{
		std::cerr << "Error opening " << filename << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		curlpp::Cleanup cleaner;
		curlpp::Easy request;

		// Set the writer callback to enable cURL to write result in a memory area
		curlpp::types::WriteFunctionFunctor functor(utilspp::BindFirst(utilspp::make_functor(&FileCallback), file));
		curlpp::options::WriteFunction *test = new curlpp::options::WriteFunction(functor);
		request.setOpt(test);

		// Setting the URL to retrive.
		request.setOpt(new curlpp::options::Url(url));
		request.setOpt(new curlpp::options::Verbose(true));
		request.perform();

		return EXIT_SUCCESS;
	}

	catch ( curlpp::LogicError & e )
	{
		std::cout << e.what() << std::endl;
	}

	catch ( curlpp::RuntimeError & e )
	{
		std::cout << e.what() << std::endl;
	}

	return EXIT_FAILURE;
}