/********************************************************************
	created:	2002/03/22
	created:	22:3:2002   23:03
	filename: 	$(Projects)\Sources\MTDEv\KnowledgeBase.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	KnowledgeBase
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Grammatical Inference
*********************************************************************/

#pragma once

#include <CObjectEx.h>
#include <BaseStructs.h>

#include <GrmKrnlExports.h> //Здесь описаны прототипы функций ядра разбора

class CGrammaticalInference: public CCObjectEx, public IGrammaticalInferenceLink
{
protected:
	struct gi_lexeme {
		ELexemeCLass				m_nLexemeClass;
		EStdTerminalClass			m_nStdTerminalClass;
		string_std					m_strText;
		grm_structs::text_location	m_TextLocation;
		
		gi_lexeme (cstr szText, ELexemeCLass nLexemeClass, EStdTerminalClass nStdTerminalClass, 
			cstr szFileName, long nLine, long nPos): m_TextLocation(szFileName,nLine,nPos)
		{
			m_nLexemeClass = nLexemeClass;
			m_nStdTerminalClass = nStdTerminalClass;
			m_strText = szText;

		}
		void dump(base_utils::stream & s){
			s<<m_nLexemeClass<<" "<<grm_structs::GetStdTerminalClassName(m_nStdTerminalClass)<<"\t"<<m_strText.c_str()<<"\n";
		}
	};
	typedef list<gi_lexeme>	lexeme_list;
	lexeme_list	m_lstLexeme;

	void OnParse(grmkrnl_exports::parse_info_packet & info);
	
	override bool Run();
public:
	string_smart GetInferenceDir() const {
		return string_smart(ApplicationData::APP_DIR,"Grammatical Inference\\");
	}

	CGrammaticalInference();
	virtual ~CGrammaticalInference();
};
