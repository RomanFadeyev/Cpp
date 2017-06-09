/********************************************************************
	created:	2004/01/26
	created:	26:1:2004   16:16
	filename: 	$(Projects)\sources\grm compiler\syntax analysis\sntxanalyzer.cpp
	file path:	$(Projects)\sources\grm compiler\syntax analysis
	file base:	sntxanalyzer
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Implementation of three basic syntax classes: variant, rule, analyzer
				class term was defined early, in BaseStructs.h
*********************************************************************/

#include "stdafx.h"
#include "SntxAnalyzer.h"
#include "EWQueue.h"

string_smart CSntxAnalyzer::m_strCurFileName;

//------------------------------------------------------------------------
CSntxAnalyzer::CSntxAnalyzer(const IBuildContext & BuildContext, cstr szFileName)
{
	m_strCurFileName = szFileName;

	string_smart strFileName (include_backslash(BuildContext.GetIntermediateDirectory()),extract_filename(szFileName),".sntx");
	remove(strFileName.c_str());

	#ifdef __DIAGNOSTIC__
	if (!m_AnalyzeContext.OpenDiagnosticFile(strFileName.c_str()))
		errors::w_file_was_not_opened(strFileName.c_str());
	else
		m_AnalyzeContext.WriteToDiagnosticFile("Build tree of syntax analyzer.\n\n",NULL);
	#endif
}
//------------------------------------------------------------------------
CSntxAnalyzer::~CSntxAnalyzer()
{
	m_AnalyzeContext.CloseDiagnosticFile();
	Clear();
}
//------------------------------------------------------------------------
string_smart CSntxAnalyzer::GetTreeDump() const
{
	string_list s,s2;
	for (vector_sntx_rules_cit i=m_arrRules.begin();i!=m_arrRules.end();++i){
		s.items().add(string_smart("---",(*i)->GetName(),"---").c_str());
		s.items().add((*i)->GetTreeDump().c_str());
		s.items().add("\n");
		for (CSntxRule::tree_gramms_tit it=(*i)->GetTree().begin(); it!=(*i)->GetTree().end(); ++it){
			structs::grm_term & T = (*it).get_val();
			if (T.IsForbidBacktrack())
				s2.items().add(T.GetTextRepresentation());
		}
	}

	return string_smart(s,"\n--------------- NobackTrack Terminals ----------------\n",s2);
}
//------------------------------------------------------------------------
string_smart CSntxAnalyzer::GetXBNF()
{
	string_list s;
	for (vector_sntx_rules_cit i=m_arrRules.begin();i!=m_arrRules.end();++i){
		s.items().add((*i)->GetXBNF().c_str());
		s.items().add("");
	}
	return s.c_str();
}
//------------------------------------------------------------------------
bool CSntxAnalyzer::Rebuild()
{
	SetRoot(&GetRule(0));
	TIME_REPORT("\nTime (start build tree): ",clock());

	
	//Склеивание дерева
	for (vector_sntx_rules_it i=m_arrRules.begin();i!=m_arrRules.end();++i){
		for (vector_sntx_rules_it j=m_arrRules.begin();j!=m_arrRules.end();++j) {
			CSntxRule & first  = *(*i);
			CSntxRule & second = *(*j);
			cstr szSecond = second.GetName();

			//iterate whole tree
			for (CSntxRule::tree_gramms_tit it=first.GetTree().begin(); it!=first.GetTree().end(); ++it){
				structs::grm_term & T = (*it).get_val();

				if (T.IsNonterminalLinkByName()){
					if (strcmp(szSecond,T.GetLinkedRuleName())==0)
						T.LinkToRule(second);
				}
			}
		}
	}

	#ifdef _NO_BACK_OPTIMIZATION
	typedef map<string_smart, terminal_data> map_terms;
	map_terms mapTerminals; 

	//iterate tree
	for (vector_sntx_rules_it i=m_arrRules.begin();i!=m_arrRules.end();++i){
		for (CSntxRule::tree_gramms_tit it=(*i)->GetTree().begin(); it!=(*i)->GetTree().end(); ++it){
			structs::grm_term & T = (*it).get_val();
			if (T.IsTerminal() && T.GetTerminal().GetClass()==WORD_CLASS_ID){
				terminal_data & x = mapTerminals[T.GetTextRepresentation().c_str()];
				x.m_nRefs++;
				x.m_pData=&T;
			}
		}
	}

	//Вычисление "поворотных" терминалов
	for (map_terms::iterator i=mapTerminals.begin(); i!=mapTerminals.end(); ++i){
		structs::grm_term & T = *(*i).second.m_pData;
		if (((*i).second.m_nRefs==1) && !T.IsOptional() && !T.IsChildOptional())
			T.SetForbidBacktrackFlag(true);
	}
	#endif

	TIME_REPORT("\nTime (end build tree): ",clock());

	//Проверка
	for (vector_sntx_rules_it i=m_arrRules.begin();i!=m_arrRules.end();++i)
		for (CSntxRule::tree_gramms_tit it=(*i)->GetTree().begin(); it!=(*i)->GetTree().end(); ++it){
			_ASSERTE(!(*it).get_val().IsNonterminalLinkByName());
		}

		//Оптимизация
		for (vector_sntx_rules_it i=m_arrRules.begin();i!=m_arrRules.end();++i) {
			(*i)->InitHashTable();
			(*i)->OptimizeRecursing();
		}
	TIME_REPORT("\nTime (end init optimization): ",clock());
	return true;
}
//------------------------------------------------------------------------
bool CSntxAnalyzer::Analyze(const ILexList & LexList, ISntxList & SntxList)
{
	m_AnalyzeContext.Clear();
	m_AnalyzeContext.m_pLexList=&LexList;
	m_AnalyzeContext.m_pSntxtList=&SntxList;

	LexList.SetPos(0);
	SntxList.Reserve(LexList.GetCount()*5); //Резевируем место под результирующую цепочку


/*	//Скинем дамп хеша
	#if defined(_SEARCH_HASH_OPTIMIZATION_1) || defined(_SEARCH_HASH_OPTIMIZATION_2)
	for (vector_sntx_rules_it i=m_arrRules.begin();i!=m_arrRules.end();++i)
	{
		WriteToDiagnosticFile("Hash for:%s\n",(*i)->GetName());
		(*i)->m_HashInfo.dump(m_DiagnosticFile);
		WriteToDiagnosticFile("\n\n");
	}
	return false;
	#endif

*/	m_nstartClock = clock();
	
	if ((!GetRoot().AnalyzeStructure(m_AnalyzeContext)) || (!LexList.IsEnd()))	
	{
		if (m_AnalyzeContext.m_pConjecturalErrorRule==NULL)
			errors::general_error("Unknown syntax error");
		else
		{
			if (!m_AnalyzeContext.m_ConjecturalSyntaxError.IsError())
			{
				//Попытамся собрать сообщение об ошибке
				CSntxRule::list_symbol_iterate_results lstTerms;
				m_AnalyzeContext.m_pConjecturalErrorRule->GetFrontTerminals(lstTerms);

				string_list lstNames;
				for	(CSntxRule::list_symbol_iterate_results::iterator i=lstTerms.begin(); i!=lstTerms.end();++i)
					lstNames.items().add((*i).m_Symbol.GetTextRepresentation());
				lstNames.items().remove_coincidences(false);

				if (lstNames.items().count()!=0) 
				{
					string_smart txt("Expected \"",lstNames.items()[0],"\"");
					for (size_t i=1; i<lstNames.items().count();i++)
						txt.append(" or \"",lstNames.items()[i],"\"",NULL);

					m_AnalyzeContext.m_ConjecturalSyntaxError.SetName(txt.c_str());
				}
				else
					m_AnalyzeContext.m_ConjecturalSyntaxError.SetName("Syntax error");
			}
			errors::general_syntax_error(m_AnalyzeContext.m_ConjecturalSyntaxError);
		}
			
		return false;
	}
	TIME_REPORT("\nTime (end sntx analysis): ",clock());
	m_AnalyzeContext.WriteToDiagnosticFile("Time:%g",(double(clock()-m_nstartClock))/CLOCKS_PER_SEC);
	return true;
}

