/********************************************************************
	created:	2004/01/21
	created:	21:1:2004   20:55
	filename: 	$(Projects)\sources\grm crt lib\crtdatetime.cpp
	file path:	$(Projects)\sources\grm crt lib
	file base:	crtdatetime
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once

#include "StdAfx.h"
#include <BaseUtils.h>
#include <OleAuto.h>
#include <ComUtil.h>
#include <TChar.h>
#include "GrmCrt.h"


using namespace base_utils;

//------------------------------------------------------------------------
// global functions for crt files
//------------------------------------------------------------------------
typedef double_proxy date_time ;

#define REG(Name,Comment) registrator reg_##Name(Name,_T("Date & Time"), #Name, Comment)

namespace crt_datetime
{
	static DATE GetCurTime()
	{
		SYSTEMTIME st; GetLocalTime (&st);
		DATE res; ::SystemTimeToVariantTime(&st, &res);
		return res;
	}

	static date_time OleDateToDateTime(DATE aDate)
	{
		return *((date_time*)(&aDate)); 
	}

	static DATE DateTimeToOleDate(date_time aDate)
	{
		return *((DATE*)(&aDate)); 
	}

	static SYSTEMTIME OleDateToSysDate(DATE aDate)
	{
		SYSTEMTIME res;
		::VariantTimeToSystemTime(aDate, &res);
		return res;
	}
	
	static SYSTEMTIME DateTimeToSysDate(date_time aDate)
	{
		SYSTEMTIME res;
		::VariantTimeToSystemTime(DateTimeToOleDate(aDate), &res);
		return res;
	}	

	static struct tm DateTimeToTM(date_time aDate)
	{
		UDATE ud;
		struct tm tmTemp = {0};
		if (S_OK!=VarUdateFromDate(DateTimeToOleDate(aDate), 0, &ud))
			return tmTemp;


		tmTemp.tm_sec	= ud.st.wSecond;
		tmTemp.tm_min	= ud.st.wMinute;
		tmTemp.tm_hour	= ud.st.wHour;
		tmTemp.tm_mday	= ud.st.wDay;
		tmTemp.tm_mon	= ud.st.wMonth - 1;
		tmTemp.tm_year	= ud.st.wYear - 1900;
		tmTemp.tm_wday	= ud.st.wDayOfWeek;
		tmTemp.tm_yday	= ud.wDayOfYear - 1;
		tmTemp.tm_isdst	= 0;
		
		return tmTemp;
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------

	//Get Current Date And Time
	EXPORT date_time WINAPI DateGetCurrent()
	{
		return OleDateToDateTime(GetCurTime());
	}
	REG(DateGetCurrent,"Returns current date and time");

	EXPORT cstr WINAPI DateToStr(date_time aDate)
	{
		STRING_IMPL("");
		s = ttos(DateTimeToSysDate(aDate));
		return s.c_str();
	}
	REG(DateToStr,"Returns string representation of a specified date and time");

	EXPORT cstr WINAPI DateDToStr(date_time aDate)
	{
		STRING_IMPL("");
		s.reserve(256);
		
		if (!GetDateFormat(	LOCALE_USER_DEFAULT, 0,&DateTimeToSysDate(aDate),NULL,
			s.buffer(),static_cast<int>(s.buffer_size())))
			return "";

		return s.c_str();
	}
	REG(DateDToStr,"Returns string representation of a specified date (without time)");

	EXPORT cstr WINAPI DateTToStr(date_time aDate)
	{
		STRING_IMPL("");
		s.reserve(256);

		if (!GetTimeFormat(	LOCALE_USER_DEFAULT, 0,&DateTimeToSysDate(aDate),NULL,
			s.buffer(),static_cast<int>(s.buffer_size())))
			return "";

		return s.c_str();
	}
	REG(DateTToStr,"Returns string representation of a specified time (without date)");

	EXPORT int WINAPI DateGetYear(date_time aDate)
	{
		return DateTimeToSysDate(aDate).wYear;
	}
	REG(DateGetYear,"Returns a year of a specified date");

	// Month of year (1 = January)
	EXPORT int WINAPI DateGetMonth(date_time aDate)
	{
		return DateTimeToSysDate(aDate).wMonth;
	}
	REG(DateGetMonth,"Returns a month of a specified date (1=January, 2=February etc)");

	// Day of month (1-31)
	EXPORT int WINAPI DateGetDay(date_time aDate)
	{
		return DateTimeToSysDate(aDate).wDay;
	}
	REG(DateGetDay,"Returns a day of a month according to a specified date (1-31)");

	// Hour in day (0-23)
	EXPORT int WINAPI DateGetHour(date_time aDate)
	{
		return DateTimeToSysDate(aDate).wHour;
	}
	REG(DateGetHour,"Returns a hour of a specified date (0-23)");

	// Minute in hour (0-59)
	EXPORT int WINAPI DateGetMinute(date_time aDate) 
	{
		return DateTimeToSysDate(aDate).wMinute;
	}
	REG(DateGetMinute,"Returns a minute of a specified date (0-59)");
	
	// Second in minute (0-59)
	EXPORT int WINAPI DateGetSecond(date_time aDate)
	{
		return DateTimeToSysDate(aDate).wSecond;
	}
	REG(DateGetSecond,"Returns a second of a specified date (0-59)");

	// Day of week (1 = Sunday, 2 = Monday, ..., 7 = Saturday)
	EXPORT int WINAPI DateGetDayOfWeek(date_time aDate)
	{
		return DateTimeToSysDate(aDate).wDay+1;
	}
	REG(DateGetDayOfWeek,"Returns a day of week according to a specified date (1 = Sunday, 2 = Monday, ..., 7 = Saturday)");
}
