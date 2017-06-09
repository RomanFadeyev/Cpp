// CObjectEx.cpp: implementation of the CCObjectEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CObjectEx.h"
#include "Names.h"

#include "CObjectEx.h" //Один из родителей

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
CCObjectEx::CCObjectEx()
{
	//TRACE0("ExОbject:");
	Connect();
}
//------------------------------------------------------------------------
CCObjectEx::~CCObjectEx()
{
	//TRACE0("ExObject:");
}
//------------------------------------------------------------------------
