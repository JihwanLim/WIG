#ifndef __CONFIG_INFO_H
#define __CONFIG_INFO_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

using namespace std;
using namespace libconfig;

const string OUTER_VAR_NAME = "forecast";
const string INNER_VAR_NAME = "location";

class ConfigInfo {
private:
	Config cfg;
	string nx;
	string ny;

public:
	ConfigInfo(string fileName);
	string getNx() const;
	string getNy() const;

private:
	void readValues();
};

#endif