//------------------------------------------------------------------------
// End of CSntxAnalyzer
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// CSntxVariant	Class
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
CSntxVariant::CSntxVariant (GRM_PRMS_1,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_2,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_3,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3); 
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_4,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_5,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_6,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_7,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_8,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_9,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);AddTerm(GE9);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_10,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);AddTerm(GE9);AddTerm(GE10);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_11,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);AddTerm(GE9);AddTerm(GE10);AddTerm(GE11);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_12,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);AddTerm(GE9);AddTerm(GE10);AddTerm(GE11);AddTerm(GE12);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_13,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);AddTerm(GE9);AddTerm(GE10);AddTerm(GE11);AddTerm(GE12);AddTerm(GE13);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_14,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);AddTerm(GE9);AddTerm(GE10);AddTerm(GE11);AddTerm(GE12);AddTerm(GE13);AddTerm(GE14);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_15,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);AddTerm(GE9);AddTerm(GE10);AddTerm(GE11);AddTerm(GE12);AddTerm(GE13);AddTerm(GE14);AddTerm(GE15);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_16,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);AddTerm(GE9);AddTerm(GE10);AddTerm(GE11);AddTerm(GE12);AddTerm(GE13);AddTerm(GE14);AddTerm(GE15);AddTerm(GE16);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
CSntxVariant::CSntxVariant (GRM_PRMS_17,const ew & EW, pf_sntx_func pF){
	AddTerm(GE1);AddTerm(GE2);AddTerm(GE3);AddTerm(GE4);AddTerm(GE5);AddTerm(GE6);AddTerm(GE7);AddTerm(GE8);AddTerm(GE9);AddTerm(GE10);AddTerm(GE11);AddTerm(GE12);AddTerm(GE13);AddTerm(GE14);AddTerm(GE15);AddTerm(GE16);AddTerm(GE17);
	AddEnd(pF);
	SetErrorText(EW.GetName());
}
//------------------------------------------------------------------------
void CSntxVariant::AddEnd(pf_sntx_func pFunc)
{
	structs::grm_term GE(GE_END,pFunc);
	AddTerm(GE);	// terminator 
}

