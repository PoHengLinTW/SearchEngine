CC=g++
CFLAGS=`/usr/local/opt/curlpp/bin/curlpp-config --cflags` `/usr/local/opt/curlpp/bin/curlpp-config --libs` -std=c++11

all: 
	$(CC) $(CFLAGS) crawler.cpp -o crawler
n:
	#g++ `/usr/local/opt/curlpp/bin/curlpp-config --cflags` `/usr/local/opt/curlpp/bin/curlpp-config --libs` -std=c++11 example00.cpp -o example00
	#g++ `/usr/local/opt/curlpp/bin/curlpp-config --cflags` `/usr/local/opt/curlpp/bin/curlpp-config --libs` -std=c++11 test.cpp -o test