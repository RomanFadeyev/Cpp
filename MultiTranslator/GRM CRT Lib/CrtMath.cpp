/********************************************************************
	created:	2004/02/22
	created:	22:2:2004   22:24
	filename: 	$(Projects)\sources\grm crt lib\crtmath.cpp
	file path:	$(Projects)\sources\grm crt lib
	file base:	crtmath
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once
#include "StdAfx.h"

#define  _USE_MATH_DEFINES
#include <BaseUtils.h>
#include <Math.h>
#include <TChar.h>
#include "GrmCrt.h"

using namespace base_utils;

//------------------------------------------------------------------------
// global functions for crt files
//------------------------------------------------------------------------

#define REG(Name,Comment) registrator reg_##Name(Name,_T("Math Operations"), #Name, Comment)

namespace crt_math
{
	EXPORT double_proxy WINAPI MathCeil(double_proxy fValue)
	{
		return double_proxy::from_double(ceil(double_proxy::to_double(fValue)));
	}
	REG(MathCeil,"Returns the lowest integer greater than or equal to specified value. For example:	MathCeil(-2.8) = -2, MathCeil(2.8) = 3");

	EXPORT double_proxy WINAPI MathFloor(double_proxy fValue)
	{
		return double_proxy::from_double(floor(double_proxy::to_double(fValue)));
	}
	REG(MathFloor,"Returns the highest integer less than or equal to specified value. For example: MathFloor(-2.8) = -3,MathFloor(2.8) = 2");
	
	EXPORT int WINAPI MathRound(double_proxy fValue)
	{
		return int(double_proxy::to_double(fValue)+0.5);
	}
	REG(MathRound,"Rounds a real-type value to an integer-type value.");

	EXPORT int WINAPI MathTrunc(double_proxy fValue)
	{
		return int(double_proxy::to_double(fValue));
	}
	REG(MathTrunc,"Truncates a real-type value to an integer-type value.");

	EXPORT double_proxy WINAPI MathSin(double_proxy fValue)
	{
		return double_proxy::from_double(sin(double_proxy::to_double(fValue)));
	}
	REG(MathSin,"Returns the sine of the specified angle, in radians.");

	EXPORT double_proxy WINAPI MathCos(double_proxy fValue)
	{
		return double_proxy::from_double(cos(double_proxy::to_double(fValue)));
	}
	REG(MathCos,"Returns the cosine of the specified angle, in radians.");

	EXPORT double_proxy WINAPI MathTan(double_proxy fValue)
	{
		return double_proxy::from_double(tan(double_proxy::to_double(fValue)));
	}
	REG(MathTan,"Returns the tangent of the specified angle, in radians.");

	EXPORT double_proxy WINAPI MathExp(double_proxy fValue)
	{
		return double_proxy::from_double(exp(double_proxy::to_double(fValue)));
	}
	REG(MathExp,"Returns the value of e raised to the power of specified argument, where e is the base of the natural logarithms");

	EXPORT double_proxy WINAPI MathPow(double_proxy fBase, double_proxy fExponent)
	{
		return double_proxy::from_double(pow(double_proxy::to_double(fBase),double_proxy::to_double(fExponent)));
	}
	REG(MathPow,"MathPow(Base,Exponent) raises Base to Exponent power.");

	EXPORT int WINAPI MathFact(int nValue)
	{
		return base_utils::factorial(nValue);
	}
	REG(MathFact,"Returns factorial. The argument must be integral value");

	EXPORT double_proxy WINAPI MathSqrt(double_proxy fValue) 
	{
		return double_proxy::from_double(sqrt(double_proxy::to_double(fValue)));
	}
	REG(MathSqrt,"Returns the square root of specified value.");

	// Second in minute (0-59)
	EXPORT double_proxy WINAPI MathLog(double_proxy fValue)
	{
		return double_proxy::from_double(log(double_proxy::to_double(fValue)));
	}
	REG(MathLog,"Returns the natural logarithm (Log(e) = 1) of the specified value.");

	EXPORT double_proxy WINAPI MathPi()
	{
		return double_proxy::from_double(M_PI);
	}
	REG(MathPi,"Returns 3.141592...");

	EXPORT double_proxy WINAPI MathE()
	{
		return double_proxy::from_double(M_E);
	}
	REG(MathE,"Returns 2.71828...");
}
