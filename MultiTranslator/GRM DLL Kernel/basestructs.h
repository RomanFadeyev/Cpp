
#if !defined(AFX_BASE_STRUCTS_H__0939AA68_7120_11CF_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_BASE_STRUCTS_H__0939AA68_7120_11CF_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

#include <Functors.h>
#include "GrmKrnlExports.h"
#include "GrmStructs.h"

extern DWORD Language;

#define UNDEFINED_ADDR	UINT_MAX
#define ALPHABET_SIZE	128

class CSntxRule;

interface ILexList;

namespace grmkrnl_structs
{
	typedef unsigned TFuncRet;

	//------------------------------------------------------------------------
	struct lexeme: public grm_structs::grm_base_lexeme
	{
	public:
		lexeme(cstr szText){
			m_strText=szText;
		}
		lexeme	(ELexemeCLass id): grm_base_lexeme(id){
		}

		override cstr GetSzTypeName()	const{
			return "";
		}
	};

	//------------------------------------------------------------------------
	struct lexeme_ex: public lexeme
	{
	public:
		EStdTerminalClass m_nStandardTerminal;// нужен для обеспечения некоторых заранее определенных терминалов, как то ID, CONST ...

		lexeme_ex(const string_std & strText, ELexemeCLass nLexClass, EStdTerminalClass nStandardTerminal=NO_STD_TERMINAL): 
			lexeme(nLexClass), m_nStandardTerminal(nStandardTerminal)
		{
			m_strText = strText;
		}

		lexeme_ex(): lexeme(NO_CLASS), m_nStandardTerminal(NO_STD_TERMINAL)	{
		}

		bool is_std_terminal() const{
			return m_nStandardTerminal!=NO_STD_TERMINAL;
		}
	};

	//------------------------------------------------------------------------
	class grm_base
	{
	protected:
		size_t		m_nPreFuncAddr;		// Здесь будет хранится имя метки функции, которая выполняется до выполнения элемента
		string_std	m_strPreFuncName;	// а после меняться на адрес
		size_t		m_nFuncAddr;		// Здесь будет хранится имя метки функции
		string_std	m_strFuncName;		// а после меняться на адрес
	public:
		grm_base():m_nFuncAddr(UNDEFINED_ADDR),m_nPreFuncAddr(UNDEFINED_ADDR){
		}

		void write_dmp(base_utils::stream  & s){
			s.write_dmp(m_nPreFuncAddr);
			s.write_dmp(m_nFuncAddr);
		}
		void read_dmp(base_utils::stream  & s){
			s.read_dmp(m_nPreFuncAddr);
			s.read_dmp(m_nFuncAddr);
		}

		cstr GetFuncName()		const	{_ASSERT(IsHasFunc()); return m_strFuncName.c_str();}
		cstr GetPreFuncName()	const	{_ASSERT(IsHasPreFunc()); return m_strPreFuncName.c_str();}
		size_t GetFuncAddr()	const	{_ASSERT(IsFuncLinked()); return m_nFuncAddr;}
		size_t GetPreFuncAddr()	const	{_ASSERT(IsPreFuncLinked()); return m_nPreFuncAddr;}
		
		bool CmpFuncName(cstr szFuncName) const {return m_strFuncName.cmp(szFuncName);}
		bool CmpPreFuncName(cstr szPreFuncName) const {return m_strPreFuncName.cmp(szPreFuncName);}

		bool IsHasFunc()		const	{return !m_strFuncName.isempty();}
		bool IsHasPreFunc()		const	{return !m_strPreFuncName.isempty();}
		bool IsFuncLinked()		const	{return m_nFuncAddr!=UNDEFINED_ADDR;}
		bool IsPreFuncLinked()	const	{return m_nPreFuncAddr!=UNDEFINED_ADDR;}

		void ClearFunc()				{m_nFuncAddr=UNDEFINED_ADDR; m_strFuncName.erase();}
		void ClearPreFunc()				{m_nPreFuncAddr=UNDEFINED_ADDR; m_strPreFuncName.erase();}

