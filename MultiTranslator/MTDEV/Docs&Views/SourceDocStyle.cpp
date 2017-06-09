/********************************************************************
	created:	2004/07/11
	created:	11:7:2004   16:27
	filename: 	X:\MultiTranslator\sources\mtdev\docs&views\sourcedocstyle.cpp
	file path:	X:\MultiTranslator\sources\mtdev\docs&views
	file base:	sourcedocstyle
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Класс, обеспечивающий чтение свойств (расраска,отступы и проч)
				различных типов документов
*********************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "SourceDocStyle.h"
#include "Names.h"

#include <ModuleNames.h>
#include <Scintilla.h>
#include <SciLexer.h>

//Создание переходника для объекта
static i_objects::object_thunk<CSourceDocStyler> SourceDocStyler(__MSG_PREFIX__,i_objects::support_interface<ISourceDocStylerLink>());

const cstr szDefaultChemaFileName = "default";

//////////////////////////////////////////////////////////////////////////
// CSourceDocStyler
//////////////////////////////////////////////////////////////////////////

CSourceDocStyler::CSourceDocStyler() :m_bInited(false),m_bChangedOutside(false),m_pDefaultDescriptor(NULL)
{
}
//------------------------------------------------------------------------
CSourceDocStyler::~CSourceDocStyler()
{
	for (list_descriptors::iterator i = m_lstDescriptors.begin(); i!=m_lstDescriptors.end(); ++i)
		delete (*i);
	free_and_null(m_pDefaultDescriptor);
}
//------------------------------------------------------------------------
void CSourceDocStyler::Init()
{
	CWaitCursor WaitCursor;

	m_WatchDog.clear();
	m_lstDocTypes.clear();
	m_bChangedOutside=false;

	//Директория программы\HighLight
	EnumerateFiles(get_module_filedir().cat(module_names::szFolderHighLight).c_str());
	//Текущая директория\HighLight
	EnumerateFiles(string_smart(get_current_dir(),"\"",module_names::szFolderHighLight).c_str());

	//Дополнительная директория поиска
	string_cstr strPath=get_env_variable(module_names::szEnvPath);
	if (!strPath.isempty())
		EnumerateFiles(include_backslash(strPath).cat(module_names::szFolderHighLight).c_str());

	// --- Файл схемы по умолчанию --- 
	string_smart strDefaultChema(get_module_filedir(),include_backslash(module_names::szFolderHighLight),szDefaultChemaFileName,".",szHiLightExtension);
	if (!is_file_exist(strDefaultChema))
		strDefaultChema = string_smart(include_backslash(module_names::szFolderHighLight),szDefaultChemaFileName,".",szHiLightExtension);
	if (!is_file_exist(strDefaultChema))
		strDefaultChema = string_smart(include_backslash(get_env_variable(module_names::szEnvPath)),include_backslash(module_names::szFolderHighLight),szDefaultChemaFileName,".",szHiLightExtension);
	
	if (!is_file_exist(strDefaultChema) && (m_pDefaultDescriptor==NULL)){
		LOG_ERROR_SENDER(string_smart("Can't find file ",strDefaultChema).c_str(),this);
		guiMsgError(0,FormatRes(IDS_CANT_LOAD_INSTALLED_FILE_s,HRESOURCES,strDefaultChema.c_str()).c_str());
	}
	//Загружаем схему по умолчанию
	else{
		if(!m_pDefaultDescriptor) {
			mtdev_lib::IHilightScriptXMLProviderPtr spScriptProvider(__uuidof(mtdev_lib::HilightScriptXMLProvider));
			m_pDefaultDescriptor = new CDocumentStyleDescriptor(spScriptProvider);
		}

		if (!m_pDefaultDescriptor->GetScriptProvider()->LoadFile(strDefaultChema.c_str())){
			LOG_ERROR_SENDER(string_smart("Can't lod file ",strDefaultChema).c_str(),this);
			guiMsgError(0,FormatRes(IDS_CANT_LOAD_INSTALLED_FILE_s,HRESOURCES,strDefaultChema.c_str()).c_str());
		}
	}

	m_bInited=true;
}
//------------------------------------------------------------------------
void CSourceDocStyler::EnumerateFiles(cstr szDirectory)
{
	string_smart strDirectory(include_backslash(szDirectory));
	string_smart strWildMask(strDirectory.c_str(),_T("*."),szHiLightExtension);
	_finddata_t	FindData;
	long FileSpec;
	
	
	if ((FileSpec=_findfirst(strWildMask.c_str(),&FindData))!=-1){
		ReadProperties(string_smart(strDirectory.c_str(),FindData.name).c_str());
		while (_findnext(FileSpec,&FindData)!=-1){
			ReadProperties(string_smart (strDirectory.c_str(),FindData.name).c_str());
		}
		
		m_WatchDog.add(szDirectory,misc_utils::file_watch_set::call_back_t(this,OnChangeOutside));
		_findclose(FileSpec);
	}
}
//------------------------------------------------------------------------
void CSourceDocStyler::ReadProperties(cstr szFileName)
{
	LOG_PROTECT_CODE_BEGIN
	
	CDocumentStyleDescriptor * pDescriptor = NULL;

	//для начала поищем среди уже существующих
	for (list_descriptors::iterator i = m_lstDescriptors.begin(); i!=m_lstDescriptors.end(); ++i)
	{
		//Полезем глубже - достанем COM-провайдера
		mtdev_lib::IHilightScriptXMLProvider * pScriptProvider = (*i)->GetScriptProvider();
		_ASSERT(pScriptProvider);

		//нашли такой файл, перегружаемся и выходим
		if (string_smart(pScriptProvider->FileName).cmpi(szFileName)){
			pScriptProvider->LoadFile(szFileName); 
			pDescriptor = (*i);
			break;
		}
	}

	//Ничего не нашли, добавляем новую запись
	if (pDescriptor==NULL)
	{
		mtdev_lib::IHilightScriptXMLProviderPtr spScriptProvider(__uuidof(mtdev_lib::HilightScriptXMLProvider));

		if (spScriptProvider->LoadFile(szFileName)){
			//Если все нормально и не было исключений, можно добавлять скрипт в список
			pDescriptor = new CDocumentStyleDescriptor(spScriptProvider);
			m_lstDescriptors.push_back(pDescriptor);	
		}
	}
	
	//Теперь регистрируем тип документа
	if (pDescriptor){
		string_smart strName = pDescriptor->GetScriptProvider()->DocumentName;
		string_smart strFilter = pDescriptor->GetScriptProvider()->Extensions;
		m_lstDocTypes.push_back(DOCUMENT_INFO(strName.trim(),strFilter.trim()));
	}

	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceDocStyler::OnChangeOutside(misc_utils::file_watch &, misc_utils::file_watch::change_type)
{
	LOG_PROTECT_CODE_BEGIN
	friend class re_read_f;

	//класс функтор для синхроницации и перехода в контекст главного потока
	class re_read_f: public functors::IFunctorRemote
	{
	private:
		CSourceDocStyler * m_pStyler;
		override void invoke() {
			m_pStyler->m_bChangedOutside=true;
			m_pStyler->m_WatchDog.clear();	//Это, по моему, и не обязательно
		}
	public:
		re_read_f(CSourceDocStyler * pStyler): m_pStyler(pStyler) {
			i_objects::GetInterface<IMainFrameLink>()->SyncInvoke(*this);}
	};
	
	//Переходим в главный поток
	re_read_f f(this); 
	LOG_PROTECT_CODE_END
}
//------------------------------------------------------------------------
void CSourceDocStyler::OnIdle(bool & IN OUT bStopIdle)
{
	if (m_bChangedOutside && m_bInited) {
		Init(); //Starting reinit...
		i_objects::event<ARGS_0> evOnChangeData(ISourceDocStylerLink::IEventSink::evOnChangeData);
		bStopIdle = true; //Stops the idle queue
	}
}
//------------------------------------------------------------------------
CSourceDocStyler::IDocumentStyleDescriptor * CSourceDocStyler::FindStyleDescriptor(string_adapter strFileExt)
{
	InitNeeded();

	//Получаем расширение без точки и прочего мусора
	string_smart strPureExt  = strFileExt;
	if (strPureExt.pos('.')!=-1)
		strPureExt = string_split(strFileExt,true,".").get_right_();

	for (list_descriptors::iterator i = m_lstDescriptors.begin(); i!=m_lstDescriptors.end(); ++i)
	{
		IDocumentStyleDescriptor * pDesc = (*i);
		//Перечень расширений могут вводить через , ; или даже |. Приводим все к одному виду
		string_smart s = str_replace(str_replace(pDesc->Extensions, ",", ";"),"|",";");
		string_list strExts = str_split_text(s,";");
		
		//Ищем указанное расширение
		for (size_t j=0; j< strExts.items().count(); j++)
		{
			//Избавляемся от точки и прочего мусора
			string_smart strPureCurExt = strExts.items()[j];
			if (strPureCurExt.pos('.')!=-1)
				strPureCurExt = string_split(strPureCurExt,true,".").get_right_();

			if (strPureCurExt.cmpi(strPureExt))
				return pDesc;
		}
	}

	//Если ничего не нашли, возвращаем стандартный стиль
	return m_pDefaultDescriptor;
}
//------------------------------------------------------------------------
const CSourceDocStyler::lst_doc_types & CSourceDocStyler::GetDocTypes()
{
	InitNeeded();
	return m_lstDocTypes;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::get_ItemCount (long * Value )
{
	InitNeeded();
	*Value  = m_lstDescriptors.size();
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::get_Items (long Index, mtdev_lib::IDocumentStyleDescriptor * * Value )
{
	InitNeeded();
	if (!inrange(Index,(long)0,(long)m_lstDescriptors.size()-1))
		throw exception("Wrong index");
	
	*Value = m_lstDescriptors[Index];
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::get_Modified (VARIANT_BOOL * Value)
{
	*Value=false;
	for (list_descriptors::iterator i =m_lstDescriptors.begin();(i!=m_lstDescriptors.end()) && (!*Value);++i)
		*Value=(*i)->IsModified();
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::raw_SaveChanges ( )
{

	if (Modified) {
		CWaitCursor cursor;
		m_WatchDog.clear();
		for (list_descriptors::iterator i =m_lstDescriptors.begin();i!=m_lstDescriptors.end();++i)
			(*i)->SaveChanges();
		
		//Провоцируем обновление
		m_bChangedOutside=true;
		bool bStopIdle;
		OnIdle(bStopIdle);
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// CSourceDocStyler::CDocumentStyleDescriptor
//////////////////////////////////////////////////////////////////////////

bool CSourceDocStyler::CDocumentStyleDescriptor::GetIndentOpening()
{
	return false;
	//??????
	//return get_bool("indent.opening",false)!=0; 
}
//------------------------------------------------------------------------
bool CSourceDocStyler::CDocumentStyleDescriptor::GetIndentClosing()
{
	//return get_bool("indent.closing",false)!=0; 
	//??????
	return false;
}
//------------------------------------------------------------------------
bool CSourceDocStyler::CDocumentStyleDescriptor::IsModified()
{
	return itob(m_ScriptProvider->Modified);
}
//------------------------------------------------------------------------
void CSourceDocStyler::CDocumentStyleDescriptor::SaveChanges()
{
	m_ScriptProvider->SaveChanges();
}
//------------------------------------------------------------------------
void CSourceDocStyler::CDocumentStyleDescriptor::GetStyleDescByID(int aStyle, OUT ISourceDocStylerLink::style_definition & sd)
{
	_bstr_t StyleName;
	_bstr_t FontName;
	long aStyleIndex;
	VARIANT_BOOL bBold,bItalics,bUnderlined,bVisible;
	m_ScriptProvider->GetStyleByID(
		aStyle,
		&aStyleIndex,
		StyleName.GetAddress(),
		&bVisible,
		FontName.GetAddress(),
		&sd.m_iSize,
		&bBold, 
		&bItalics, 
		&bUnderlined, 
		(OLE_COLOR *)&sd.m_clFore, 
		(OLE_COLOR *)&sd.m_clBack);
	
	sd.m_nStyleID = aStyle;
	sd.m_nCharset = SC_CHARSET_DEFAULT; //???? 
	sd.m_bVisible=itob(bVisible);
	sd.m_bBold=itob(bBold);
	sd.m_bItalics=itob(bItalics);
	sd.m_bUnderlined=itob(bUnderlined);
	sd.m_strFont = FontName;
	sd.m_strStyleName= StyleName;
}
//------------------------------------------------------------------------
void CSourceDocStyler::CDocumentStyleDescriptor::GetStyleDescByIndex(int aStyleIndex, OUT ISourceDocStylerLink::style_definition & sd)
{
	_bstr_t StyleName;
	_bstr_t FontName;	
	VARIANT_BOOL bBold,bItalics,bUnderlined,bVisible;

	m_ScriptProvider->GetStyleByIndex(
		aStyleIndex,
		&sd.m_nStyleID,
		StyleName.GetAddress(),
		&bVisible,
		FontName.GetAddress(),
		&sd.m_iSize,
		&bBold, 
		&bItalics, 
		&bUnderlined, 
		(OLE_COLOR *)&sd.m_clFore, 
		(OLE_COLOR *)&sd.m_clBack);

	sd.m_nCharset = SC_CHARSET_DEFAULT; //???? 
	sd.m_bVisible=itob(bVisible);
	sd.m_bBold=itob(bBold);
	sd.m_bItalics=itob(bItalics);
	sd.m_bUnderlined=itob(bUnderlined);

	sd.m_strFont = FontName;
	sd.m_strStyleName= StyleName;
}
//------------------------------------------------------------------------
void CSourceDocStyler::CDocumentStyleDescriptor::SetStyleDescByIdndex(int aStyleIndex, IN ISourceDocStylerLink::style_definition & sd)
{
	m_ScriptProvider->SetStyleByIndex(
		aStyleIndex,
		_bstr_t(sd.m_strStyleName.c_str()),
		_bstr_t(sd.m_strFont.c_str()),
		sd.m_iSize,
		sd.m_bBold,
		sd.m_bItalics,
		sd.m_bUnderlined,
		sd.m_clFore,
		sd.m_clBack);
}
//------------------------------------------------------------------------
void CSourceDocStyler::CDocumentStyleDescriptor::GetStatementIndent(OUT style_and_words & result)
{
	long nStartStyle, nEndStyle;
	_bstr_t strStartWords, strEndWords;

	m_ScriptProvider->GetStatementIndent(&nStartStyle,strStartWords.GetAddress(),&nEndStyle,strEndWords.GetAddress());
	result.SetData(nStartStyle,strStartWords);
}
//------------------------------------------------------------------------
void CSourceDocStyler::CDocumentStyleDescriptor::GetStatementEnd(OUT style_and_words & result)
{
	long nStartStyle, nEndStyle;
	_bstr_t strStartWords, strEndWords;

	m_ScriptProvider->GetStatementIndent(&nStartStyle,strStartWords.GetAddress(),&nEndStyle,strEndWords.GetAddress());
	result.SetData(nEndStyle,strEndWords);
}
//------------------------------------------------------------------------
void CSourceDocStyler::CDocumentStyleDescriptor::GetBlockStart(OUT style_and_words & result)
{
	long nStartStyle, nEndStyle;
	_bstr_t strStartWords, strEndWords;

	m_ScriptProvider->GetStatementBlock(&nStartStyle,strStartWords.GetAddress(),&nEndStyle,strEndWords.GetAddress());
	result.SetData(nStartStyle,strStartWords);
}
//------------------------------------------------------------------------
void CSourceDocStyler::CDocumentStyleDescriptor::GetBlockEnd(OUT style_and_words & result)
{
	long nStartStyle, nEndStyle;
	_bstr_t strStartWords, strEndWords;

	m_ScriptProvider->GetStatementBlock(&nStartStyle,strStartWords.GetAddress(),&nEndStyle,strEndWords.GetAddress());
	result.SetData(nEndStyle,strEndWords);
}
//------------------------------------------------------------------------
// COM
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_UseTabs (VARIANT_BOOL * Value)
{
	*Value = m_ScriptProvider->UseTabs;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_UseTabs (VARIANT_BOOL Value )
{
	m_ScriptProvider->UseTabs = Value;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_TabSize (long * Value )
{
	*Value = m_ScriptProvider->TabSize;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_TabSize (long Value )
{
	m_ScriptProvider->TabSize = Value;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_WordWrap (VARIANT_BOOL * Value )
{
	*Value = m_ScriptProvider->WordWrap;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_WordWrap (VARIANT_BOOL Value )
{
	m_ScriptProvider->WordWrap=Value;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_ColorChema (mtdev_lib::IDocumentColorChema * * Value )
{
	*Value = this;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_IndentSize (long * Value )
{
	*Value = m_ScriptProvider->IndentSize;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_IndentSize (long Value )
{
	m_ScriptProvider->IndentSize = Value;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_DocumentName (BSTR * Value )
{
	*Value = m_ScriptProvider->DocumentName.Detach();
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_Keywords(long ClassNo,BSTR * Value)
{
	_ASSERT(m_ScriptProvider.GetInterfacePtr());

	*Value = m_ScriptProvider->Keywords[ClassNo].Detach();
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_Keywords (long ClassNo,BSTR Value)
{
	m_ScriptProvider->Keywords[ClassNo] = Value;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_Extensions (BSTR * Value )
{
	*Value = m_ScriptProvider->Extensions.Detach();
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_Extensions (BSTR Value )
{
	m_ScriptProvider->Extensions = Value;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_Lexer (BSTR * Value )
{
	*Value = m_ScriptProvider->Lexer.Detach();
	return S_OK;
}

//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_Lexer (BSTR Value )
{
	m_ScriptProvider->Lexer = Value;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_FoldVisible (VARIANT_BOOL * Value)
{
	*Value = m_ScriptProvider->FoldVisible;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT CSourceDocStyler::CDocumentStyleDescriptor::get_FoldCode (VARIANT_BOOL * Value)
{
	*Value = m_ScriptProvider->FoldCode;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT CSourceDocStyler::CDocumentStyleDescriptor::put_FoldCode (VARIANT_BOOL Value)
{
	m_ScriptProvider->FoldCode=Value;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT CSourceDocStyler::CDocumentStyleDescriptor::get_FoldComment (VARIANT_BOOL * Value)
{
	*Value = m_ScriptProvider->FoldComment;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT CSourceDocStyler::CDocumentStyleDescriptor::put_FoldComment (VARIANT_BOOL Value)
{
	m_ScriptProvider->FoldComment=Value;
	return S_OK;
}
//------------------------------------------------------------------------
HRESULT CSourceDocStyler::CDocumentStyleDescriptor::get_StoragePath (BSTR * Value )
{
	*Value = m_ScriptProvider->DocumentPath.Detach();
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
//CSourceDocStyler::CDocumentStyleDescriptor::IDocumentColorChema
//////////////////////////////////////////////////////////////////////////
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_ItemCount (long * Value)
{
	*Value = m_ScriptProvider->StyleCount;
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_ForeColor (long Index,OLE_COLOR * Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=sd.m_clFore;
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_ForeColor (long Index,OLE_COLOR Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	if (sd.m_clFore!=Value){
		sd.m_clFore=Value;
		SetStyleDescByIdndex(Index,sd);
	}
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_BkColor (long Index,OLE_COLOR * Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=sd.m_clBack;
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_BkColor (long Index,OLE_COLOR Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	if (sd.m_clBack!=Value){
		sd.m_clBack=Value;
		SetStyleDescByIdndex(Index,sd);
	}
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_FontName (long Index,BSTR * Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=_bstr_t(sd.m_strFont.c_str()).Detach();
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_FontName (long Index,BSTR Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	if (!sd.m_strFont.cmpi(string_smart(Value))){
		sd.m_strFont=string_smart(Value);
		SetStyleDescByIdndex(Index,sd);
	}

	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_FontSize (long Index,long * Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=sd.m_iSize;
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_FontSize (long Index,long Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	if (sd.m_iSize!=Value){
		sd.m_iSize=Value;
		SetStyleDescByIdndex(Index,sd);
	}
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_IsBold (long Index,VARIANT_BOOL * Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=sd.m_bBold;
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_IsBold (long Index,VARIANT_BOOL Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	if (sd.m_bBold!=itob(Value)){
		sd.m_bBold=itob(Value);
		SetStyleDescByIdndex(Index,sd);
	}
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_IsItalic (long Index,VARIANT_BOOL * Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=sd.m_bItalics;
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_IsItalic (long Index,VARIANT_BOOL Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	if (sd.m_bItalics!=itob(Value)){
		sd.m_bItalics=itob(Value);
		SetStyleDescByIdndex(Index,sd);
	}
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_IsUnderline (long Index,VARIANT_BOOL * Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=sd.m_bUnderlined;
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::put_IsUnderline (long Index,VARIANT_BOOL Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	if (sd.m_bUnderlined!=itob(Value)){
		sd.m_bUnderlined=itob(Value);
		SetStyleDescByIdndex(Index,sd);
	}
	return S_OK;
}
HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_ItemName (long Index,BSTR * Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=_bstr_t(sd.m_strStyleName.c_str()).Detach();
	return S_OK;
}

HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_ItemVisible (long Index,VARIANT_BOOL * Value)
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=sd.m_bVisible;
	return S_OK;
}

HRESULT __stdcall CSourceDocStyler::CDocumentStyleDescriptor::get_ItemID (long Index,long * Value )
{
	ISourceDocStylerLink::style_definition sd;
	GetStyleDescByIndex(Index,sd);
	*Value=sd.m_nStyleID;
	return S_OK;
}