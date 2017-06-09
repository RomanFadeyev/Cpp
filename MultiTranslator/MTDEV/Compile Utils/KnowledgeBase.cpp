/********************************************************************
	created:	2002/03/23
	created:	23:3:2002   0:05
	filename: 	$(Projects)\Sources\MTDEv\KnowledgeBase.cpp
	file path:	$(Projects)\Sources\MTDEv

	file base:	KnowledgeBase
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Knowledge base management
*********************************************************************/

#include "stdafx.h"
#include <ModuleNames.h>
#include <GUIService.h>

#include "KnowledgeBase.h"
#include "Names.h"
#include "Resource.h"
 
using namespace i_objects;
//using namespace grmc_exports;
//Создание переходника для объекта
static i_objects::object_thunk<CKnowledgeBase> KnowledgeBase(__MSG_PREFIX__);

//////////////////////////////////////////////////////////////////////
// CKnowledgeBase class
//////////////////////////////////////////////////////////////////////
CKnowledgeBase::CKnowledgeBase()
{
}
//------------------------------------------------------------------------
CKnowledgeBase::~CKnowledgeBase() 
{
}
//------------------------------------------------------------------------
bool CKnowledgeBase::Add (IProjectDocLink * pProjectLink)
{
	_ASSERT(pProjectLink);
	string_smart strSrcLang = pProjectLink->GetSourceLanguage();
	string_smart strTargetLang = pProjectLink->GetTargetLanguage();

	LOG_PROTECT_CODE_BEGIN
	mtdev_lib::IAddToKnowledgeBasePtr spAddToKnowledgeBasePtr(__uuidof(mtdev_lib::AddToKnowledgeBase));
	
	IParserLink::syntax_info SI; 
	singleton_cache::GetParserLink()->ParseWholeProject(*pProjectLink,SI);
	
	string_smart strXBNF;
	singleton_cache::GetParserLink()->AssembleToXBNF(SI,strXBNF);
	string_smart strGrammarInfo; 
	if (!load_file(pProjectLink->GetCompilerInfoFilename(),strGrammarInfo)){
		guiMsgError(0,FormatRes(IDS_CANT_READ_FILE_s,HRESOURCES,pProjectLink->GetCompilerInfoFilename().c_str()));
		return false;
	}

	spAddToKnowledgeBasePtr->SourceLanguage=strSrcLang.c_str();
	spAddToKnowledgeBasePtr->TargetLanguage=strTargetLang.c_str();
	spAddToKnowledgeBasePtr->ModulePath = pProjectLink->GetExePath().c_str();
	spAddToKnowledgeBasePtr->ModuleSourceCode = ""; //Как получить весь исходный код? Он же может быть из нескольких файлов
	spAddToKnowledgeBasePtr->ModuleXBNF = strXBNF.c_str();
	spAddToKnowledgeBasePtr->GrammarInfo = strGrammarInfo.c_str();

	//запускаем форму
	if 	(spAddToKnowledgeBasePtr->Show()==FALSE)
		return false;

	strSrcLang = spAddToKnowledgeBasePtr->SourceLanguage;
	strTargetLang = spAddToKnowledgeBasePtr->TargetLanguage;
	pProjectLink->SetSourceLanguage(strSrcLang.c_str());
	pProjectLink->SetTargetLanguage(strTargetLang.c_str());

	return true;
	LOG_PROTECT_CODE_END
	return false;
}
//------------------------------------------------------------------------
bool CKnowledgeBase::ShowRegistry()
{
	LOG_PROTECT_CODE_BEGIN
	mtdev_lib::IViewKnowledgeBasePtr spViewKnowledgeBasePtr(__uuidof(mtdev_lib::ViewKnowledgeBase));
	if (spViewKnowledgeBasePtr->Show()==FALSE)
		return false;

	return true;
	LOG_PROTECT_CODE_END
	return false;
}