		void LinkFunc(size_t nAddr)		{m_nFuncAddr=nAddr;}	
		void LinkPreFunc(size_t nAddr)	{m_nPreFuncAddr=nAddr;}	
	};

	//--------------------------------------------------------------------
	// grm_symbol
	//--------------------------------------------------------------------
	class sntx_rule;
	typedef grm_structs::grm_base_sntx_term<grmkrnl_structs::lexeme, CSntxRule> grm_item_t;
	struct grm_symbol: public grm_item_t, public grm_base
	{
	protected:
		size_t		m_nLinkedRule;
		size_t		m_nID;		//Уникальный ID
		string_std	m_strText;	
	public:
		cstr GetSzText() const{
			return m_strText.c_str();
		}

		void SetText(const string_std & src){
			m_strText=src;
		}

		void write_dmp(base_utils::stream  & s){
			grm_base::write_dmp(s);
			grm_item_t::write_dmp(s);
			s.write_dmp(m_nLinkedRule);
			s.write_dmp(m_nID);
		}

		void read_dmp(base_utils::stream  & s){
			grm_base::read_dmp(s);
			grm_item_t::read_dmp(s);
			s.read_dmp(m_nLinkedRule);
			s.read_dmp(m_nID);
		}

		size_t	GetLinkedRuleAddr() const	{return m_nLinkedRule;	}
		size_t	GetID() const				{return m_nID;}

		void LinkToRuleAddr(size_t nRuleAddr){
			_ASSERT(IsNonterminalLinkByName());
			SetClass(GE_NONTERMINAL);
			m_nLinkedRule=nRuleAddr;
		}

		void LinkToRule(CSntxRule & rule){
			_ASSERT(IsNonterminal() || IsNonterminalLinkByName());
			SetClass(GE_NONTERMINAL_LINK_BY_NAME);
			grm_item_t::LinkToRule(	rule);
		}

		void Clear() {
			grm_base::ClearFunc();
			grm_base::ClearPreFunc();
			grm_item_t::Clear();
		}

		grm_symbol ():m_nLinkedRule(UNDEFINED_ADDR)	{
		}
		
		grm_symbol (lexeme & Terminal, cstr szErrorText, cstr szFuncName, cstr szPreFuncName,flags_t Flags, size_t nID)
			:grm_item_t(Terminal,Flags), m_nLinkedRule(UNDEFINED_ADDR)
		{
			_ASSERT(szFuncName!=szPreFuncName); //Это похоже на опечатку при вызове
			this->m_strFuncName		= szFuncName;
			this->m_strPreFuncName	= szPreFuncName;
			this->m_nID = nID;
			SetError(szErrorText);	
		}
		grm_symbol (cstr szLinkedRule, cstr szFuncName, cstr szPreFuncName, flags_t Flags, size_t nID)
			:grm_item_t(szLinkedRule,Flags),m_nLinkedRule(UNDEFINED_ADDR)
		{
			_ASSERT(szFuncName!=szPreFuncName); //Это похоже на опечатку при вызове
			this->m_strFuncName		= szFuncName;
			this->m_strPreFuncName	= szPreFuncName;
			this->m_nID = nID;
		}

		grm_symbol (EStdTerminalClass StdTerminalClass, cstr szErrorText, cstr szFuncName, cstr szPreFuncName,flags_t Flags, size_t nID)	
			:grm_item_t(StdTerminalClass,Flags),m_nLinkedRule(UNDEFINED_ADDR)
		{
			_ASSERT(szFuncName!=szPreFuncName); //Это похоже на опечатку при вызове
			this->m_strFuncName		= szFuncName;
			this->m_strPreFuncName	= szPreFuncName;
			this->m_nID = nID;
			SetError(szErrorText);	
		}
		
