#ifndef __LOCAL_TIME_H
#define __LOCAL_TIME_H

#include <iostream>
#include <ctime>
using namespace std;

class LocalTime {
private:
	string baseDate;
	string baseTime;

public:
	string getBaseDate();
	string getBaseTime();
	string getBaseDateTime();

private:
	void setLocalTime();
};

#endif


