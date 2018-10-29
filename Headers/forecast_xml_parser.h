#ifndef __FORECAST_XML_PARSER_H
#define __FORECAST_XML_PARSER_H

#include <iostream>
#include <vector>
#include "../Headers/tinyxml2.h"
#include "../Headers/local_time.h"
using namespace std;
using namespace tinyxml2;

const int MIN_NX = 21; const int MAX_NX = 144;
const int MIN_NY = 8;  const int MAX_NY = 147;

inline void HandleXMLError(XMLError errorCode)
{
/*
	cerr << "[XML error code: " << errorCode << "] ";
	switch (errorCode)
	{
	case XML_SUCCESS:							cerr << "XML_SUCCESS\n";						 break;
	case XML_NO_ATTRIBUTE:						cerr << "XML_NO_ATTRIBUTE\n";					 break;
	case XML_WRONG_ATTRIBUTE_TYPE:				cerr << "XML_WRONG_ATTRIBUTE_TYPE\n";			 break;
	case XML_ERROR_FILE_NOT_FOUND:				cerr << "XML_ERROR_FILE_NOT_FOUND\n";			 break;
	case XML_ERROR_FILE_COULD_NOT_BE_OPENED:	cerr << "XML_ERROR_FILE_COULD_NOT_BE_OPENED\n";	 break;
	case XML_ERROR_FILE_READ_ERROR:				cerr << "XML_ERROR_FILE_READ_ERROR\n";			 break;
	case UNUSED_XML_ERROR_ELEMENT_MISMATCH:		cerr << "UNUSED_XML_ERROR_ELEMENT_MISMATCH\n";	 break;
	case XML_ERROR_PARSING_ELEMENT:				cerr << "XML_ERROR_PARSING_ELEMENT\n";			 break;
	case XML_ERROR_PARSING_ATTRIBUTE:			cerr << "XML_ERROR_PARSING_ATTRIBUTE\n";		 break;
	case UNUSED_XML_ERROR_IDENTIFYING_TAG:		cerr << "UNUSED_XML_ERROR_IDENTIFYING_TAG\n";	 break;
	case XML_ERROR_PARSING_TEXT:				cerr << "XML_ERROR_PARSING_TEXT\n";				 break;
	case XML_ERROR_PARSING_CDATA:				cerr << "XML_ERROR_PARSING_CDATA\n";			 break;
	case XML_ERROR_PARSING_COMMENT:				cerr << "XML_ERROR_PARSING_COMMENT\n";			 break;
	case XML_ERROR_PARSING_DECLARATION:			cerr << "XML_ERROR_PARSING_DECLARATION\n";		 break;
	case XML_ERROR_PARSING_UNKNOWN:				cerr << "XML_ERROR_PARSING_UNKNOWN\n";			 break;
	case XML_ERROR_EMPTY_DOCUMENT:				cerr << "XML_ERROR_EMPTY_DOCUMENT\n";			 break;
	case XML_ERROR_MISMATCHED_ELEMENT:			cerr << "XML_ERROR_MISMATCHED_ELEMENT\n";		 break;
	case XML_ERROR_PARSING:						cerr << "XML_ERROR_PARSING\n";					 break;
	case XML_CAN_NOT_CONVERT_TEXT:				cerr << "XML_CAN_NOT_CONVERT_TEXT\n";			 break;
	case XML_NO_TEXT_NODE:						cerr << "XML_NO_TEXT_NODE\n";					 break;
	case XML_ERROR_COUNT:						cerr << "XML_ERROR_COUNT\n";					 break;
	default:									cerr << "UNEXPECTED ERROR\n";					 break;
	}*/
}

//------------------------------------------------------------------------------

class ForecastXML {
private:
	string resultCode;
	string resultMsg;
	string numOfRows;
	string pageNo;
	string totalCount;

public:
	void setResultCode(string resultCode);
	void setResultMsg(string resultMsg);
	void setNumOfRows(string numOfRows);
	void setPageNo(string pageNo);
	void setTotalCount(string totalCount);
	string getResultCode() const;
	string getResultMsg() const;
	string getNumOfRows() const;
	string getPageNo() const;
	string getTotalCount() const;
};

//------------------------------------------------------------------------------

class ForecastGribItem {
private:
	string baseDate;
	string baseTime;
	string category;
	string nx;
	string ny;
	string obsrValue;
public:
	ForecastGribItem(string baseDate, string baseTime, string category, string nx, string ny, string obsrValue);
	string getBaseDate() const;
	string getBaseTime() const;
	string getCategory() const;
	string getNx() const;
	string getNy() const;
	string getObsrValue() const;
};



class ForecastGribXML : public ForecastXML {
private:	
	vector<ForecastGribItem> items;
public:
	void addItem(ForecastGribItem item);
	ForecastGribItem* getItem(size_t index);
	unsigned int getSizeOfItems() const;
};

//------------------------------------------------------------------------------

class ForecastTimeDataItem {
private:
	string baseDate;
	string baseTime;
	string category;
	string fcstDate;
	string fcstTime;
	string fcstValue;
	string nx;
	string ny;
public:
	ForecastTimeDataItem(string baseDate, string baseTime, string category, string fcstDate, string fcstTime, string fcstValue, string nx, string ny);
	string getBaseDate() const;
	string getBaseTime() const;
	string getCategory() const;
	string getFcstDate() const;
	string getFcstTime() const;
	string getFcstValue() const;
	string getNx() const;
	string getNy() const;
};



class ForecastTimeDataXML : public ForecastXML {
private:
	vector<ForecastTimeDataItem> items;
public:
	void addItem(ForecastTimeDataItem item);
	ForecastTimeDataItem* getItem(size_t index);
	unsigned int getSizeOfItems() const;
};

//------------------------------------------------------------------------------

class ForecastSpaceDataItem {
private:
	string baseDate;
	string baseTime;
	string category;
	string fcstDate;
	string fcstTime;
	string fcstValue;
	string nx;
	string ny;
public:
	ForecastSpaceDataItem(string baseDate, string baseTime, string category, string fcstDate, string fcstTime, string fcstValue, string nx, string ny);
	string getBaseDate() const;
	string getBaseTime() const;
	string getCategory() const;
	string getFcstDate() const;
	string getFcstTime() const;
	string getFcstValue() const;
	string getNx() const;
	string getNy() const;
};



class ForecastSpaceDataXML : public ForecastXML {
private:
	vector<ForecastSpaceDataItem> items;
public:
	void addItem(ForecastSpaceDataItem item);
	ForecastSpaceDataItem* getItem(size_t index);
	unsigned int getSizeOfItems() const;
};

//------------------------------------------------------------------------------

class ForecastXMLParser {
public:
	bool parse(string content);
private:
	ForecastGribXML* makeForecastGribXML(string content);
	ForecastTimeDataXML* makeForecastTimeDataXML(string content);
	ForecastSpaceDataXML* makeForecastSpaceDataXML(string content);
	bool isTrustworthy(ForecastGribXML* xml);
	bool isTrustworthy(ForecastTimeDataXML* xml);
	bool isTrustworthy(ForecastSpaceDataXML* xml);
};

#endif


