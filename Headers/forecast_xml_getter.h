#ifndef __FORECAST_XML_GETTER_H
#define __FORECAST_XML_GETTER_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include "curl/curl.h"

using namespace std;

const string BASE_URL = "http://newsky2.kma.go.kr/service/SecndSrtpdFrcstInfoService2";
const string SERVICE_KEY = "cTwz1vK6M%2F2H2grK8%2F8PF%2F1eKcvhWDiUSmXEm%2FgVaBUA3Nzzz5Thqd1sabmjshLk1iOV9zBB3EYjkloax4tnFg%3D%3D";

struct MemoryStruct {
	char *memory;
	size_t size;
};

class ForecastXMLGetter {
private:
	string buffer;

public:
	// referred to: https://curl.haxx.se/libcurl/c/getinmemory.html
	string getXML(string operationName, string baseDate, string baseTime, string nx, string ny, string type);

private:
	static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		size_t realsize = size * nmemb;
		struct MemoryStruct *mem = (struct MemoryStruct *)userp;

		mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
		if (mem->memory == NULL) {
			/* out of memory! */
			printf("not enough memory (realloc returned NULL)\n");
			return 0;
		}

		memcpy(&(mem->memory[mem->size]), contents, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;

		return realsize;
	}
};

#endif


