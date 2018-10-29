#include "../Headers/config_info.h"
#include "../Headers/local_time.h"
#include "../Headers/forecast_xml_getter.h"
#include "../Headers/forecast_xml_parser.h"
#include "../Headers/SBS.h"
#include "../Headers/crypto.h"
#include "../Headers/CSBS_Packet.h"

const vector<string> FNAMES = { "ForecastGrib", "ForecastTimeData", "ForecastSpaceData" };
const string FORMAT = "xml";

string getCWD()
{
	char buff[1024];
	getcwd(buff, 1024);
	string path = buff;
	return path;
}

const string PROGRAM_PATH = getCWD() + "/";
const string CONFIG_PATH = PROGRAM_PATH + "Configs/forecast.cfg";
const string XML_PATH = PROGRAM_PATH + "XML/";

int main() try
{
	LocalTime* lt = new LocalTime;
	ConfigInfo* ci = new ConfigInfo(CONFIG_PATH);
	ForecastXMLGetter* getter = new ForecastXMLGetter;
	ForecastXMLParser* parser = new ForecastXMLParser;

	vector<string> contents;

	for (size_t i = 0; i < FNAMES.size(); ++i) {
		contents.push_back(getter->getXML(FNAMES[i], lt->getBaseDate(), lt->getBaseTime(), ci->getNx(), ci->getNy(), FORMAT));
		
		if (parser->parse(contents[i])) {
			string path = XML_PATH + FNAMES[i] + "." + FORMAT;
			encrypt(path, contents[i]);		// encrypt and create a file
			cout << path.c_str() << " created\n";
		}
		else {
			cout << "Can't get the XML file\n";
		}
	}
	
	contents.clear();
	delete lt;
	delete ci;
	delete getter;
	delete parser;

	return 0;
}
catch (exception& e)
{
	cerr << "error: " << e.what() << '\n';
	exit(-1);
}
catch (...)
{
	cerr << "unknown exception\n";
	exit(-1);
}


