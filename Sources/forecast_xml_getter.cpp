#include "../Headers/forecast_xml_getter.h"

string ForecastXMLGetter::getXML(string operationName, string baseDate, string baseTime, string nx, string ny, string type) {
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	/* create url */
	string url = BASE_URL;
	url += "/";
	url += operationName;
	url += "?ServiceKey=";
	url += SERVICE_KEY;
	url += "&base_date=";
	url += baseDate;
	url += "&base_time=";
	url += baseTime;
	url += "&nx=";
	url += nx;
	url += "&ny=";
	url += ny;
	url += "&_type=";
	url += type;

	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */

	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl_handle = curl_easy_init();

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

	/* some servers don't like requests that are made without a user-agent
	field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* get it! */
	res = curl_easy_perform(curl_handle);

	/* check for errors */
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
	}
	else {
		/*
		* Now, our chunk.memory points to a memory block that is chunk.size
		* bytes big and contains the remote file.
		*
		* Do something nice with it!
		*/

		printf("%lu bytes retrieved\n", (long)chunk.size);
	}

	/* store it into buffer */
	buffer = chunk.memory;

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	free(chunk.memory);

	/* we're done with libcurl, so clean it up */
	curl_global_cleanup();

	return buffer;
}


