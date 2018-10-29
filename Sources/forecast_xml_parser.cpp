
#include "../Headers/forecast_xml_parser.h"

//-------------------------------------------------------------------------------------------------------

void ForecastXML::setResultCode(string resultCode)	{ this->resultCode = resultCode; }
void ForecastXML::setResultMsg(string resultMsg)	{ this->resultMsg = resultMsg; }
void ForecastXML::setNumOfRows(string numOfRows)	{ this->numOfRows = numOfRows; }
void ForecastXML::setPageNo(string pageNo)			{ this->pageNo = pageNo; }
void ForecastXML::setTotalCount(string totalCount)	{ this->totalCount = totalCount; }
string ForecastXML::getResultCode() const			{ return resultCode; }
string ForecastXML::getResultMsg() const			{ return resultMsg; }
string ForecastXML::getNumOfRows() const			{ return numOfRows; }
string ForecastXML::getPageNo() const				{ return pageNo; }
string ForecastXML::getTotalCount() const			{ return totalCount; }



//-------------------------------------------------------------------------------------------------------

ForecastGribItem::ForecastGribItem(string baseDate, string baseTime, string category, string nx, string ny, string obsrValue)
	: baseDate(baseDate), baseTime(baseTime), category(category), nx(nx), ny(ny), obsrValue(obsrValue) {}


string ForecastGribItem::getBaseDate() const	{ return baseDate; }
string ForecastGribItem::getBaseTime() const	{ return baseTime; }
string ForecastGribItem::getCategory() const	{ return category; }
string ForecastGribItem::getNx() const			{ return nx; }
string ForecastGribItem::getNy() const			{ return ny; }
string ForecastGribItem::getObsrValue() const	{ return obsrValue; }


void ForecastGribXML::addItem(ForecastGribItem item)
{
	items.push_back(item);
}


ForecastGribItem* ForecastGribXML::getItem(size_t index)
{
	try
	{
		if (this->getSizeOfItems() == 0) {
			throw "no items";
		}
		if (index >= items.size()) {
			throw "out of range error";
		}
		return &items[index];
	}
	catch (const char* message)
	{
		cout << "ForecastGribXML::getItem(): " << message << '\n';
		return nullptr;
	}
}


unsigned int ForecastGribXML::getSizeOfItems() const
{
	return items.size();
}



//-------------------------------------------------------------------------------------------------------

ForecastTimeDataItem::ForecastTimeDataItem(string baseDate, string baseTime, string category, string fcstDate, string fcstTime, string fcstValue, string nx, string ny)
	: baseDate(baseDate), baseTime(baseTime), category(category), fcstDate(fcstDate), fcstTime(fcstTime), fcstValue(fcstValue), nx(nx), ny(ny) {}

string ForecastTimeDataItem::getBaseDate() const   { return baseDate;}
string ForecastTimeDataItem::getBaseTime() const   { return baseTime;}
string ForecastTimeDataItem::getCategory() const   { return category;}
string ForecastTimeDataItem::getFcstDate() const   { return fcstDate;}
string ForecastTimeDataItem::getFcstTime() const   { return fcstTime;}
string ForecastTimeDataItem::getFcstValue() const  { return fcstValue;}
string ForecastTimeDataItem::getNx() const		   { return nx;}
string ForecastTimeDataItem::getNy() const		   { return ny;}

void ForecastTimeDataXML::addItem(ForecastTimeDataItem item) 
{
	items.push_back(item);
}


ForecastTimeDataItem* ForecastTimeDataXML::getItem(size_t index)
{
	try
	{
		if (this->getSizeOfItems() == 0) {
			throw "no items";
		}
		if (index >= items.size()) {
			throw "out of range error";
		}
		return &items[index];
	}
	catch (const char* message)
	{
		cout << "ForecastTimeDataXML::getItem(): " << message << '\n';
		return nullptr;
	}
}


unsigned int ForecastTimeDataXML::getSizeOfItems() const
{
	return items.size();
}



//-------------------------------------------------------------------------------------------------------