//////////////////////////////////////////////////////////////////////
// CSntxRule	Class
//////////////////////////////////////////////////////////////////////

CSntxRule::~CSntxRule()
{
	if (m_pTree)
		m_pTree->release();
}
//------------------------------------------------------------------------
CSntxRule::CSntxRule(): structs::sntx_rule(NULL), m_pTree(NULL),m_bRecursionToCycle(false)
{
}
//------------------------------------------------------------------------
CSntxRule::CSntxRule(const CSntxRule &src): structs::sntx_rule(src), m_pTree(NULL),m_bRecursionToCycle(false)
{
	*this=src;
}
//------------------------------------------------------------------------
CSntxRule::CSntxRule(HUB_PRMS1, void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);
}
CSntxRule::CSntxRule(HUB_PRMS2,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);
}
CSntxRule::CSntxRule(HUB_PRMS3,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);
}
CSntxRule::CSntxRule(HUB_PRMS4,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);
}
CSntxRule::CSntxRule(HUB_PRMS5,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);
}
CSntxRule::CSntxRule(HUB_PRMS6,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);AddVariant(V6);
}
CSntxRule::CSntxRule(HUB_PRMS7,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);AddVariant(V6);AddVariant(V7);
}
CSntxRule::CSntxRule(HUB_PRMS8,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);AddVariant(V6);AddVariant(V7);AddVariant(V8);
}
CSntxRule::CSntxRule(HUB_PRMS9,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);AddVariant(V6);AddVariant(V7);AddVariant(V8);AddVariant(V9);
}
CSntxRule::CSntxRule(HUB_PRMS10,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);AddVariant(V6);AddVariant(V7);AddVariant(V8);AddVariant(V9);AddVariant(V10);
}
CSntxRule::CSntxRule(HUB_PRMS11,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);AddVariant(V6);AddVariant(V7);AddVariant(V8);AddVariant(V9);AddVariant(V10);AddVariant(V11);
}
CSntxRule::CSntxRule(HUB_PRMS12,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);AddVariant(V6);AddVariant(V7);AddVariant(V8);AddVariant(V9);AddVariant(V10);AddVariant(V11);AddVariant(V12);
}
CSntxRule::CSntxRule(HUB_PRMS13,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);AddVariant(V6);AddVariant(V7);AddVariant(V8);AddVariant(V9);AddVariant(V10);AddVariant(V11);AddVariant(V12);AddVariant(V13);
}
CSntxRule::CSntxRule(HUB_PRMS14,void*):structs::sntx_rule(szName), m_pTree(NULL),m_bRecursionToCycle(false){
	SetErrorText(Err.GetName());
	AddVariant(V1);AddVariant(V2);AddVariant(V3);AddVariant(V4);AddVariant(V5);AddVariant(V6);AddVariant(V7);AddVariant(V8);AddVariant(V9);AddVariant(V10);AddVariant(V11);AddVariant(V12);AddVariant(V13);AddVariant(V14);
}

