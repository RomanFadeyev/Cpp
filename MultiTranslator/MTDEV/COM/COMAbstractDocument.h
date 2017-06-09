/********************************************************************
	created:	2004/08/21
	created:	21:8:2004   12:37
	filename: 	x:\multitranslator\sources\mtdev\com\comabstractdocument.h
	file path:	x:\multitranslator\sources\mtdev\com
	file base:	comabstractdocument
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	ќбъ€вление абстрактного COM-класса документа
*********************************************************************/

#pragma once
#include "COMInterfaces.h"       

namespace COM
{
	template <typename T, typename DocT>
	class ATL_NO_VTABLE AbstractDocument : 	public IDispatchImpl<T>
	{
	protected:
		DocT *	m_pOwner;
	public:
		AbstractDocument();
		virtual ~AbstractDocument();

		void LinkToOwner(DocT * pOwner);

		//From IDocument
		STDMETHOD(get_Application)(IApplication** pVal);
		STDMETHOD(Save)(void);
		STDMETHOD(Close)(void);

		STDMETHOD(get_Modified)(VARIANT_BOOL* pVal);
		STDMETHOD(put_Modified)(VARIANT_BOOL newVal);
		
		STDMETHOD(get_Path)(BSTR* pVal);
	};

	//--------------------------------------------------------------------
	template <typename T, typename DocT>
	AbstractDocument<T,DocT>::AbstractDocument(): m_pOwner(NULL)
	{
	}
	//--------------------------------------------------------------------
	template <typename T, typename DocT>
	inline AbstractDocument<T,DocT>::~AbstractDocument()
	{
		if (m_pOwner!=NULL){
			m_pOwner->m_pCoClass = NULL;
			m_pOwner=NULL; 
		}
	}
	//--------------------------------------------------------------------
	template <typename T, typename DocT>
	void AbstractDocument<T,DocT>::LinkToOwner(DocT * pOwner)
	{
		_ASSERT(pOwner!=NULL);
		m_pOwner = pOwner;
	}
	//--------------------------------------------------------------------
	template <typename T, typename DocT>
	STDMETHODIMP AbstractDocument<T,DocT>::get_Application(IApplication** pVal)
	{
		AFX_MANAGE_STATE(AfxGetAppModuleState());

		IApplicationLink * pAppLink=singleton_cache::GetApplicationLink();
		ASSERT_RETURN_E_FAIL(pAppLink);

		*pVal = pAppLink->GetComInterface().detach();
		_ASSERT(*pVal);

		return S_OK;
	}
	//--------------------------------------------------------------------
	template <typename T, typename DocT>
	STDMETHODIMP AbstractDocument<T,DocT>::get_Modified(VARIANT_BOOL* pVal)
	{
		AFX_MANAGE_STATE(AfxGetAppModuleState());
		ASSERT_RETURN_E_FAIL(m_pOwner);

		*pVal = m_pOwner->IsModified();
		return S_OK;
	}
	//--------------------------------------------------------------------
	template <typename T, typename DocT>
	STDMETHODIMP AbstractDocument<T,DocT>::put_Modified(VARIANT_BOOL newVal)
	{
		AFX_MANAGE_STATE(AfxGetAppModuleState());
		ASSERT_RETURN_E_FAIL(m_pOwner);

		m_pOwner->SetModifiedFlag(newVal!=FALSE);
		return S_OK;
	}
	//--------------------------------------------------------------------
	template <typename T, typename DocT>
	STDMETHODIMP AbstractDocument<T,DocT>::get_Path(BSTR* pVal)
	{
		AFX_MANAGE_STATE(AfxGetAppModuleState());
		ASSERT_RETURN_E_FAIL(m_pOwner);

		*pVal = _bstr_t(m_pOwner->GetPath().c_str()).Detach();
		return S_OK;
	}
	//--------------------------------------------------------------------
	template <typename T, typename DocT>
	STDMETHODIMP AbstractDocument<T,DocT>::Save(void)
	{
		AFX_MANAGE_STATE(AfxGetAppModuleState());
		ASSERT_RETURN_E_FAIL(m_pOwner);

		m_pOwner->Save();
		return S_OK;
	}
	//--------------------------------------------------------------------
	template <typename T, typename DocT>
	STDMETHODIMP AbstractDocument<T,DocT>::Close(void)
	{
		AFX_MANAGE_STATE(AfxGetAppModuleState());
		ASSERT_RETURN_E_FAIL(m_pOwner);

		DocT * pOwner=m_pOwner;
		m_pOwner->m_pCoClass = NULL;
		m_pOwner=NULL; 

		pOwner->OnCmdMsg(ID_FILE_CLOSE,0,NULL,NULL);
		return S_OK;
	}
}; // of namespace
