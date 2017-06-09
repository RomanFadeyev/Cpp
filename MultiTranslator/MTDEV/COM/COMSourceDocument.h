/********************************************************************
	created:	2004/08/19
	created:	19:8:2004   22:42
	filename: 	x:\multitranslator\sources\mtdev\com\comsourcedocument.h
	file path:	x:\multitranslator\sources\mtdev\com
	file base:	comsourcedocument
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	COM-интерфейс к документу Source

	Примеч: Глюк компилятора - все исходники, которые используют заголовки MIDL, 
	должны лежать в одной общей директории, или нужно включить режим
	Debug Information Format как Program Database for Edit & Continue (/ZI). 

*********************************************************************/

#pragma once
#include "COMInterfaces.h"       
#include "COMAbstractDocument.h"

class CSourceDoc;

namespace COM
{
	//------------------------------------------------------------------------
	// SourceDocument
	//------------------------------------------------------------------------
	[
		coclass,
		threading("apartment"),
		//event_source("com"),
		vi_progid("MTDev.SourceDocument"),
		progid("MTDev.SourceDocument.1"),
		version(1.0),
		uuid("0314E4F6-E6C5-4773-84D4-B71E8945851B"),
		helpstring("SourceDocument Class")
	]
	class ATL_NO_VTABLE SourceDocument : public AbstractDocument<ISourceDocument,CSourceDoc>
	{
	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

		SourceDocument(){}

		override HRESULT FinalConstruct() {return S_OK;};
		override void FinalRelease() {};
	public:
		//From IDocument

		//from ISourceDocument
	};
}