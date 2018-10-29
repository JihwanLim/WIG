#include "../Headers/local_time.h"


string LocalTime::getBaseDate()
{
	setLocalTime();
	return baseDate;
}


string LocalTime::getBaseTime()
{
	setLocalTime();
	return baseTime;
}


string LocalTime::getBaseDateTime()
{
	setLocalTime();
	return baseDate + baseTime;
}


void LocalTime::setLocalTime()
{
	time_t rawTime;
	struct tm* timeInfo;
	char buffer[80];

	time(&rawTime);
	timeInfo = localtime(&rawTime);


	// 시간을 5분 느리게 설정
	if (timeInfo->tm_min <= 5) {
		timeInfo->tm_min = 55 + timeInfo->tm_min;
		timeInfo->tm_hour -= 1;
	}
	else {
		timeInfo->tm_min -= 5;
	}


	strftime(buffer, 80, "%Y%m%d", timeInfo);
	baseDate = buffer;

	strftime(buffer, 80, "%H%M", timeInfo);
	baseTime = buffer;
}
