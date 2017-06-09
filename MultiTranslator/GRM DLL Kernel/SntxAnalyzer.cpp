/********************************************************************
	created:	2003/04/01
	created:	1:4:2003   8:36
	filename: 	$(Projects)\sources\grm dll kernel\sntxanalyzer.cpp
	file path:	$(Projects)\sources\grm dll kernel
	file base:	sntxanalyzer
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Синтаксический анализатор универсального ядра разбора
*********************************************************************/

#include "stdafx.h"
#include <AppUtils.h>

#include "SntxAnalyzer.h"
#include "Resource.h"


cstr CSntxAnalyzer::szCurrentText=NULL;

//////////////////////////////////////////////////////////////////////
// CSntxVariant	Class
//////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////
// CSntxRule	Class
//////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
CSntxRule::CSntxRule():sntx_rule("")
{
	m_nSolitaryWords=VAL_AUTO;
	m_nIgnoreWhiteSpaces=VAL_AUTO;
	m_nFuncAddr=UNDEFINED_ADDR;
}
//------------------------------------------------------------------------
CSntxRule::CSntxRule(const CSntxRule &src):sntx_rule(src)
{
	*this=src;
}
//------------------------------------------------------------------------
CSntxRule::CSntxRule(cstr	szRuleName,	cstr szErrorText, cstr szFuncName, E3State nSolitaryWords, E3State nIgnoreWhiteSpaces)
	:sntx_rule(szRuleName)
{
	SetSolitaryWords(nSolitaryWords);
	SetIgnoreWhiteSpaces(nIgnoreWhiteSpaces);
	SetErrorText(szErrorText);

	m_strFuncName	= szFuncName;
	m_nFuncAddr=UNDEFINED_ADDR;
}
//------------------------------------------------------------------------
CSntxRule::~CSntxRule()
{
}
//------------------------------------------------------------------------
void CSntxRule::read_dmp(base_utils::stream & Stream)
{
	sntx_rule::read_dmp(Stream);
	/*1*/SetSolitaryWords((E3State)Stream.read_byte());		//SolitaryWords;
	/*1*/SetIgnoreWhiteSpaces((E3State)Stream.read_byte());	//IgnoreWhiteSpaces
	/*4*/LinkFunc(Stream.read_dword());						//Номер	функции
}
//------------------------------------------------------------------------
void CSntxRule::write_dmp(base_utils::stream	& Stream)
{
	sntx_rule::write_dmp(Stream);
	/*1*/Stream.write_byte(GetSolitaryWords());			//SolitaryWords;
	/*1*/Stream.write_byte(GetIgnoreWhiteSpaces());		//IgnoreWhiteSpaces
	/*4*/Stream.write_dword(GetFuncAddr());				//Номер	функции
}
//------------------------------------------------------------------------
size_t CSntxRule::ReplaceNameLink(CSntxRule & aRule, size_t nRuleNumber)
{
	size_t Replaces=0;
	
	for	(size_t	j=0;j<GetVariantCount();j++)
		for (size_t k = 0; k<GetVariant(j).GetTermCount();k++)
		{
			grmkrnl_structs::grm_symbol & GE = GetVariant(j).GetTerm(k);
			if ((GE.IsNonterminalLinkByName()) &&  (strcmp(aRule.GetName(),GE.GetLinkedRuleName())==0))
			{
				GE.LinkToRuleAddr(nRuleNumber); //Это чтобы сохранить можно было в файле
				GE.LinkToRule(aRule); //Это как положено
				Replaces++;
			}
		}

	return Replaces;
}
//------------------------------------------------------------------------
void CSntxRule::AddVariant(CSntxVariant	& src)
{
	src.AddTerm(grm_symbol( src.GetFuncName(),	//Этот параметр	дожен быть обязательно
						 (src.IsHasPreFunc()) ? src.GetPreFuncName()	: NULL));
	
	sntx_rule::AddVariant(src);
}
//------------------------------------------------------------------------
CSntxRule & CSntxRule::operator=(const CSntxRule& src)
{
	sntx_rule::operator = (src);

	this->m_nFuncAddr=src.m_nFuncAddr;
	this->m_strFuncName=src.m_strFuncName;

	SetIgnoreWhiteSpaces(src.GetIgnoreWhiteSpaces());
	SetSolitaryWords(src.GetSolitaryWords());
	return *this;
} 
//------------------------------------------------------------------------
void CSntxRule::SendParsePacket(grm_parse_packet & Msg,sntx_analyze_context & Context)
{
	_ASSERT(Context.m_ParseCallBckFunc);
	Msg.szCurHubName=this->GetName();
	grm_structs::text_location Location = Context.GetLexList().GetCurrentLocation();

	Msg.dwRecursionLevel=Context.m_nRecursionLevel;
	Msg.szFileName = Location.m_strFileName.c_str();
	Msg.dwLine = Location.GetPos().m_nLine;
	Msg.dwPos = Location.GetPos().m_nCol;

	string_smart strTextAhead = Context.GetLexList().GetTextAhead(128);
	string_smart strTextBehind = Context.GetLexList().GetTextBehind(128);
	Msg.szTextAhead=strTextAhead.c_str();
	Msg.szTextBehind=strTextBehind.c_str();
	
	Context.m_ParseCallBckFunc(Msg);
}
//------------------------------------------------------------------------
void CSntxRule::ReportEnterToRule(sntx_analyze_context & Context)
{
	if (Context.m_ParseCallBckFunc)
	{
		static string_res strText(IDS_REPORT_ENTER_TO_RULE,ApplicationData::APP_INSTANCE);
		static string_smart strFileName;
		
		grm_parse_packet Msg;
		Msg.Type=GE_NONTERMINAL;
		Msg.szText=this->GetName();
		Msg.dwCmd=grmkrnl_exports::parse_info_packet::nfoENTER;
		Msg.szMessage=strText.c_str();

		SendParsePacket(Msg,Context);
	}
}
//------------------------------------------------------------------------
void CSntxRule::ReportLeaveRule(sntx_analyze_context & Context)
{
	if (Context.m_ParseCallBckFunc)
	{
		static string_res strText(IDS_REPORT_LEAVE_RULE,ApplicationData::APP_INSTANCE);

		grm_parse_packet Msg;
		Msg.Type=GE_NONTERMINAL;
		Msg.szText=this->GetName();
		Msg.dwCmd=grmkrnl_exports::parse_info_packet::nfoEXIT_TRUE;
		Msg.szMessage=strText.c_str();

		SendParsePacket(Msg,Context);
	}
}
//------------------------------------------------------------------------
void CSntxRule::ReportRollbackFromRule(cstr szText, sntx_analyze_context & Context)
{
	if (Context.m_ParseCallBckFunc)
	{
		static string_res strText(IDS_REPORT_ROLLBACK_RULE,ApplicationData::APP_INSTANCE);

		grm_parse_packet Msg;
		Msg.Type=GE_NONTERMINAL;
		Msg.szText=szText;
		Msg.dwCmd=grmkrnl_exports::parse_info_packet::nfoROLLBACK;
		Msg.szMessage=strText.c_str();

		SendParsePacket(Msg,Context);
	}
}
//------------------------------------------------------------------------
void CSntxRule::ReportFoundTerm(const grm_structs::text_pos & pos,sntx_analyze_context & Context)
{
	if (Context.m_ParseCallBckFunc)
	{
		static string_res strText1(IDS_REPORT_FOUND_TERMINAL,ApplicationData::APP_INSTANCE);
		static string_res strText2(IDS_REPORT_FOUND_STD_TERMINAL,ApplicationData::APP_INSTANCE);

		grm_parse_packet Msg;
		Msg=Context.GetSntxList().back();
		
		Msg.dwCmd=grmkrnl_exports::parse_info_packet::nfoFOUND;
		
		switch(Msg.Type) {
		case GE_TERMINAL:
			Msg.szMessage=strText1.c_str();
			break;
		case GE_STANDARD_TERMINAL:
			Msg.szMessage=strText2.c_str();
			break;
		case GE_REGEXP_TERMINAL:
			Msg.szMessage=strText2.c_str();
			break;
		default:
			_ASSERT(false);
		}

		SendParsePacket(Msg,Context);
		//Msg.dwLine=pos.m_nLine;
		//Msg.dwPos=pos.m_nCol;
	}
}
//------------------------------------------------------------------------
void CSntxRule::ReportSearchAlternative(cstr szText, sntx_analyze_context & Context)
{
	if (Context.m_ParseCallBckFunc)
	{
		static string_res strText(IDS_REPORT_SEARCH_ALTERNATIVE,ApplicationData::APP_INSTANCE);

		grm_parse_packet Msg;
		Msg.Type=GE_NONTERMINAL;
		Msg.szText=szText;
		Msg.dwCmd=grmkrnl_exports::parse_info_packet::nfoNEXT_VARIANT;
		Msg.szMessage=strText.c_str();

		SendParsePacket(Msg,Context);
	}
}
//------------------------------------------------------------------------
void CSntxRule::AnalyzeError(const grmkrnl_structs::grm_symbol & CurGE, sntx_analyze_context & Context)
{
	static string_std strEmpty;
	const CLexSource & LexList=Context.GetLexList();

	grm_structs::text_pos pos(LexList.GetCurrentPosition().m_nLine,LexList.GetCurrentPosition().m_nCol);
	
	if (pos<Context.m_ConjecturalSyntaxErrorLocation.GetPos())
		return;

	//if ((pos==Context.m_ConjecturalSyntaxError.GetPos()) && (Context.m_nRecursionLevel<<Context.m_nConjecturalSyntaxErrorLevel))
	//	return;

	//if we have just found the error we have to clear the set
	if  (pos>Context.m_ConjecturalSyntaxErrorLocation.GetPos()) {
		Context.m_ConjecturalErrorRules.clear();
		Context.m_ConjecturalErrorTerms.clear();
	}

	//adding the suspicious data into set
	Context.m_ConjecturalErrorRules.push_back(this);
	Context.m_ConjecturalErrorTerms.push_back(&CurGE);

	//Context.m_ConjecturalSyntaxError.Clear();
	//if (!CurGE.GetError().IsError())
	//	Context.m_ConjecturalSyntaxError=GetError();
	//else 
	//	Context.m_ConjecturalSyntaxError=CurGE.GetError();

	Context.m_nConjecturalSyntaxErrorLevel=Context.m_nRecursionLevel;
	Context.m_ConjecturalSyntaxErrorLocation.SetPos(pos);

	//grm_structs::text_container	aObject;
	//aObject.set(grm_structs::text_start_end_location(strEmpty,pos,pos));
	//Context.m_ConjecturalSyntaxError.SetArg(aObject); 
	//Context.m_pConjecturalErrorRule=this;
}
//------------------------------------------------------------------------
bool CSntxRule::AnalyzeStructure(sntx_analyze_context & Context)
{
	CSntxList& SntxList	=Context.GetSntxList();
	CLexList & LexList  =Context.GetLexList();
	//CSntxAnalyzer & Analyzer = Context.GetAnalyzer();

	
	#define	ABORT_ANALYZE {Context.m_nRecursionLevel--; return	false;}

	Context.m_nRecursionLevel++;
	if (Context.m_nRecursionLevel>65535)
	{
		Context.m_bFatalError=true;
		::MessageBox(0,FormatRes(IDS_LEFT_RECURSION_s,ApplicationData::APP_INSTANCE,GetName()).c_str(),NULL,MB_ICONERROR);
		ABORT_ANALYZE 
	}

	ReportEnterToRule(Context);

	grm_structs::text_pos CurTextPos(LexList.GetCurLine(),LexList.GetCurCol());
	CLexSource::src_pos StorePos=LexList.GetCurrentPos();

	size_t	nResultChainLen=SntxList.size();

	bool oldm_nSolitaryWords;
	bool oldIgnoreWhiteSpaces;
	if (m_nSolitaryWords!=VAL_AUTO)
	{
		oldm_nSolitaryWords=LexList.GetSolitaryWords();
		LexList.SetSolitaryWords(m_nSolitaryWords!=0);
	}
	if (m_nIgnoreWhiteSpaces!=VAL_AUTO)
	{
		oldIgnoreWhiteSpaces=LexList.GetIgnoreWhiteSpaces();
		LexList.SetIgnoreWhiteSpaces(m_nIgnoreWhiteSpaces!=0);
	}


	cstr	szNotFound;
	bool	Success=true;

	size_t nCurVariant=0;

SEARCH:
	_ASSERT(nCurVariant<GetVariantCount());
	sntx_variant * CurVariant=&GetVariant(nCurVariant); // выбираем первый вариант
	CSntxVariant::iterator CurNode=CurVariant->Begin();

	if (CurVariant->GetLastTerm().IsPreFuncLinked())	// PreSub for variant
	{
		SntxList.push_back(CurVariant->GetLastTerm());
		SntxList.back().ClearFunc();
	}

	while  (true)
	{
		for	(;CurNode!=CurVariant->End(); ++CurNode)
		{
			grm_symbol & CurGE = (*CurNode);
			if (CurGE.IsTerminalEnd()) //	это	уже	терминатор - пора заканчивать
			{
				//_ASSERT(!CurNode->Next);
				SntxList.push_back(CurGE);
				SntxList.back().ClearPreFunc(); //	m_nPreFuncAddr is already in chain.	See	up
				// Дополнительно цепляем функцию узла
				if (this->IsFuncLinked())
				{
					SntxList.push_back(CurGE);
					SntxList.back().ClearPreFunc(); 
					SntxList.back().LinkFunc(this->m_nFuncAddr);
				}
				++CurNode;
				_ASSERT(CurNode==CurVariant->End());
				break;
			}

			/////----------- TERMINAL----------- /////////	

			if (CurGE.IsTerminal())
			{
				if (LexList.GetAndCompare(CurGE.GetTerminal().GetSzText()))
				{
					SntxList.push_back(CurGE);
					SntxList.back().GetTerminal().SetClass(CurGE.GetTerminal().GetClass());
					SntxList.back().GetTerminal().SetText(CurGE.GetTerminal().Get_Str_Text());

					ReportFoundTerm(CurTextPos,Context);
				}
				else  if (!CurGE.IsOptional())
				{
					AnalyzeError(CurGE,Context);
					szNotFound=CurGE.GetTerminal().GetSzText();
					break;
				}
				else
				{
					for (CSntxVariant::iterator NextNode = CurNode+1;(NextNode!=CurVariant->End()) && (*NextNode).IsChildOptional(); ++NextNode)
						++CurNode;
				}
			}
			/////----------- STANDARD TERMINAL --- /////////
			else if (CurGE.IsTerminalStd())		
			{
				if ((LexList.GetStandardTerminal(CurGE.GetStdTerminalClass())) &&
					((CurGE.GetStdTerminalClass()!=STD_ID) || (!Context.IsKeyword(LexList.GetTerminalText())))) 
				{
					SntxList.push_back(CurGE);
					//SntxList.back().GetTerminal().SetClass(CurGE.GetStdTerminalClass());
					//SntxList.back().GetTerminal().SetText(CurGE.GetTerminal().Get_Str_Text());
					SntxList.back().SetText(LexList.GetTerminalText());

					ReportFoundTerm(CurTextPos,Context);
		//			continue;
				}
				else if	(!CurGE.IsOptional())
				{
					AnalyzeError(CurGE,Context);
					szNotFound="Standard Terminal";
					break;
				}
				else 
				{
					for (CSntxVariant::iterator NextNode = CurNode+1;(NextNode!=CurVariant->End()) && (*NextNode).IsChildOptional(); ++NextNode)
						++CurNode;
				}


				//Reset accumulated set of errors
				grm_structs::text_pos pos(LexList.GetCurrentPosition().m_nLine,LexList.GetCurrentPosition().m_nCol);
				if (pos>Context.m_ConjecturalSyntaxErrorLocation.GetPos()){
					Context.m_ConjecturalErrorTerms.clear();
					Context.m_ConjecturalErrorRules.clear();
				}
			}
			/////-------- REGULAR EXPRESSION ------ /////////
			else if (CurGE.IsTerminalRegExp())
			{
				if (LexList.GetRegExpTerminal(CurGE.GetRegExp()))
				{
					SntxList.push_back(CurGE);
					//SntxList.back().GetTerminal().SetClass(CurGE.GetTerminal().GetClass());
					//SntxList.back().GetTerminal().SetText(CurGE.GetTerminal().Get_Str_Text());
					SntxList.back().SetText(LexList.GetTerminalText());
					ReportFoundTerm(CurTextPos,Context);
				}
				else  if (!CurGE.IsOptional())
				{
					AnalyzeError(CurGE,Context);
					szNotFound=CurGE.GetRegExp();
					break;
				}
				else
				{
					for (CSntxVariant::iterator NextNode = CurNode+1;(NextNode!=CurVariant->End()) && (*NextNode).IsChildOptional(); ++NextNode)
						++CurNode;
				}
			}
			/////----------- NONTERMINAL----------- /////////
			else if (CurGE.IsNonterminal())
			{
				CLexSource::src_pos LexPos=LexList.GetCurrentPos();
#ifdef _DEBUG
				string_smart Name=CurGE.GetLinkedRule().GetName();
#endif
				if (CurGE.IsPreFuncLinked())
				{
					SntxList.push_back(CurGE); //	add	prefunction
					SntxList.back().ClearFunc();
				}
				
				if (CurGE.GetLinkedRule().AnalyzeStructure(Context)!=false)
				{
					if (CurGE.IsFuncLinked())
					{
						SntxList.push_back(CurGE);
						SntxList.back().ClearPreFunc();	// m_nPreFuncAddr is already in	chain
					}
				}
				else 
				{
					_ASSERT(LexPos==LexList.GetCurrentPos());
					_ASSERT(CurGE==*CurNode);
					szNotFound=CurGE.GetLinkedRule().GetName();

					if (CurGE.IsPreFuncLinked())
						SntxList.pop_back();
				
					if (Context.m_bFatalError) 
						ABORT_ANALYZE
				
					if (!CurGE.IsOptional())
						break;
					else
					{
						for (CSntxVariant::iterator NextNode = CurNode+1;(NextNode!=CurVariant->End()) && (*NextNode).IsChildOptional(); ++NextNode)
							++CurNode;
					}
				}
			}
			else
				_ASSERT(0);
		}

		if (CurNode!=CurVariant->End()) //	где	- то была ошибка
		{
			LexList.SetCurrentPos(StorePos);
			while (SntxList.size()>nResultChainLen) 
				SntxList.pop_back();

			if (nCurVariant<GetVariantCount()-1)
			{	
				nCurVariant++;
				ReportSearchAlternative(szNotFound,Context);
				goto SEARCH;
			}
			else
			{
				Success=false;
				break;
			}
		}
		else
		{
			CurVariant->IncStatistic();
			Success=true;
			break;
		}
	}

	if (m_nSolitaryWords!=VAL_AUTO)
		LexList.SetSolitaryWords(oldm_nSolitaryWords);
	if (m_nIgnoreWhiteSpaces!=VAL_AUTO)
		LexList.SetIgnoreWhiteSpaces(oldIgnoreWhiteSpaces);

	if (Success==true)
		ReportLeaveRule(Context);
	else {
		_ASSERT(nResultChainLen==SntxList.size());
		ReportRollbackFromRule(szNotFound,Context);
	}
		
	Context.m_nRecursionLevel--;
	return Success;
}

