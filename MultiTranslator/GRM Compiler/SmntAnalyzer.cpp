/********************************************************************
	created:	2003/06/10
	created:	10:6:2003   20:49
	filename: 	$(Projects)\sources\grm compiler\smntanalyzer.cpp
	file path:	$(Projects)\sources\grm compiler
	file base:	smntanalyzer
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/
#include "StdAfx.h"
#include "smntanalyzer.h"
#include "EWQueue.h"

#ifdef __DIAGNOSTIC__
#define REPORT0(szText)			WriteToDiagnosticFile(szText,NULL);
#define REPORT1(szText,szA1)	WriteToDiagnosticFile(szText,szA1);
#else
#define REPORT0							
#define REPORT1(szText,szA1)			
#endif

//------------------------------------------------------------------------
CSmntAnalyzer::CSmntAnalyzer(const IBuildContext & BuildContext, cstr szFileName)
{
	string_smart strFileName (include_backslash(BuildContext.GetIntermediateDirectory()),extract_filename(szFileName),".smnt");
	remove(strFileName.c_str());

	#ifdef __DIAGNOSTIC__
	if (!m_DiagnosticFile.open(strFileName.c_str(),"wt"))
		errors::w_file_was_not_opened(strFileName.c_str());
	else
		WriteToDiagnosticFile("Building tree by syntax analyzer.\n\n",NULL);
	#endif
}
//------------------------------------------------------------------------
CSmntAnalyzer::~CSmntAnalyzer(void)
{
}
//------------------------------------------------------------------------
void CSmntAnalyzer::WriteToDiagnosticFile(cstr szText,...)
{
	string_smart strRes;
	va_list marker;
	va_start( marker, szText);     
	try{	
		strRes = FormatV(szText,marker);
	}
	catch(...){
		strRes = "<format error>";
	}
	va_end( marker );

	
	m_DiagnosticFile<<strRes;
	m_DiagnosticFile.flush();
}
//------------------------------------------------------------------------
bool CSmntAnalyzer::Analyze(const ILexTable & LexTable,IImportTable & ImportTable,const ILangGenTable & LangGenTable,
							ITypeTable & TypeTable, ISntxTable & SntxTable,
							const ILexList & LexList, ISntxList & SntxList, IPCodeList & PCodeList)
{
	_ASSERT(!errors::is_errors());
	m_Context.init(LexTable,ImportTable,LangGenTable,TypeTable,SntxTable,LexList,SntxList,PCodeList);
	LexList.SetPos(0);

	const structs::lexeme* CurLex=NULL;
	for	(int CurGEPointer=0;CurGEPointer<SntxList.GetCount();CurGEPointer++)
	{
		
		structs::grm_term & GEL=SntxList.GetAt(CurGEPointer);
		/////----------- TERMINAL----------- /////////			
		if (GEL.IsTerminal())
		{
			_ASSERT(!LexList.IsEnd());
			CurLex=&LexList.Get();	//Временно
			_ASSERT(CurLex->GetSzText());
			
			//Каждой константе нужно прицепить тип SType*
			if (CurLex->IsConstClass())
			{
				const variant & value = LexTable.GetConstant(CurLex->Index);
				const structs::type * pType;
				if (!TypeTable.FindStdType(structs::object::variant2object(value.get_type()),pType))
					_ASSERT(false);
				_ASSERT(CurLex->pType==NULL);
				const_cast<structs::lexeme*>(CurLex)->pType=const_cast<structs::type *>(pType);
			}

			REPORT1("Get TERMINAL %s\n", str_translate_control(CurLex->GetSzText()).c_str());
			_ASSERT(GEL.GetTerminal().GetClass()==CurLex->GetClass());
			if (GEL.GetTerminal().Index==NO_INDEX)
				_ASSERT(GEL.GetTerminal().IsConstClass() || GEL.GetTerminal().IsIDClass());
			else				
				_ASSERT(GEL.GetTerminal().Index==CurLex->Index);
		}

		/////----------- END OF	RULE ----------- /////////			

		else if	(GEL.IsTerminalEnd()) {
			REPORT1("Get END-OF-RULE <\"%s>\">\n",	GEL.m_szParentRuleName);
		}
		///// ---- EMPTY ELEMENT FUNCTIONS------
		else if	(GEL.IsTerminalEmpty())	{
			REPORT0("Get EMPTY-TERMINAL\n");
		}
		///// ---- NONTERMINAL FUNCTIONS	------
		else if	(GEL.IsNonterminal()) {
			REPORT1("Get NONTERMINAL  <\"%s>\">\n",GEL.GetLinkedRuleName());
		}
		else
			_ASSERT(0);

		if (GEL.m_pFunc!=NULL)
			m_Context.invoke(GEL.m_pFunc,*CurLex);

		if (errors::is_errors())
		{
			REPORT0("\t---!!! ERROR	!!!---\n");
			break;
		}

		if (GEL.IsTerminal())
			LexList.Next();
	} // end for

	m_DiagnosticFile.close();
	return !errors::is_errors();	
}
//------------------------------------------------------------------------
