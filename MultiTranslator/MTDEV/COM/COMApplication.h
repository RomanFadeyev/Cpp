/********************************************************************
	created:	2004/08/19
	created:	19:8:2004   23:02
	filename: 	x:\multitranslator\sources\mtdev\com\comapplication.h
	file path:	x:\multitranslator\sources\mtdev\com
	file base:	comapplication
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Declaration of the Application co-class
*********************************************************************/

#pragma once
#include "resource.h"       // main symbols
#include "COMInterfaces.h"

class CMTDevApp;
namespace COM{

	[
		coclass,
		threading("apartment"),
		vi_progid("MTDev.Application"),
		progid("MTDev.Application.1"),
		version(1.0),
		uuid("ADC6D969-C5D5-49E4-A75C-F58156BDDBC9"),
		helpstring("Application Class")
	]
	//------------------------------------------------------------------------
	class ATL_NO_VTABLE Application : public IApplication
	{
	protected:
		CMTDevApp *m_pOwner;
	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

		//constrcutor/destructor
		Application():m_pOwner(NULL){
		}
		HRESULT FinalConstruct();
		void FinalRelease();

		void LinkToOwner(CMTDevApp *pOwner);

		STDMETHOD(get_Visible)(VARIANT_BOOL* pVal);
		STDMETHOD(put_Visible)(VARIANT_BOOL newVal);
		STDMETHOD(ScreenRefresh)(void);
		STDMETHOD(Resize)(LONG Width, LONG Height);
		STDMETHOD(Activate)(void);
		STDMETHOD(Quit)(VARIANT_BOOL SaveChanges);
		STDMETHOD(get_ProjectCount)(LONG* pVal);
		STDMETHOD(get_Projects)(LONG nIndex, IProjectDocument** pDoc);
		STDMETHOD(get_ActiveProject)(IProjectDocument** pVal);
		STDMETHOD(get_MRUDocument)(BSTR* pVal);
		STDMETHOD(FindDocument)(BSTR Path, IDocument** pDoc);
		STDMETHOD(OpenDocument)(BSTR Path, IDocument** pDoc);
		STDMETHOD(CreateProject)(BSTR Path, IProjectDocument** pDoc);
		STDMETHOD(get_Parser)(IParser** pVal);
	}; 
};