//------------------------------------------------------------------------
CSntxRule & CSntxRule::operator=(const CSntxRule& src)
{
	sntx_rule::operator = (src);

	m_pTree->release();
	m_pTree = src.m_pTree;
	m_pTree->addref(); 

	// Этим	можно пренебречь, но лучше не стоит
	m_bRecursionToCycle=src.m_bRecursionToCycle;

	#if defined(_SEARCH_HASH_OPTIMIZATION_1) || defined(_SEARCH_HASH_OPTIMIZATION_2)
	m_HashInfo = src.m_HashInfo;
	#endif

	return *this;
} 

//The reference to internal syntax tree of the hub
CSntxRule::tree_gramms & CSntxRule::GetTree()	
{
	if (m_pTree==NULL)
		m_pTree=new ref_type<tree_gramms>();
	return *m_pTree;
}	

//Returns string representation of the tree
string_smart CSntxRule::GetTreeDump() 
{
	string_list s;
	for (tree_gramms_tit i=GetTree().begin(); i!=GetTree().end(); ++i){
		s.items().add(string_smart(
			strnchar(' ',i.level()+1).c_str(),
			(*i).get_val().GetTextRepresentation().c_str()).c_str());
	}
	return s.c_str();
}

//Returns string representation of the tree
string_smart CSntxRule::GetXBNF() 
{
	string_smart strName = GetName();
	strName.replace(" ","_");
	strName.replace(",","_");

	size_t nLMargin = 8; //string_smart(GetName(),"  = ").length();

	string_list s;
	for (size_t i=0; i<GetVariantCount(); ++i)
	{
		structs::sntx_variant & V = GetVariant(i);
		string_smart l;
		bool bPrevOptional = false;

		for (size_t j=0;j<V.GetTermCount(); ++j){
			structs::grm_term & T = V.GetTerm(j);
			string_smart x;

			if (T.IsNonterminalLinkByName()){
				x = T.GetLinkedRuleName();
				x.replace(" ","_");
				x.replace(",","_");
			}
			else if (T.IsTerminal()){
				
				switch (T.GetTerminal().GetClass()) {
						case CONST_CLASS_ID: x =  "CONSTANT"; break;
						case ID_CLASS_ID: x = "ID"; break;
						default: x = T.GetTerminal().GetSzText(); x.frames_wrap_on('"','"');
				}
			}
			else if (T.IsTerminalStd()){
				x = T.GetTextRepresentation();
			}
			else if (T.IsTerminalGroup()){
				x = "(";
				for (grm_structs::lexeme_group::lexeme_it k = T.GetTermGroup().Begin(); k!=T.GetTermGroup().End(); ++k)
				{
					x.cat((*k).first);
					grm_structs::lexeme_group::lexeme_it m = k; ++m;
					if (m!=T.GetTermGroup().End())
						x.cat(" | ");
				}
				x.cat(")");
			}

			if (bPrevOptional) {
				if (!T.IsChildOptional()) {
					l.del_last();
					l.cat("] ");
					bPrevOptional = false;
				}
			}

			if (T.IsOptional()){
				l.cat('[');
				bPrevOptional=true;
			}
			l.cat(x).cat(' ');
		}
		if (i==0)
			s.items().add(string_smart(strName," = \n", strnchar(' ',nLMargin), l));
		else
			s.items().add(string_smart(strnchar(' ',nLMargin-2),"| ",l));
	}
	string_smart ret = s.c_str();
	ret.trimright();
	ret.cat(".");
	return ret;
}

//Количество прямых нетерминальных дочерних вершин
size_t CSntxRule::GetNonTerminalRootsCount() 
{
	size_t res=0;
	//Use top level of branches - root level 
	for (tree_gramms_it it = GetTree().child_row_begin(); it!=GetTree().child_row_end(); ++it)
	{
		structs::grm_term & GEL = (*it).get_val();
		if (GEL.IsNonterminal() || GEL.IsNonterminalLinkByName())
			res++;
	}
	return res;
}

//Adds alternatives to the hub
void CSntxRule::AddVariant(const CSntxVariant & src)
{
	sntx_rule::AddVariant(src);

	tree_gramms_it it = GetTree().child_row_begin();
	for	(int i=0;i<src.GetTermCount();i++, it = it->begin())
	{
		const structs::grm_term & GE = src.GetTerm(i);
		tree_gramms_it fit = it.find_sibling_node(GE);
		if (fit)//Такая ветка уже есть, переходим дальше
			it = fit; 
		else												//Такой ветки нет 
			it = it.add_sibling_node(GE);
	}
}