//-------------------------------------------------------------------------
bool CSntxRule::SaveUsingStatistic(stream & Stream)
{
	try
	{
		Stream<<(DWORD )strlen(GetName());
		Stream<<(cstr)GetName();
		Stream<<GetVariantCount();
		for	(size_t	i=0; i<GetVariantCount(); i++)
		{
			Stream<<(DWORD)	GetVariant(i).GetStatistic();
		}
	}
	catch (...)
	{
		return false;  
	}

	return true;	
}

//////////////////////////////////////////////////////////////////////////
// CSntxAnalyzer
//////////////////////////////////////////////////////////////////////////
CSntxAnalyzer::CSntxAnalyzer():	m_pSubRoutineHandler(NULL)
{

}
//------------------------------------------------------------------------
CSntxAnalyzer::~CSntxAnalyzer()
{
	Clear();
}
//------------------------------------------------------------------------
void CSntxAnalyzer::GatherInfo(stream & Stream)
{
	Stream.write_strln(strnchar('=',80)); 
	Stream.write_strln("Grammar");
	Stream.write_strln(strnchar('=',80));
	for	(size_t	i=0;i<GetRuleCount();i++)
	{
		CSntxRule& aRule = GetRule(i);
		Stream<<string_smart(GetRule(i).GetName(),": ");

		for (size_t i = 0;i<aRule.GetVariantCount();++i)
		{
			grmkrnl_structs::sntx_variant & aVariant = aRule.GetVariant(i);
			for (size_t j = 0;j<aVariant.GetTermCount();++j)
			{
				 grmkrnl_structs::grm_symbol & aSymbol = aVariant.GetTerm(j);
				 if (oneof(aSymbol.GetClass(),GE_NULL,GE_END))
					 continue;
				
				 if (aSymbol.IsOptional()||aSymbol.IsChildOptional())
					 Stream<<"[";
				 if (aSymbol.IsNonterminal())
					 Stream<<"<";
				 Stream<<string_smart("\"",aSymbol.GetTextRepresentation(),"\"");
				 if (aSymbol.IsNonterminal())
					 Stream<<">";
				 if (aSymbol.IsOptional()||aSymbol.IsChildOptional())
					 Stream<<"]";
				 if (aSymbol.IsChildOptional())
					 Stream<<"*";
			}
			if (i<aRule.GetVariantCount()-1)
				Stream<<"|";
		}
		Stream<<"\n\n";
	}

	Stream.write_strln(strnchar('=',80));
	Stream.write_strln("Terminal set");
	Stream.write_strln(strnchar('=',80));

	for	(size_t	i=0;i<GetRuleCount();i++)
	{
		sntx_rule_t::list_symbol_iterate_results lstSymbols;
		GetRule(i).GetTerminalsSet(lstSymbols);
		
		Stream<<string_smart(GetRule(i).GetName(),": ");
		for (sntx_rule_t::list_symbol_iterate_results::iterator i = lstSymbols.begin();i!=lstSymbols.end();++i)
			Stream<<"\""<<(*i).m_Symbol.GetTextRepresentation()<<"\":"<<itos((*i).m_nDeep)<<" ";
		Stream<<"\n\n";
	}

	Stream.write_strln(strnchar('=',80));
	Stream.write_strln("Nonterminal set");
	Stream.write_strln(strnchar('=',80));
	for	(size_t	i=0;i<GetRuleCount();i++)
	{
		sntx_rule_t::list_symbol_iterate_results lstSymbols;
		GetRule(i).GetNonterminalsSet(lstSymbols);
		
		Stream<<string_smart(GetRule(i).GetName(),": ");
		for (sntx_rule_t::list_symbol_iterate_results::iterator i = lstSymbols.begin();i!=lstSymbols.end();++i)
			Stream<<"\""<<(*i).m_Symbol.GetTextRepresentation()<<"\":"<<itos((*i).m_nDeep)<<" ";
		Stream<<"\n\n";
	}
	
	Stream.write_strln(strnchar('=',80));
	Stream.write_strln("front terminal set");
	Stream.write_strln(strnchar('=',80));
	for	(size_t	i=0;i<GetRuleCount();i++)
	{
		sntx_rule_t::list_symbol_iterate_results lstSymbols;
		GetRule(i).GetFrontTerminals(lstSymbols);
		
		Stream<<string_smart(GetRule(i).GetName(),": ");
		for (sntx_rule_t::list_symbol_iterate_results::iterator i = lstSymbols.begin();i!=lstSymbols.end();++i)
			Stream<<"\""<<(*i).m_Symbol.GetTextRepresentation()<<"\":"<<itos((*i).m_nDeep)<<" ";
		Stream<<"\n\n";
	}
}
//------------------------------------------------------------------------
bool CSntxAnalyzer::Rebuild()
{
	for	(size_t	i=0;i<GetRuleCount();i++)
		for	(size_t	j=0;j<GetRuleCount();j++)
			GetRule(j).ReplaceNameLink(GetRule(i),i);

	return true;
}
//------------------------------------------------------------------------
bool CSntxAnalyzer::Analyze(CLexSource & LexList, grmkrnl_exports::pSubRoutineHandlerFunc pFunc, size_t nStartRule)
{
	m_AnalyzeContext.Clear();
	m_AnalyzeContext.m_pAnalyzer=this;
	m_AnalyzeContext.m_pLexList=&LexList;
	m_AnalyzeContext.m_ResultChain.clear();
	m_AnalyzeContext.SetIgnoreCase(LexList.GetIgnoreCase());
	

	m_pSubRoutineHandler = pFunc;
	m_nstartClock = clock();

	//Link All rules to each other
	for (size_t i=0;i<GetRuleCount();i++)
		for (size_t j=0;j<GetRule(i).GetVariantCount();j++)
			for (size_t k = 0; k<GetRule(i).GetVariant(j).GetTermCount();k++)
			{
				grmkrnl_structs::grm_symbol & GE = GetRule(i).GetVariant(j).GetTerm(k);
				if (GE.IsNonterminal())
					GE.LinkToRule(GetRule(GE.GetLinkedRuleAddr()));
				else if (GE.IsTerminal())
					m_AnalyzeContext.AddKeyword(GE.GetTerminal().GetSzText());
			}

	SetRoot(&GetRule(nStartRule));
	return GetRoot().AnalyzeStructure(m_AnalyzeContext);
	//m_AnalyzeContext.WriteToDiagnosticFile("Time:%g",(double(clock()-m_nstartClock))/CLOCKS_PER_SEC);
}
//------------------------------------------------------------------------
void CSntxAnalyzer::StopAnalyze()
{
	m_AnalyzeContext.m_bStopComplete=true;
}
//------------------------------------------------------------------------
string_list CSntxAnalyzer::GetErrorRuleName() const	
{
	_ASSERT(IsSyntaxError()); 
	string_list s;
	for (sntx_analyze_context::list_rules::const_iterator i =m_AnalyzeContext.m_ConjecturalErrorRules.begin(); 
		i!=m_AnalyzeContext.m_ConjecturalErrorRules.end(); ++i){
		s.items().add((*i)->GetName());
	}
	
	s.items().remove_coincidences();
	return s;
}
//------------------------------------------------------------------------
const CSntxAnalyzer::sntx_error CSntxAnalyzer::GetSyntaxError()
{
	_ASSERT(IsSyntaxError()); 
	sntx_error err;
	grm_structs::text_container	aObject;
	aObject.set(grm_structs::text_start_end_location("",
				m_AnalyzeContext.m_ConjecturalSyntaxErrorLocation.GetPos(),
				m_AnalyzeContext.m_ConjecturalSyntaxErrorLocation.GetPos()));
	err.SetArg(aObject); 
	
	//Context.m_pConjecturalErrorRule=this;

	string_list lstNames;
	//Попытамся собрать сообщение об ошибке
	for (sntx_analyze_context::list_symbols::const_iterator i =m_AnalyzeContext.m_ConjecturalErrorTerms.begin(); 
		i!=m_AnalyzeContext.m_ConjecturalErrorTerms.end(); ++i)
	{
		lstNames.items().add((*i)->GetTextRepresentation());
	}
	
	lstNames.items().remove_coincidences(false);
	if (lstNames.items().count()!=0) 
	{
		string_smart txt("Expected \"",lstNames.items()[0],"\"");
		for (size_t i=1; i<lstNames.items().count();i++)
			txt.append(" or \"",lstNames.items()[i],"\"",NULL);
		
		err.SetName(txt.c_str());
	}
	else
		err.SetName("Syntax error");

	return err;
}
//------------------------------------------------------------------------
bool CSntxAnalyzer::CompleteChain()
{
	szCurrentText=NULL;

	CSntxList& SntxList	=m_AnalyzeContext.GetSntxList();
	_ASSERT(m_pSubRoutineHandler);
	for	(CSntxList::iterator CurGEPointer=SntxList.begin();CurGEPointer!=SntxList.end();++CurGEPointer)
	{
		if ((*CurGEPointer).IsPreFuncLinked())
		{
			m_pSubRoutineHandler((*CurGEPointer).GetPreFuncAddr());
			if (m_AnalyzeContext.m_bStopComplete)
				return false;
		}

		/////----------- TERMINAL----------- /////////			

		if ((*CurGEPointer).IsTerminal()) 
		{
			szCurrentText=(*CurGEPointer).GetTerminal().GetSzText();

			if ((*CurGEPointer).IsFuncLinked())
				m_pSubRoutineHandler((*CurGEPointer).GetFuncAddr());
			if (m_AnalyzeContext.m_bStopComplete)
				return false;
		}

		/////----------- STANDARD TERMINAL----------- /////////			

		else if	 ((*CurGEPointer).IsTerminalStd())
		{
			szCurrentText=(*CurGEPointer).GetSzText();
			if ((*CurGEPointer).IsFuncLinked())
				m_pSubRoutineHandler((*CurGEPointer).GetFuncAddr());
			if (m_AnalyzeContext.m_bStopComplete)
				return false;
		}

		/////-------- REGULAR EXPRESSION ------ /////////
		else if	 ((*CurGEPointer).IsTerminalRegExp())
		{
			szCurrentText=(*CurGEPointer).GetSzText();
			if ((*CurGEPointer).IsFuncLinked())
				m_pSubRoutineHandler((*CurGEPointer).GetFuncAddr());
			if (m_AnalyzeContext.m_bStopComplete)
				return false;
		}

		/////----------- END OF	RULE ----------- /////////			
		else if	((*CurGEPointer).IsTerminalEnd())
		{
			if ((*CurGEPointer).IsFuncLinked())
				m_pSubRoutineHandler((*CurGEPointer).GetFuncAddr()); 
			if (m_AnalyzeContext.m_bStopComplete)
				return false;
		}

		///// ---- NONTERMINAL FUNCTIONS	------
		else if	((*CurGEPointer).IsNonterminal())
		{
			if ((*CurGEPointer).IsFuncLinked())
				m_pSubRoutineHandler((*CurGEPointer).GetFuncAddr());
			if (m_AnalyzeContext.m_bStopComplete)
				return false;
		}
		else
			_ASSERT(0);

		//Мониторинг процесса генерации извне
		if (m_AnalyzeContext.m_GenCallBckFunc)
		{
			grmkrnl_exports::gen_info_packet data;
			data.szText=cstr_safe(szCurrentText);
			data.dwItemID=(*CurGEPointer).GetID();
			m_AnalyzeContext.m_GenCallBckFunc(data);
		}

	} 
	return true;
}
