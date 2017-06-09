/********************************************************************
	created:	2004/06/08
	created:	8:6:2004   18:40
	filename: 	X:\MultiTranslator\sources\grm dll kernel\grammaticalinference.h
	file path:	X:\MultiTranslator\sources\grm dll kernel
	file base:	grammaticalinference
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Grammatical Inference algorithms implementation
*********************************************************************/

#include <GrmKrnlExports.h>
#include "SntxAnalyzer.h"
#include <algorithm>

class CGrammaticalInference: public grmkrnl_exports::IGrammaticalInference
{
protected:
	class gi_rule;
	typedef list<gi_rule*> list_gi_rule;

	//-- class -----------------------------------------------------------
	struct gi_lexeme: public lexeme_ex
	{
	public:
		int m_nIndex;
		gi_lexeme(const string_std & strText, ELexemeCLass nLexClass, EStdTerminalClass nStandardTerminal=NO_STD_TERMINAL): 
				lexeme_ex(strText,nLexClass,nStandardTerminal),m_nIndex(-1)
		{
		}

		gi_lexeme(): lexeme_ex(){
		}
		
		gi_lexeme(const lexeme_ex & lex): lexeme_ex(lex){
		}
		
		void dump(base_utils::stream & s) const;
	};
	typedef list<gi_lexeme> list_gi_lexeme;
	typedef list_gi_lexeme::iterator list_gi_lexeme_it;

	//-- class -----------------------------------------------------------
	class gi_symbol
	{
	private:
		bool m_bTerminal;			//Является ли данный символ терминалом
		gi_lexeme m_Lexeme;			//Для терминала
		gi_rule * m_pLinkedRule;	//Для нетерминала
	public:	
		bool is_terminal() const {return m_bTerminal;}
		gi_symbol (const gi_lexeme & Lexeme): m_bTerminal(true),m_Lexeme(Lexeme),m_pLinkedRule(NULL) {
		}
		gi_symbol (gi_rule * pRule): m_bTerminal(false),m_pLinkedRule(pRule){
			m_pLinkedRule->AddRef();
		}
		gi_symbol (const gi_symbol & src): m_bTerminal(false),m_pLinkedRule(NULL){
			this->operator = (src);
		}
		~gi_symbol(){
			if (m_pLinkedRule)
				m_pLinkedRule->Release();
		}

		gi_symbol & operator = (const gi_symbol & src){
			if (src.m_pLinkedRule!=m_pLinkedRule)
			{
				if (m_pLinkedRule)	m_pLinkedRule->Release();
				m_pLinkedRule=src.m_pLinkedRule;
				if (m_pLinkedRule)	m_pLinkedRule->AddRef();
			}
			m_bTerminal = src.m_bTerminal;
			m_Lexeme = src.m_Lexeme;
			return *this;
		}

		bool operator == (const	gi_symbol & src)	const{
			if (is_terminal())
				return m_Lexeme == src.m_Lexeme;
			else
				return m_pLinkedRule == src.m_pLinkedRule;
		}
		
		gi_rule * get_linked_rule() const{
			_ASSERT(m_pLinkedRule); return m_pLinkedRule;
		}

		bool like(const gi_symbol & s) const;
		void dump(base_utils::stream & s) const;
		
		static bool compare(const gi_symbol & s1, const gi_symbol & s2){
			return s1.like(s2);
		}
	};

	//-- class -----------------------------------------------------------
	//У данного правила отсутствуют варианты - вариант всегда 1
	class gi_rule: public base_utils::ref_cnt_object
	{
	public:
		typedef list<gi_symbol> list_symbols;
	private:
		static long  m_nID;
		list_gi_rule&m_Owner;
		string_std	 m_strName;
		list_symbols m_lstSymbols;
	public:
		list_gi_lexeme m_lstParsedLexemes;	 //Собирает информацию о том, какие лексемы прошли через правило при выполнении Parse
		long m_nLinkLevel; //Для сортировки

		gi_rule(list_gi_rule & Owner): m_strName(Format("Rule %d",++m_nID).c_str()),m_Owner(Owner),m_nLinkLevel(0) {
			m_Owner.push_back(this);
		}
		virtual ~gi_rule(){
			m_Owner.erase(std::find(m_Owner.begin(),m_Owner.end(),this));
		}
		
		void set_name(cstr szName){
			m_strName = szName;
		}

		cstr name_cstr() const{
			return m_strName.c_str();
		}
		size_t count() const {
			return m_lstSymbols.size();
		}
		void add(const gi_symbol & s) {
			m_lstSymbols.push_back(s);
		}

		//Выдает список всех правил, на которые оно ссылается 
		void get_linked_rules(list_gi_rule & lst);

		bool expose_link(gi_rule * pRule); //Заменяет ссылку на нетерминал на его содержимое

		void add_lexeme(const gi_lexeme & lex);
		void process();		//Поиск поторяющихся элементов и их объединение в нетерминалы
		void merge();		//Пробег по нетерминалам с попыткой удалить те из них, которые используются только 1 раз

		void chunk(list_symbols & lst); //Пробег по нетерминалам с попыткой подменить два последних символа в вызов нетерминала	

		void dump(base_utils::stream & s) const;
		
		static bool compare_level(gi_rule * pRule1,gi_rule * pRule2){
			return pRule1->m_nLinkLevel<pRule2->m_nLinkLevel;
		}
		
		//--- прогон текста через полученную синтаксическую структуру
		void parse(list_gi_lexeme_it & itCur);
		void dump_parse_result(base_utils::stream & s) const;
	};
	//--------------------------------------------------------------------
private:
	int m_nRefCnt;
	CSntxAnalyzer	& m_SntxAnalyzer;
	CLexSource		& m_LexSource;
	list_gi_lexeme	m_lstLexeme_Parse;	//После лексического анализа
	list_gi_rule	m_lstRule_BSS;		//После BuildSyntaxStructure
	gi_rule	*		m_pRoot;
	list_wrd		m_lstWords;			//Содержит статистику повторений слов
public:
	CGrammaticalInference(CSntxAnalyzer & SntxAnalyzer, CLexSource & LexSource):
		m_nRefCnt(0),m_SntxAnalyzer(SntxAnalyzer),m_LexSource(LexSource),m_pRoot(NULL)
	{
	}
	virtual ~CGrammaticalInference(){
		delete m_pRoot;
	}

	override HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {
		return E_NOTIMPL;
	}
	override ULONG   STDMETHODCALLTYPE AddRef(void) {
		m_nRefCnt++; return m_nRefCnt;
	}
	override ULONG   STDMETHODCALLTYPE Release(void){
		m_nRefCnt--; if (m_nRefCnt==0) delete this; return m_nRefCnt;
	}

	override void STDMETHODCALLTYPE ParseLexical(cstr szDumpFile);
	override void STDMETHODCALLTYPE FindCorrespondence(cstr szDumpFile);	//Найти соответствия некоторых частей текста в других грамматиках
	override void STDMETHODCALLTYPE BuildSyntaxStructure(cstr szDumpFile);	//Построить синтаксическую структуру документа
	override list_wrd * STDMETHODCALLTYPE SearchWordRepetition(cstr szDumpFile);//Найти слова-повторения в документе и выделить их как keywords
};