//optimization of recurse structure
void CSntxRule::OptimizeRecursing()	
{
	m_bRecursionToCycle=false;
#ifndef	_SEARCH_RECURSE_OPTIMIZATION
	return ;
#endif
	// Оптимизация грамматик типа <A>=<B>,<C>,<D>.... [<A>]
	//tree:             <B>
	//				end      <A>

	if (GetTree().root_count()!=1) // We must have only 1 root 
		return;		

	if ((--GetTree().end())->get_val().m_pFunc)	//in order to simplify algo
		return;									//we demand that rule cannot have action

	size_t n =0;
	for (tree_gramms::tree_iterator i= GetTree().begin(); !i.bottom(); ++i){
		if ((*i).size()>1)	//item must contain only one variant
			return;			//	

		const structs::grm_term & aItem = i->get_val();

		if (aItem.IsNonterminal() && (&aItem.GetLinkedRule()==this)){
			if (aItem.m_pFunc)		//item cannot have action
				return;				//

			if (!(aItem.IsOptional() || aItem.IsChildOptional()))
				return;
			n++;
			if (!(++i)->get_val().IsTerminalEnd())
				return;
			break;
		}
	}

	if (n!=1)
		return;

	m_bRecursionToCycle=true;
}

//initialization of hash table
void CSntxRule::InitHashTable()
{
#if defined(_SEARCH_HASH_OPTIMIZATION_1) || defined(_SEARCH_HASH_OPTIMIZATION_2)

	if (m_HashInfo.m_bPassed)
		throw exception("Internal Error: left recursion detected");

	if (m_HashInfo.m_bInited)
		return;

	m_HashInfo.m_bPassed=true;

	//Use top level of branches - root level 
	for (tree_gramms_it it = GetTree().child_row_begin(); it!=GetTree().child_row_end(); ++it)
	{
		structs::grm_term & GEL = (*it).get_val();
		switch (GEL.GetClass())
		{
		case GE_TERMINAL:
			// Identifier
			if (GEL.GetTerminal().IsIDClass())
				m_HashInfo.m_bID=true;
			//Constant
			else if (GEL.GetTerminal().IsConstClass())
				m_HashInfo.m_bConst=true;
			//Word
			else if (GEL.GetTerminal().IsWordClass()){
				m_HashInfo.m_bWord=true;
				_ASSERT(*GEL.GetTerminal().GetSzText());
				m_HashInfo.add(GEL.GetTerminal().GetSzText());
			}
			//Operator
			else if (GEL.GetTerminal().IsOperatorClass()){
				m_HashInfo.m_bOperator=true;
				_ASSERT(*GEL.GetTerminal().GetSzText());
				m_HashInfo.add(GEL.GetTerminal().GetSzText());
			}
			//What's it?
			else
				_ASSERT(false);
			break;
		case GE_TERMINAL_GROUP:
			for	(grm_structs::lexeme_group::lexeme_it i = GEL.GetTermGroup().Begin();i!=GEL.GetTermGroup().End();++i)
			{
				const string_std & strName = GEL.GetTermGroup().GetAt(i);
				_ASSERT(!strName.isempty());
				m_HashInfo.add(strName.c_str());
			}
			break;
		case GE_END:
			break;
		case GE_EMPTY:
			continue;
			break;
		case GE_NONTERMINAL:
			GEL.GetLinkedRule().InitHashTable();
			m_HashInfo.unite(static_cast<CSntxRule&>(GEL.GetLinkedRule()).m_HashInfo);
			break;
		default:
			_ASSERT(0);

		}
	}

	m_HashInfo.m_bPassed=false;
	m_HashInfo.m_bInited=true;
#endif
}

#ifdef __DIAGNOSTIC__
#define REPORT0(szText)					Context.WriteToDiagnosticFile(szText,NULL);
#define REPORT1(szText,szA1)			Context.WriteToDiagnosticFile(szText,szA1);
#define REPORT2(szText,szA1,szA2)		Context.WriteToDiagnosticFile(szText,szA1,szA2);
#define REPORT3(szText,szA1,szA2,szA3)	Context.WriteToDiagnosticFile(szText,szA1,szA2,szA3);
#else
#define REPORT0							
#define REPORT1(szText,szA1)			
#define REPORT2(szText,szA1,szA2)		
#define REPORT3(szText,szA1,szA2,szA3)	
#endif