ForecastSpaceDataItem::ForecastSpaceDataItem(string baseDate, string baseTime, string category, string fcstDate, string fcstTime, string fcstValue, string nx, string ny)
	: baseDate(baseDate), baseTime(baseTime), category(category), fcstDate(fcstDate), fcstTime(fcstTime), fcstValue(fcstValue), nx(nx), ny(ny) {}


string ForecastSpaceDataItem::getBaseDate() const   { return baseDate;}
string ForecastSpaceDataItem::getBaseTime() const   { return baseTime;}
string ForecastSpaceDataItem::getCategory() const   { return category;}
string ForecastSpaceDataItem::getFcstDate() const   { return fcstDate;}
string ForecastSpaceDataItem::getFcstTime() const   { return fcstTime;}
string ForecastSpaceDataItem::getFcstValue() const  { return fcstValue;}
string ForecastSpaceDataItem::getNx() const		   { return nx;}
string ForecastSpaceDataItem::getNy() const		   { return ny;}


void ForecastSpaceDataXML::addItem(ForecastSpaceDataItem item)
{
	items.push_back(item);
}


ForecastSpaceDataItem* ForecastSpaceDataXML::getItem(size_t index)
{
	try
	{
		if (this->getSizeOfItems() == 0) {
			throw "no items";
		}
		if (index >= items.size()) {
			throw "out of range error";
		}
		return &items[index];
	}
	catch (const char* message)
	{
		cout << "ForecastSpaceDataXML::getItem(): " << message << '\n';
		return nullptr;
	}
}


unsigned int ForecastSpaceDataXML::getSizeOfItems() const
{
	return items.size();
}



//-------------------------------------------------------------------------------------------------------

bool ForecastXMLParser::parse(string content)
{
	return (
		isTrustworthy(makeForecastGribXML(content)) ||
		isTrustworthy(makeForecastTimeDataXML(content)) ||
		isTrustworthy(makeForecastSpaceDataXML(content))
	);
}


