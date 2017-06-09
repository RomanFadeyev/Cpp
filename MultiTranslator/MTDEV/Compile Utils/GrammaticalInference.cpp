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

#include "GrammaticalInference.h"
#include "Names.h"
#include "Resource.h"

using namespace i_objects;
//using namespace grmc_exports;
//Создание переходника для объекта
static i_objects::object_thunk<CGrammaticalInference> GrammaticalInference(__MSG_PREFIX__);

//////////////////////////////////////////////////////////////////////
// CGrammaticalInference class
//////////////////////////////////////////////////////////////////////
CGrammaticalInference::CGrammaticalInference()
{
}
//------------------------------------------------------------------------
CGrammaticalInference::~CGrammaticalInference() 
{
}
//------------------------------------------------------------------------
void CGrammaticalInference::OnParse(grmkrnl_exports::parse_info_packet & info)
{
	if (info.dwCmd==grmkrnl_exports::parse_info_packet::nfoFOUND)
		m_lstLexeme.push_back(gi_lexeme(info.szText,info.LexemeType,info.StandardTerminal,info.szFileName,info.dwLine,info.dwPos));
}
//------------------------------------------------------------------------
bool CGrammaticalInference::Run()
{
	//Запускаем мастер
	LOG_PROTECT_CODE_BEGIN
		mtdev_lib::IGrammaticalInferencePtr spGrammaticalInference(__uuidof(mtdev_lib::GrammaticalInference));
		if (spGrammaticalInference->Show()==FALSE)
			return false;
		return true;
	LOG_PROTECT_CODE_END


	/*
	dll_function<grmkrnl_exports::pf_GetGrmKrnlInterface> pLibGetGrmKrnlInterface;
	if (!pLibGetGrmKrnlInterface.attach(module_names::szGrmKrnlName,grmkrnl_exports::szf_GetGrmKrnlInterface)){
		LOG_ERROR(MSG_ERR_CannotLoadModule(module_names::szGrmKrnlName));
		return false;
	}
	
	//Скобки обязательны, чтобы локализовать GrmKrnlIface
	{
		ref_cnt_ptr<grmkrnl_exports::IGrmKrnl> GrmKrnlIface;
		GrmKrnlIface.reset((*pLibGetGrmKrnlInterface)());
		ASSERT_RETURN_FALSE(GrmKrnlIface!=NULL);

		ref_cnt_ptr<grmkrnl_exports::IGrammaticalInference> GrammaticalInferenceIface;
		GrammaticalInferenceIface.reset(GrmKrnlIface->GetGrammaticalInferenceInterface());
		ASSERT_RETURN_FALSE(GrammaticalInferenceIface!=NULL);

		//Запускаем мастер
		LOG_PROTECT_CODE_BEGIN
		mtdev_lib::IGrammaticalInferencePtr spGrammaticalInference(__uuidof(mtdev_lib::GrammaticalInference));
		if (spGrammaticalInference->Show()==FALSE)
			return false;

		GrmKrnlIface->SetParseCallBck(grmkrnl_exports::parse_info_functor(this,OnParse));

		GrmKrnlIface->IgnoreCase(spGrammaticalInference->IgnoreCase!=FALSE);
		GrmKrnlIface->Delimiters(spGrammaticalInference->Delimiters);
		string_list lstDelimiters(string_adapter(spGrammaticalInference->Comments));
		for (size_t i=0; i<lstDelimiters.items().count();i++){
			string_split s(lstDelimiters.items()[i],true,"|");
			string_smart sl = s.get_left_();
			string_smart sr = s.get_right_();
			if (sl.cmp("\\n")) sl="\n";
			if (sr.cmp("\\n")) sr="\n";
			GrmKrnlIface->IgnoreBlock(sl.c_str(),sr.c_str());
		}

		string_smart strFileName(spGrammaticalInference->FileName);
		if (strFileName.isempty()){ //Используем режим чтения из буфера
			strFileName=get_temp_filename(".data");
			save_file(strFileName,spGrammaticalInference->Text);
		}
		if (!GrmKrnlIface->OpenSource(strFileName.c_str())){
			guiMsgError(0,MSG_ERR_CannotLoadModule(module_names::szGrmKrnlName));
			return false;
		}

		m_lstLexeme.clear();
		force_dirs(GetInferenceDir());
		#ifndef _DEBUG
		try
		#endif
		{
			//Исходный текст
			string_smart strDumpFileName(GetInferenceDir(),extract_filenameext(strFileName).c_str(),".inference.source.dmp");
			CopyFile(strFileName.c_str(),strDumpFileName.c_str(),false);

			//Лексический анализатор
			string_smart strDumpFileName1(GetInferenceDir(),extract_filenameext(strFileName).c_str(),".inference.LA.dmp");
			GrammaticalInferenceIface->ParseLexical(strDumpFileName1.c_str());
			
			//Первая пробная структура текста
			string_smart strDumpFileName2(GetInferenceDir(),extract_filenameext(strFileName).c_str(),".inference.BSS.dmp");
			GrammaticalInferenceIface->BuildSyntaxStructure(strDumpFileName2.c_str());

			//статистика повторяемости слов (Statistic of word repetition)
			string_smart strDumpFileName3(GetInferenceDir(),extract_filenameext(strFileName).c_str(),".inference.SWR.dmp");
			GrammaticalInferenceIface->SearchWordRepetition(strDumpFileName3.c_str());
		}
		#ifndef _DEBUG
		catch (exception & e){
			guiMsgError(0,e.what());
		}
		catch (...) {
			guiMsgError(0,MSG_ERR_UnexpectedError().c_str());
		}
		#endif

		return true;
		LOG_PROTECT_CODE_END
	}*/
	return false;
}