void CSntxRule::AnalyzeError(const structs::grm_term & CurGE,structs::sntx_analyze_context & Context)
{
	const ILexList & LexList=Context.GetLexList();
	const structs::lexeme  & Lexeme = (LexList.IsEnd()) ? LexList.GetPrev() : LexList.Get();

	if (Lexeme.GetEndPos()<Context.m_ConjecturalSyntaxError.GetPos())
		return;

	Context.m_ConjecturalSyntaxError.Clear();
	if (!CurGE.GetError().IsError())
		Context.m_ConjecturalSyntaxError=GetError();
	else 
		Context.m_ConjecturalSyntaxError=CurGE.GetError();

	//if (Context.m_ConjecturalSyntaxError.GetName().isempty())
	//	Context.m_ConjecturalSyntaxError.SetName(strDefaultErrorText);

	Context.m_nConjecturalSyntaxErrorLevel=Context.m_nRecursionLevel;
	Context.m_ConjecturalSyntaxError.SetArg(Lexeme); 
	Context.m_pConjecturalErrorRule=this;

	REPORT3("Possible Error	'%s'. line %u, level %u\n", Context.m_ConjecturalSyntaxError.GetText().c_str(), 
														Context.m_ConjecturalSyntaxError.GetPos().m_nLine,Context.m_nRecursionLevel);
}
//------------------------------------------------------------------------
bool CSntxRule::AnalyzeStructure(structs::sntx_analyze_context & Context)
{
	tree_gramms_tit CurNode;
	tree_gramms_tit LocalTail;

	list	<chain_info> States;
	ISntxList & SntxList	=Context.GetSntxList();
	const ILexList & LexList=Context.GetLexList();

	bool	Success=true;//	static для скорости
	
	#ifdef _SEARCH_RECURSE_OPTIMIZATION
	size_t	ListItems=0;		//Идея: если у нас узел, реализующий список <A>=<B>[<A>]
	#endif						//то его можно организовать без рекурсий
	
	#ifdef _NO_BACK_OPTIMIZATION
	bool	NoBack=false;		// Идея	такова:	если был удачно	обнаружен управляющий терминал (word ||	operator ) и он	разрешает NoBack,
	#endif						// то при появлении	дальнейшей ошибки разбирать	текст не имееть	смысла.	Необходимо выставить FatalError	
	

	Context.m_nRecursionLevel++;
#ifdef __DIAGNOSTIC__
	Context.m_nRulesCompleted++;
	if (Context.m_nMaxRecursionLevel<Context.m_nRecursionLevel)
		Context.m_nMaxRecursionLevel=Context.m_nRecursionLevel;
#endif

START:
	CurNode  = GetTree().begin();	
	LocalTail= --GetTree().end();	
	
	_ASSERT(LocalTail);
	_ASSERT(LocalTail->get_val().IsTerminalEnd());

	States.clear();
	States.push_back(chain_info(SntxList.GetCount()-1,LexList.GetPos()));

	while (true)
	{
		for	(;CurNode; CurNode++,States.push_back(chain_info(SntxList.GetCount()-1,LexList.GetPos())))
		{
			structs::grm_term & CurGE = CurNode->get_val();
			if (CurGE.IsTerminalEnd()) //	это	уже	терминатор - пора заканчивать
			{
				_ASSERT(CurNode->size()==0);
				LocalTail=CurNode;
				CurNode=GetTree().end();	//переводим итератор в нерабочий режим
				break;
			}

			/////----------- TERMINAL----------- /////////	
			if (CurGE.IsTerminalEmpty())
			{
				REPORT0("Found empty terminal. Add. Search next.\n");
				//Так как эта вершина - не реальный терминал, можно по возможности
				//сэкономить на списке 
				if (CurGE.m_pFunc)
					SntxList.Add(CurGE);
				continue;
			}

			if ((CurGE.IsTerminal()) || (CurGE.IsTerminalGroup()))
			{
				if (LexList.IsEnd()) 
				{
					REPORT0("Tried to get next element, but end of expression was detected.\n");
					AnalyzeError(CurGE,Context);
					break;
				}	
				const structs::lexeme&  CurLex= LexList.Get();

				_ASSERT(CurLex.GetSzText());
				///--------------
				//	данный прием является оптимизационным, хотя	насколько, оценить трудно
				//	применяется	только для зарезервированных слов
				if (CurGE.IsTerminalGroup())
				{
					if (!CurGE.GetTermGroup().Find(CurLex.Get_Str_Text()))
					{
						REPORT1("Found TERMINAL '%s': not in group, INCORRECT\n",str_translate_control(CurLex.GetSzText()).c_str());
						goto  INCORRECT_TERMINAL;
					}
					REPORT1("Found TERMINAL '%s': exists in group, OK\n",str_translate_control(CurLex.GetSzText()).c_str());
					SntxList.Add(structs::grm_term(CurLex,CurGE.m_pFunc,CurGE.GetFlags()));
					_ASSERT(SntxList.GetBack().IsTerminal());
					LexList.Next();
					continue;
				}
				///--------------

				if (CurGE.GetTerminal().GetClass()!= CurLex.GetClass())	
				{
					REPORT1("Found TERMINAL '%s': INCORRECT CLASS\n",str_translate_control(CurLex.GetSzText()).c_str());
INCORRECT_TERMINAL:
					AnalyzeError(CurGE,Context);
					break;
				}

				/////----------- TERMINAL: CONST, ID etc ----------- /////////	

				if (CurGE.GetTerminal().Index==NO_INDEX)
				{
					if (CurGE.GetTerminal().IsConstClass())
					{
						REPORT1("Found CONST '%s'...\n",str_translate_control(CurLex.GetSzText()).c_str());
						SntxList.Add(CurGE);
						SntxList.GetBack().GetTerminal().Set(CurGE.GetTerminal().GetClass(),CurGE.GetTerminal().Index, CurLex.Get_Str_Text());
						LexList.Next();
						continue;
					}
					else if (CurGE.GetTerminal().IsIDClass())
					{
						REPORT1("Found ID '%s'...\n",CurLex.GetSzText());
						SntxList.Add(CurGE);
						//Подменяем текст
						SntxList.GetBack().GetTerminal().Set(CurGE.GetTerminal().GetClass(),CurGE.GetTerminal().Index, CurLex.Get_Str_Text());
						LexList.Next();
						continue;
					}
					_ASSERT(0);
				}
				
				if (CurGE.GetTerminal().Index!=CurLex.Index)
				{
					REPORT2("Found TERMINAL '%s', need ('%s').\n",str_translate_control(CurLex.GetSzText()).c_str(), CurGE.GetTerminal().GetSzText());
					AnalyzeError(CurGE,Context);
					break;
				}
				else

				/////----------- TERMINAL: word	or operator----------- /////////	

				{
					REPORT1("Found TERMINAL '%s'. OK\n", str_translate_control(CurLex.GetSzText()).c_str());
					SntxList.Add(CurGE);
					//Подменяем текст
					SntxList.GetBack().GetTerminal().Set(CurGE.GetTerminal().GetClass(),CurGE.GetTerminal().Index, CurLex.Get_Str_Text());
					LexList.Next();		
					#ifdef _NO_BACK_OPTIMIZATION
						NoBack|=CurGE.IsForbidBacktrack();
					#endif
					continue;
				}
				_ASSERT(0);
			}
			else
			
			/////----------- NONTERMINAL----------- /////////

			if (CurGE.IsNonterminal())
			{
				REPORT1("Found NONTERMINAL '%s'...\n",CurGE.GetLinkedRule().GetName());
			#ifdef _SEARCH_RECURSE_OPTIMIZATION
				if ((this==&CurGE.GetLinkedRule()) && m_bRecursionToCycle)	//В случае перехода на самого себя
				{								//можно обойтись без рекурсии
					_ASSERT(!CurGE.m_pFunc); //Оптимизатор должен это учесть
					ListItems++;	
					goto START;
				}
			#endif

				if (
			#if defined(_SEARCH_HASH_OPTIMIZATION_1) || defined(_SEARCH_HASH_OPTIMIZATION_2)
					(LexList.IsEnd() || CurGE.GetLinkedRule().m_HashInfo.appropriate(LexList.Get())) &&
			#endif
					 (CurGE.GetLinkedRule().AnalyzeStructure(Context)!=false)
					)
				{
					REPORT1("NONTERMINAL '%s' Success\n", CurGE.GetLinkedRule().GetName());
					//Ведем статистику удачных дочерних ветвей
					#ifdef _STATISTIC_CHILDREN
					if ((CurNode.level()==0) &&  //это - прямой наследник, то что нам нужно
						(GetTree().root_count()>1) && //и он - не единственный среди дочерних ветвей
						(GetNonTerminalRootsCount()==GetTree().root_count()))  // и дочерние ветви состоят только из нетерминалов
					{
						string_smart strDir(extract_filedir(CSntxAnalyzer::GetCurFile()).c_str(),"\\Statistics\\");
						base_utils::force_dirs(strDir.c_str());
						base_utils::file_std File(string_smart(strDir.c_str(),GetName(),".stc").c_str(),"a");
						File<<CurGE.GetLinkedRule().GetName()<<":";
						//Считаем, какая же это ветвь от края
						for (tree_gramms_tit aNode = GetTree().begin();	aNode; aNode.to_right())
						{
							if (aNode.get_node_ptr()==CurNode.get_node_ptr()){
								File<<"1";
								break;
							}
							File<<"*";
						}
						File<<"\n";
					}
					#endif

					if (CurGE.m_pFunc)			//Только для не-terminal допускается
						SntxList.Add(CurGE);	//использование подобной оптимизации
					continue;
				}
				else
				{
					REPORT1("NONTERMINAL '%s' Fail\n", CurGE.GetLinkedRule().GetName());
					AnalyzeError(CurGE,Context);
					break;
				}
			}
			else
			////---------------- BUG ------------------	//////////			
				_ASSERT(0);

		}//	Это	конец for
		
		//=================== ПРОВЕРКА РАЗБОРА ===================
		if ((Context.m_bFatalError) ||	(!CurNode))
		{
			Success=!Context.m_bFatalError;	// выходим либо	с ошибкой, либо	успешно
			break;
		}

		// где-то была ошибка
		/////----------	RECURSE_OPTIMIZATION --------- /////////
		#ifdef _SEARCH_RECURSE_OPTIMIZATION
		if (ListItems>0)	//Если у нас есть разложенная рекурсия
		{					//<A> = <B> [<A>] <<END>>
			LocalTail=--GetTree().end();
			const structs::grm_term & xx= LocalTail->get_val();

			_ASSERT(LocalTail);
			_ASSERT(LocalTail->get_val().IsTerminalEnd());
			_ASSERT(!LocalTail->get_val().m_pFunc); //Оптимизатор должен это учесть
			break;
		}
		#endif
		/////----------- is	OPTIONAL TERM----------- /////////
		if (CurNode->get_val().IsOptional()) 
		{
			do 
			{
				++CurNode;
				States.push_back(chain_info(SntxList.GetCount()-1,LexList.GetPos()));
			}
			while (CurNode->get_val().IsChildOptional());
			NoBack=false;
			continue;
		}
		/////------- попробуем другой вариант ------- /////////
		if (CurNode)
		{
			long prev_level = CurNode.level();
			if (CurNode.to_right()){	//Вправо еще есть куда двигаться
				long new_level = CurNode.level();

				_ASSERT(new_level<=prev_level);
				for (long i = new_level; i<prev_level; i++) //это будет выполняться на 1 цикл больше, чем разница в уровнях
					States.pop_back();
			}
		}

		if (CurNode)
		{
			_ASSERT(States.size()>0);
			const chain_info & ci = States.back();
			LexList.SetPos(ci.m_nLexPos);
			_ASSERT(ci.m_nChainPos+1<=SntxList.GetCount());
			SntxList.EraseFrom(ci.m_nChainPos+1);
			continue;
		}
		Success=false; break;
	}

	if (Success==true)
	{
		_ASSERT(LocalTail);
		_ASSERT(LocalTail->get_val().IsTerminalEnd());
		Context.m_nRecursionLevel--;

		//Так как эта вершина - не реальный терминал, можно по возможности
		//сэкономить на списке 
		if (LocalTail->get_val().m_pFunc){
			SntxList.Add(LocalTail->get_val());
			SntxList.GetBack().m_szParentRuleName=this->GetName();
		}
	
		if (Context.m_nRecursionLevel==0)
		{
			//ConjecturalSyntaxError.Clear();	Очищать не надо
			REPORT0("\n----------------End of structure analyzing: SUCCESSFULL------------\n");
			REPORT1("Max Recursion: %d\n",Context.m_nMaxRecursionLevel);
			REPORT1("Hubs completed:%d\n",Context.m_nRulesCompleted);
		}
		return true;
	}
#ifdef _NO_BACK_OPTIMIZATION
	if (NoBack)
		Context.m_bFatalError=true;
#endif
	
	_ASSERT(States.size()>=1);
	const chain_info & ci = States.front();
	LexList.SetPos(ci.m_nLexPos);
	_ASSERT(ci.m_nChainPos+1<=SntxList.GetCount());
	SntxList.EraseFrom(ci.m_nChainPos+1);
	Context.m_nRecursionLevel--;

	if (Context.m_nRecursionLevel==0)
	{
		REPORT0("\n----------------End of structure analyzing: FAILED------------\n");
		REPORT1("Max Recursion: %d\n",Context.m_nMaxRecursionLevel);
		REPORT1("Hubs completed:%d\n",Context.m_nRulesCompleted);
		_ASSERT(LexList.IsStart());
	}
	return false;
}