ForecastGribXML* ForecastXMLParser::makeForecastGribXML(string content)
{
	tinyxml2::XMLDocument xmlDoc;
	XMLError eResult;
	XMLNode* pRoot;
	XMLElement* pElement;
	XMLElement* ppElement;
	XMLElement* pppElement;
	XMLElement* ppppElement;
	const char* strText;

	ForecastGribXML xml;

	eResult = xmlDoc.Parse(content.c_str(), content.size());
	if (eResult != XML_SUCCESS) {
		HandleXMLError(eResult);
		return nullptr;
	}

	/* <?xml ... ?> */
	pRoot = xmlDoc.FirstChild();
	if (pRoot == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_DECLARATION);
		return nullptr;
	}

	/* <response> ... </response> */
	pRoot = pRoot->NextSibling();
	if (pRoot == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/** <header> ... </header> **/
	pElement = pRoot->FirstChildElement("header");
	if (pElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/*** <resultCode> ... </resultCode> ***/
	ppElement = pElement->FirstChildElement("resultCode");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setResultCode(strText);

	/*** <resultMsg> ... </resultMsg> ***/
	ppElement = pElement->FirstChildElement("resultMsg");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setResultMsg(strText);

	/** <body> ... </body> **/
	pElement = pRoot->FirstChildElement("body");
	if (pElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/*** <items> ... </items> ***/
	ppElement = pElement->FirstChildElement("items");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/**** <item> ... </item> ****/
	pppElement = ppElement->FirstChildElement("item");
	if (pppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	do
	{
		// baseDate
		ppppElement = pppElement->FirstChildElement("baseDate");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string baseDate = strText;

		// baseTime
		ppppElement = pppElement->FirstChildElement("baseTime");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string baseTime = strText;

		// category
		ppppElement = pppElement->FirstChildElement("category");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string category = strText;

		// nx
		ppppElement = pppElement->FirstChildElement("nx");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string nx = strText;

		// ny
		ppppElement = pppElement->FirstChildElement("ny");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string ny = strText;

		// obsrValue
		ppppElement = pppElement->FirstChildElement("obsrValue");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string obsrValue = strText;


		xml.addItem(ForecastGribItem{ baseDate, baseTime, category, nx, ny, obsrValue });
		pppElement = pppElement->NextSiblingElement();
	} while (pppElement != nullptr);

	/*** <numOfRows> ... </numOfRows> ***/
	ppElement = pElement->FirstChildElement("numOfRows");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setNumOfRows(strText);

	/*** <pageNo> ... </pageNo> ***/
	ppElement = pElement->FirstChildElement("pageNo");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setPageNo(strText);

	/*** <totalCount> ... </totalCount> ***/
	ppElement = pElement->FirstChildElement("totalCount");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setTotalCount(strText);

	// parsing completed
	return new ForecastGribXML(xml);
}


ForecastTimeDataXML* ForecastXMLParser::makeForecastTimeDataXML(string content)
{
	tinyxml2::XMLDocument xmlDoc;
	XMLError eResult;
	XMLNode* pRoot;
	XMLElement* pElement;
	XMLElement* ppElement;
	XMLElement* pppElement;
	XMLElement* ppppElement;
	const char* strText;

	ForecastTimeDataXML xml;

	eResult = xmlDoc.Parse(content.c_str(), content.size());
	if (eResult != XML_SUCCESS) {
		HandleXMLError(eResult);
		return nullptr;
	}

	/* <?xml ... ?> */
	pRoot = xmlDoc.FirstChild();
	if (pRoot == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_DECLARATION);
		return nullptr;
	}

	/* <response> ... </response> */
	pRoot = pRoot->NextSibling();
	if (pRoot == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/** <header> ... </header> **/
	pElement = pRoot->FirstChildElement("header");
	if (pElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/*** <resultCode> ... </resultCode> ***/
	ppElement = pElement->FirstChildElement("resultCode");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setResultCode(strText);

	/*** <resultMsg> ... </resultMsg> ***/
	ppElement = pElement->FirstChildElement("resultMsg");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setResultMsg(strText);

	/** <body> ... </body> **/
	pElement = pRoot->FirstChildElement("body");
	if (pElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/*** <items> ... </items> ***/
	ppElement = pElement->FirstChildElement("items");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/**** <item> ... </item> ****/
	pppElement = ppElement->FirstChildElement("item");
	if (pppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	do
	{
		// baseDate
		ppppElement = pppElement->FirstChildElement("baseDate");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string baseDate = strText;

		// baseTime
		ppppElement = pppElement->FirstChildElement("baseTime");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string baseTime = strText;

		// category
		ppppElement = pppElement->FirstChildElement("category");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string category = strText;

		// fcstDate
		ppppElement = pppElement->FirstChildElement("fcstDate");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string fcstDate = strText;

		// fcstTime
		ppppElement = pppElement->FirstChildElement("fcstTime");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string fcstTime = strText;

		// fcstValue
		ppppElement = pppElement->FirstChildElement("fcstValue");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string fcstValue = strText;

		// nx
		ppppElement = pppElement->FirstChildElement("nx");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string nx = strText;

		// ny
		ppppElement = pppElement->FirstChildElement("ny");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string ny = strText;


		xml.addItem(ForecastTimeDataItem{ baseDate, baseTime, category, fcstDate, fcstTime, fcstValue, nx, ny });
		pppElement = pppElement->NextSiblingElement();
	} while (pppElement != nullptr);

	/*** <numOfRows> ... </numOfRows> ***/
	ppElement = pElement->FirstChildElement("numOfRows");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setNumOfRows(strText);

	/*** <pageNo> ... </pageNo> ***/
	ppElement = pElement->FirstChildElement("pageNo");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setPageNo(strText);

	/*** <totalCount> ... </totalCount> ***/
	ppElement = pElement->FirstChildElement("totalCount");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setTotalCount(strText);

	// parsing completed
	return new ForecastTimeDataXML(xml);
}


ForecastSpaceDataXML* ForecastXMLParser::makeForecastSpaceDataXML(string content)
{
	tinyxml2::XMLDocument xmlDoc;
	XMLError eResult;
	XMLNode* pRoot;
	XMLElement* pElement;
	XMLElement* ppElement;
	XMLElement* pppElement;
	XMLElement* ppppElement;
	const char* strText;

	ForecastSpaceDataXML xml;

	eResult = xmlDoc.Parse(content.c_str(), content.size());
	if (eResult != XML_SUCCESS) {
		HandleXMLError(eResult);
		return nullptr;
	}

	/* <?xml ... ?> */
	pRoot = xmlDoc.FirstChild();
	if (pRoot == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_DECLARATION);
		return nullptr;
	}

	/* <response> ... </response> */
	pRoot = pRoot->NextSibling();
	if (pRoot == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/** <header> ... </header> **/
	pElement = pRoot->FirstChildElement("header");
	if (pElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/*** <resultCode> ... </resultCode> ***/
	ppElement = pElement->FirstChildElement("resultCode");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setResultCode(strText);

	/*** <resultMsg> ... </resultMsg> ***/
	ppElement = pElement->FirstChildElement("resultMsg");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setResultMsg(strText);

	/** <body> ... </body> **/
	pElement = pRoot->FirstChildElement("body");
	if (pElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/*** <items> ... </items> ***/
	ppElement = pElement->FirstChildElement("items");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	/**** <item> ... </item> ****/
	pppElement = ppElement->FirstChildElement("item");
	if (pppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}

	do
	{
		// baseDate
		ppppElement = pppElement->FirstChildElement("baseDate");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string baseDate = strText;

		// baseTime
		ppppElement = pppElement->FirstChildElement("baseTime");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string baseTime = strText;

		// category
		ppppElement = pppElement->FirstChildElement("category");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string category = strText;

		// fcstDate
		ppppElement = pppElement->FirstChildElement("fcstDate");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string fcstDate = strText;

		// fcstTime
		ppppElement = pppElement->FirstChildElement("fcstTime");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string fcstTime = strText;

		// fcstValue
		ppppElement = pppElement->FirstChildElement("fcstValue");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string fcstValue = strText;

		// nx
		ppppElement = pppElement->FirstChildElement("nx");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string nx = strText;

		// ny
		ppppElement = pppElement->FirstChildElement("ny");
		if (ppppElement == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_ELEMENT);
			return nullptr;
		}
		strText = ppppElement->GetText();
		if (strText == nullptr) {
			HandleXMLError(XML_ERROR_PARSING_TEXT);
			return nullptr;
		}
		string ny = strText;


		xml.addItem(ForecastSpaceDataItem{ baseDate, baseTime, category, fcstDate, fcstTime, fcstValue, nx, ny });
		pppElement = pppElement->NextSiblingElement();
	} while (pppElement != nullptr);

	/*** <numOfRows> ... </numOfRows> ***/
	ppElement = pElement->FirstChildElement("numOfRows");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setNumOfRows(strText);

	/*** <pageNo> ... </pageNo> ***/
	ppElement = pElement->FirstChildElement("pageNo");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setPageNo(strText);

	/*** <totalCount> ... </totalCount> ***/
	ppElement = pElement->FirstChildElement("totalCount");
	if (ppElement == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_ELEMENT);
		return nullptr;
	}
	strText = ppElement->GetText();
	if (strText == nullptr) {
		HandleXMLError(XML_ERROR_PARSING_TEXT);
		return nullptr;
	}
	xml.setTotalCount(strText);

	// parsing completed
	return new ForecastSpaceDataXML(xml);
}

bool ForecastXMLParser::isTrustworthy(ForecastGribXML* xml)
{
	if (xml == nullptr)
		return false;

	if (xml->getResultCode() != "0000")
		return false;
	if (xml->getResultMsg() != "OK")
		return false;

	for (size_t i = 0; i < xml->getSizeOfItems(); ++i) {
		LocalTime lt;
		if (lt.getBaseDate() != xml->getItem(i)->getBaseDate())
			return false;

		if (xml->getItem(i)->getCategory() == "T1H" ||	// 기온
			xml->getItem(i)->getCategory() == "RN1" ||	// 1시간 강수량
			xml->getItem(i)->getCategory() == "SKY" ||	// 하늘상태
			xml->getItem(i)->getCategory() == "UUU" ||	// 동서바람성분
			xml->getItem(i)->getCategory() == "VVV" ||	// 남북바람성분
			xml->getItem(i)->getCategory() == "REH" ||	// 습도
			xml->getItem(i)->getCategory() == "PTY" ||	// 강수형태
			xml->getItem(i)->getCategory() == "LGT" ||	// 낙뢰
			xml->getItem(i)->getCategory() == "VEC" ||	// 풍향
			xml->getItem(i)->getCategory() == "WSD"); 	// 풍속
		else
			return false;

		int nx = atoi(xml->getItem(i)->getNx().c_str());
		int ny = atoi(xml->getItem(i)->getNy().c_str());

		if (nx < MIN_NX || nx > MAX_NX)
			return false;
		if (ny < MIN_NX || ny > MAX_NY)
			return false;
	}

	return true;
}

bool ForecastXMLParser::isTrustworthy(ForecastTimeDataXML* xml)
{
	if (xml == nullptr)
		return false;

	if (xml->getResultCode() != "0000")
		return false;
	if (xml->getResultMsg() != "OK")
		return false;

	for (size_t i = 0; i < xml->getSizeOfItems(); ++i) {
		LocalTime lt;
		if (lt.getBaseDate() != xml->getItem(i)->getBaseDate())
			return false;

		if (xml->getItem(i)->getCategory() == "T1H" ||	// 기온
			xml->getItem(i)->getCategory() == "RN1" ||	// 1시간 강수량
			xml->getItem(i)->getCategory() == "SKY" ||	// 하늘상태
			xml->getItem(i)->getCategory() == "UUU" ||	// 동서바람성분
			xml->getItem(i)->getCategory() == "VVV" ||	// 남북바람성분
			xml->getItem(i)->getCategory() == "REH" ||	// 습도
			xml->getItem(i)->getCategory() == "PTY" ||	// 강수형태
			xml->getItem(i)->getCategory() == "LGT" ||	// 낙뢰
			xml->getItem(i)->getCategory() == "VEC" ||	// 풍향
			xml->getItem(i)->getCategory() == "WSD");	// 풍속
		else
			return false;

		int nx = atoi(xml->getItem(i)->getNx().c_str());
		int ny = atoi(xml->getItem(i)->getNy().c_str());

		if (nx < MIN_NX || nx > MAX_NX)
			return false;
		if (ny < MIN_NX || ny > MAX_NY)
			return false;
	}

	return true;
}

bool ForecastXMLParser::isTrustworthy(ForecastSpaceDataXML* xml)
{
	if (xml == nullptr)
		return false;

	if (xml->getResultCode() != "0000")
		return false;
	if (xml->getResultMsg() != "OK")
		return false;

	for (size_t i = 0; i < xml->getSizeOfItems(); ++i) {
		LocalTime lt;
		if (lt.getBaseDate() != xml->getItem(i)->getBaseDate())
			return false;

		if (xml->getItem(i)->getCategory() == "POP" ||	// 강수확률
			xml->getItem(i)->getCategory() == "PTY" ||	// 강수형태
			xml->getItem(i)->getCategory() == "R06" ||	// 6시간 강수량
			xml->getItem(i)->getCategory() == "REH" ||	// 습도
			xml->getItem(i)->getCategory() == "S06" ||	// 6시간 신적설
			xml->getItem(i)->getCategory() == "SKY" ||	// 하늘상태
			xml->getItem(i)->getCategory() == "T3H" ||	// 3시간 기온
			xml->getItem(i)->getCategory() == "TMN" ||	// 아침 최저기온
			xml->getItem(i)->getCategory() == "TMX" ||	// 낮 최고기온
			xml->getItem(i)->getCategory() == "UUU" ||	// 풍속(동서성분)
			xml->getItem(i)->getCategory() == "VVV" ||	// 풍속(남북성분)
			xml->getItem(i)->getCategory() == "WAV" ||	// 파고
			xml->getItem(i)->getCategory() == "VEC" ||	// 풍향
			xml->getItem(i)->getCategory() == "WSD");	// 풍속
		else
			return false;

		int nx = atoi(xml->getItem(i)->getNx().c_str());
		int ny = atoi(xml->getItem(i)->getNy().c_str());

		if (nx < MIN_NX || nx > MAX_NX)
			return false;
		if (ny < MIN_NX || ny > MAX_NY)
			return false;
	}

	return true;
}



