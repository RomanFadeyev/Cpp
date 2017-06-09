/********************************************************************
	created:	2003/01/26
	created:	26:1:2003   13:29
	filename: 	$(Projects)\sources\mtdev\com\comprojectdocument.h
	file path:	$(Projects)\sources\mtdev\com
	file base:	comprojectdocument
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	COM-интерфейс к документу проекта

	Примеч: Глюк компилятора - все исходники, которые используют заголовки MIDL, 
	должны лежать в одной общей директории, или нужно включить режим
	Debug Information Format как Program Database for Edit & Continue (/ZI). 
*********************************************************************/

#pragma once
#include "COMInterfaces.h"       
#include "COMAbstractDocument.h"

class CProjectDoc;

namespace COM
{
	//------------------------------------------------------------------------
	// ProjectDocument
	//------------------------------------------------------------------------
	[
		coclass,
		threading("apartment"),
		//event_source("com"),
		vi_progid("MTDev.ProjectDocument"),
		progid("MTDev.ProjectDocument.1"),
		version(1.0),
		uuid("206DDA10-728E-4C72-81CC-36FB3BE80916"),
		helpstring("ProjectDocument Class")
	]
	class ATL_NO_VTABLE ProjectDocument : public AbstractDocument<IProjectDocument,CProjectDoc>
	{
	protected:
		bool			m_bInBuild;
		bool			m_bBuildResult;
		void BuildWaitToRun(bool Succesful)
		{
			m_bInBuild		= false;
			m_bBuildResult	= Succesful;
		}

	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

		ProjectDocument(){}
		HRESULT FinalConstruct() {return S_OK;};
		void FinalRelease() {};
	public:
		//from IProjectDocument
		STDMETHOD(SaveAll)(VARIANT_BOOL* Result);
		STDMETHOD(Build)  (VARIANT_BOOL* Result);
		STDMETHOD(Run)    (LONG* ExitCode, VARIANT_BOOL* Result);
		STDMETHOD(Clean)  (void);

		STDMETHOD(AddFile)    (BSTR FilePath, VARIANT_BOOL* Result);
		STDMETHOD(RemoveFile) (BSTR FilePath, VARIANT_BOOL* Result);

		STDMETHOD(GetFileFirst) (BSTR* FileName, VARIANT_BOOL* Result);
		STDMETHOD(GetFileNext)  (BSTR* FileName, VARIANT_BOOL* Result);

		STDMETHOD(get_Active)   (VARIANT_BOOL* pVal);
		STDMETHOD(put_Active)   (VARIANT_BOOL newVal);
		STDMETHOD(get_TargetPath)(BSTR* pVal);
		STDMETHOD(get_TargetDirectory)(BSTR* pVal);
		STDMETHOD(get_Notes)    (BSTR* pVal);
		STDMETHOD(put_Notes)    (BSTR newVal);
		STDMETHOD(get_Params)   (BSTR* pVal);
		STDMETHOD(put_Params)   (BSTR newVal);
		STDMETHOD(get_Switches) (BSTR* pVal);
		STDMETHOD(put_Switches) (BSTR newVal);
		STDMETHOD(get_LangGens) (BSTR* pVal);
		STDMETHOD(put_LangGens) (BSTR newVal);
	};
}	