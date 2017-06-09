/********************************************************************
	created:	2004/07/11
	created:	11:7:2004   16:28
	filename: 	X:\MultiTranslator\sources\mtdev\docs&views\sourcedocstyle.h
	file path:	X:\MultiTranslator\sources\mtdev\docs&views
	file base:	sourcedocstyle
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Класс, обеспечивающий чтение свойств (расраска,отступы и проч)
				различных типов документов
*********************************************************************/

#pragma once

#include <PropSet.h>
#include "CobjectEx.h"	//Один из родителей
#include <MiscUtils.h>

using namespace	properties;


//Класс, икнапсулиующий	работу с настройками раскраски
class CSourceDocStyler: public ISourceDocStylerLink,
						public IApplicationLink::IIdleHandler,
						public CCObjectEx
{
public: 
	//-- class -----------------------------------------------------------
	class CDocumentStyleDescriptor: public IDocumentStyleDescriptor, 
									public mtdev_lib::IDocumentColorChema
	{
	private:
		mtdev_lib::IHilightScriptXMLProviderPtr m_ScriptProvider;
	public:
		//Получение чистого COM iface
		mtdev_lib::IHilightScriptXMLProvider * GetScriptProvider() {return m_ScriptProvider.GetInterfacePtr();}

		DEFINE_ADD_REF_STUB
		DEFINE_RELEASE_STUB
		
		BEGIN_QUERY_INTERFACE 
			COM_INTERFACE_SUPPORT(mtdev_lib::IDocumentStyleDescriptor)
			COM_INTERFACE_SUPPORT(mtdev_lib::IDocumentColorChema)
		END_QUERY_INTERFACE

		//From IDocumentStyleDescriptor
		override void GetStatementIndent(OUT style_and_words & result);
		override void GetStatementEnd(OUT style_and_words & result);
		override void GetBlockStart(OUT style_and_words & result);
		override void GetBlockEnd(OUT style_and_words & result);
		override void GetStyleDescByID(int aStyle, OUT ISourceDocStylerLink::style_definition & );
		override void GetStyleDescByIndex(int aStyleIndex, OUT ISourceDocStylerLink::style_definition & );

		override bool GetIndentOpening();
		override bool GetIndentClosing();

		//From mtdev_lib::IDocumentStyleDescriptor
		override HRESULT __stdcall get_DocumentName (BSTR * Value );
		override HRESULT __stdcall get_ColorChema (mtdev_lib::IDocumentColorChema * * Value );
		override HRESULT __stdcall get_TabSize (long * Value );
		override HRESULT __stdcall put_TabSize (long Value );
		override HRESULT __stdcall get_IndentSize (long * Value );
		override HRESULT __stdcall put_IndentSize (long Value );
		override HRESULT __stdcall get_UseTabs (VARIANT_BOOL * Value);
		override HRESULT __stdcall put_UseTabs (VARIANT_BOOL Value );
		override HRESULT __stdcall get_WordWrap (VARIANT_BOOL * Value );
		override HRESULT __stdcall put_WordWrap (VARIANT_BOOL Value );
		override HRESULT __stdcall get_Keywords (long ClassNo,BSTR * Value);
		override HRESULT __stdcall put_Keywords (long ClassNo,BSTR Value);
		override HRESULT __stdcall get_Extensions (BSTR * Value );
		override HRESULT __stdcall put_Extensions (BSTR Value );
		override HRESULT __stdcall get_Lexer (BSTR * Value );
		override HRESULT __stdcall put_Lexer (BSTR Value );

		override HRESULT __stdcall get_FoldVisible (VARIANT_BOOL * Value);
		override HRESULT __stdcall get_FoldCode (VARIANT_BOOL * Value);
		override HRESULT __stdcall put_FoldCode (VARIANT_BOOL Value);
		override HRESULT __stdcall get_FoldComment (VARIANT_BOOL * Value) ;
		override HRESULT __stdcall put_FoldComment (VARIANT_BOOL Value);

		override HRESULT __stdcall get_StoragePath (BSTR * Value );

		//From IDocumentColorChema
		override HRESULT __stdcall get_ItemCount (long * Value);
		override HRESULT __stdcall get_ForeColor (long Index,OLE_COLOR * Value);
		override HRESULT __stdcall put_ForeColor (long Index,OLE_COLOR Value);
		override HRESULT __stdcall get_BkColor (long Index,OLE_COLOR * Value);
		override HRESULT __stdcall put_BkColor (long Index,OLE_COLOR Value);
		override HRESULT __stdcall get_FontName (long Index,BSTR * Value);
		override HRESULT __stdcall put_FontName (long Index,BSTR Value);
		override HRESULT __stdcall get_FontSize (long Index,long * Value);
		override HRESULT __stdcall put_FontSize (long Index,long Value);
		override HRESULT __stdcall get_IsBold (long Index,VARIANT_BOOL * Value);
		override HRESULT __stdcall put_IsBold (long Index,VARIANT_BOOL Value);
		override HRESULT __stdcall get_IsItalic (long Index,VARIANT_BOOL * Value);
		override HRESULT __stdcall put_IsItalic (long Index,VARIANT_BOOL Value);
		override HRESULT __stdcall get_IsUnderline (long Index,VARIANT_BOOL * Value);
		override HRESULT __stdcall put_IsUnderline (long Index,VARIANT_BOOL Value);
		override HRESULT __stdcall get_ItemName (long Index,BSTR * Value);
		override HRESULT __stdcall get_ItemVisible (long Index,VARIANT_BOOL * Value );
		override HRESULT __stdcall get_ItemID (long Index,long * Value );

		void SetStyleDescByIdndex(int aStyleIndex, IN ISourceDocStylerLink::style_definition & );
		bool IsModified();
		void SaveChanges();

		CDocumentStyleDescriptor(mtdev_lib::IHilightScriptXMLProviderPtr & aScriptProvider): m_ScriptProvider(aScriptProvider){
		}
	};
private:
	lst_doc_types				m_lstDocTypes;
	misc_utils::file_watch_set	m_WatchDog;
	bool						m_bInited;		
	bool						m_bChangedOutside;

	typedef vector<CDocumentStyleDescriptor* > list_descriptors;
	list_descriptors			m_lstDescriptors;
	CDocumentStyleDescriptor*	m_pDefaultDescriptor;

	void Init();
	void InitNeeded()	{if (!m_bInited) Init();}

	//Ищет в указанной директории все файлы	конфигурации
	void EnumerateFiles(cstr szDirectory);
	//Добавляет	к себе содержимое файла
	void ReadProperties(const char * szFileName);
		
	void OnChangeOutside(misc_utils::file_watch &,misc_utils::file_watch::change_type);
public:
	//IUnknown
	DEFINE_ADD_REF_STUB
	DEFINE_RELEASE_STUB

	BEGIN_QUERY_INTERFACE 
		COM_INTERFACE_SUPPORT(mtdev_lib::IPreferencesDocumentTypeProvider)
	END_QUERY_INTERFACE

	//From ISourceDocStylerLink
	override IDocumentStyleDescriptor * FindStyleDescriptor(string_adapter strFileExt);
	override const lst_doc_types & GetDocTypes();

	//From IApplicationLink::IIdleHandler
	override void IApplicationLink::IIdleHandler::OnIdle(bool & IN OUT bStopIdle);

	//From CCObjectEx 
	override void OnRunApp()			{singleton_cache::GetApplicationLink()->IncludeToOnIdleMap(this);}
	override void OnStopApp()		{singleton_cache::GetApplicationLink()->ExcludeFromOnIdleMap(this);}

	//From mtdev_lib::IPreferencesDocumentTypeProvider
	override HRESULT __stdcall get_ItemCount (long * Value );
	override HRESULT __stdcall get_Items (long Index, mtdev_lib::IDocumentStyleDescriptor * * Value );
	override HRESULT __stdcall get_Modified (VARIANT_BOOL * Value);
	override HRESULT __stdcall raw_SaveChanges ( );

	CSourceDocStyler(void);
	virtual ~CSourceDocStyler(void);
};