		grm_symbol (cstr szRegExp, double fDummy, cstr szErrorText, cstr szFuncName, cstr szPreFuncName,flags_t Flags, size_t nID)	
			:grm_item_t(szRegExp,fDummy, Flags),m_nLinkedRule(UNDEFINED_ADDR)
		{
			_ASSERT(szFuncName!=szPreFuncName); //Это похоже на опечатку при вызове
			this->m_strFuncName		= szFuncName;
			this->m_strPreFuncName	= szPreFuncName;
			this->m_nID = nID;
			SetError(szErrorText);	
		}

		grm_symbol (cstr szFuncName, cstr szPreFuncName)	// it's for TerminalHub
			:grm_item_t(GE_END), m_nLinkedRule(UNDEFINED_ADDR)
		{
			_ASSERT(szFuncName!=szPreFuncName); //Это похоже на опечатку при вызове
			this->m_strFuncName		= szFuncName;
			this->m_strPreFuncName	= szPreFuncName;
			this->m_nID = 0;
		}

		~grm_symbol()	{
			Clear();
		}
	};

	//--------------------------------------------------------------------
	// sntx_variant
	//--------------------------------------------------------------------
	typedef grm_structs::grm_base_sntx_variant<grmkrnl_structs::grm_symbol> grm_sntx_variant_t;
	class sntx_variant: public grm_base,public grm_sntx_variant_t 
	{
	private:
		size_t		m_nStatistic;
	public:
		sntx_variant():m_nStatistic(0){
		};

		grm_sntx_variant_t::AddTerm;

		override void read_dmp(base_utils::stream & Stream){
			grm_base::read_dmp(Stream);
			grm_sntx_variant_t::read_dmp(Stream);
		}
		override void write_dmp(base_utils::stream & Stream){
			grm_base::write_dmp(Stream);
			grm_sntx_variant_t::write_dmp(Stream);
		}

		void SetFunction(cstr szFuncName, cstr szPreFuncName){
			m_strFuncName=szFuncName;
			m_strPreFuncName=szPreFuncName;
		}

		size_t	GetStatistic() const		{return m_nStatistic;}
		void	SetStatistic(size_t val)	{m_nStatistic=val;} 
		void	IncStatistic()				{m_nStatistic++;}

		void Clear() {
			grm_base::ClearFunc();
			grm_base::ClearPreFunc();
			grm_sntx_variant_t::Clear();
		}

	};

	//--------------------------------------------------------------------
	// sntx_rule
	//--------------------------------------------------------------------
	typedef grm_structs::grm_base_sntx_rule<sntx_variant, grmkrnl_structs::grm_symbol,ALPHABET_SIZE> sntx_rule_t;
	class sntx_rule: public sntx_rule_t
	{	
	public:
		sntx_rule(cstr szName): sntx_rule_t(szName){}
	};

	//--------------------------------------------------------------------
	//grm_sntx_analyzer
	//--------------------------------------------------------------------
	typedef grm_structs::grm_base_sntx_analyzer<CSntxRule> grm_sntx_analyzer_t;
	class grm_sntx_analyzer: public grm_sntx_analyzer_t{
	};

	//------------------------------------------------------------------------
	struct grm_parse_packet: public grmkrnl_exports::parse_info_packet
	{
		grm_parse_packet & operator = (const grm_symbol & SrcElement)
		{
			this->Type=SrcElement.GetClass();
			switch (this->Type)
			{
			case GE_TERMINAL:
				szText=SrcElement.GetTerminal().GetSzText();
				break;
			case GE_STANDARD_TERMINAL:
				szText=SrcElement.GetSzText();
				StandardTerminal=SrcElement.GetStdTerminalClass();
				break;
			case GE_REGEXP_TERMINAL:
				szText=SrcElement.GetSzText();
				//StandardTerminal=SrcElement.GetStdTerminalClass();
				break;
			case GE_END:
				szText=NULL;
				break;
			case GE_NONTERMINAL:
				szText=SrcElement.GetLinkedRuleName();
				break;
			default:
				_ASSERT(false);
			}
			return *this;
		}
	};
};

#endif // !defined(AFX_BASE_STRUCTS_H__0939AA68_7120_11CF_A1F4_0000E8D9F1D7__INCLUDED_)
