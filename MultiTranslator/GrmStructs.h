/********************************************************************
	created:	2003/06/23
	created:	23:6:2003   21:35
	filename: 	$(Projects)\sources\grmstructs.h
	file path:	$(Projects)\sources
	file base:	grmstructs
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Базовые типы для всех grm-проектов. Здесь содержаться
				как утилитарные классы (позиция в тексте, описание ошибки и проч),
				так и шаблонные заготовки для ситаксического анализатора:
				литера, лексема, терминал/нетерминал, альтернатива, правило,
				и, собственно, сам синтаксический анализатор
				Кроме того: классы для управления типами в семантическом анализаторе,
				классы для орагниазции аргументов функций и проч.
*********************************************************************/

#pragma	once
#include <BaseUtils.h>
#include <tree.h>

using namespace	base_utils;

enum E3State					{VAL_OFF = 0,VAL_ON = 1, VAL_AUTO = 2};
//------------------------------------------------------------------------

enum EStdTerminalClass		{	NO_STD_TERMINAL,
								STD_ID,				//identifier
								STD_INT_CONST,		//integer constant
								STD_NUM_CONST,		/*==INT|FLOAT*/
								STD_DQMSTR_CONST,	//string ""
								STD_SQMSTR_CONST,	//string ''
								STD_STR_CONST,		//any string (either "" or '')	
								STD_COMMON_CONST,	//all of mentioned above
								STD_LINE,			//from the position to the end of the line
								STD_ALPHA,			//one symbol from 'a'..'z', 'A'..'Z'
								STD_NUM,			//one symbol from '0'..'9'
								STD_ALNUM,			//STD_ALPHA || STD_NUM
								STD_PUNCT,			//any punctuation symbol
								STD_CNTRL			//from 1 to 31
							};

enum ETermClass					{GE_NULL=0,GE_TERMINAL,GE_TERMINAL_GROUP,GE_NONTERMINAL, GE_NONTERMINAL_LINK_BY_NAME, GE_END, GE_STANDARD_TERMINAL, GE_REGEXP_TERMINAL, GE_EMPTY};

enum ELexemeCLass				{CONST_CLASS_ID='C', ID_CLASS_ID	='V', WORD_CLASS_ID='W', OPERATOR_CLASS_ID='O', NO_CLASS=0, 
								 // дополнительные для генерации кода	
								 LABEL_CLASS_ID='L', TYPE_CLASS_ID='T', DIRECT_NUMBER_CLASS_ID=' ',
								 FUNC_CLASS_ID='F',	SUBROUTINE_CLASS_ID='S',TEMPORARY_CONST_CLASS_ID='X',
								 TEMPORARY_ID_CLASS_ID='Y'
								};

namespace grm_structs
{
	inline cstr GetStdTerminalClassName(EStdTerminalClass Class)
	{
		switch(Class) {
			case NO_STD_TERMINAL:		return "NO_STD_TERMINAL";
			case STD_ID:				return "STD_ID";
			case STD_INT_CONST:			return "STD_INT_CONST";
			case STD_NUM_CONST:			return "STD_NUM_CONST";
			case STD_DQMSTR_CONST:		return "STD_DQMSTR_CONST";
			case STD_SQMSTR_CONST:		return "STD_SQMSTR_CONST";
			case STD_STR_CONST:			return "STD_STR_CONST";
			case STD_COMMON_CONST:		return "STD_COMMON_CONST";	
			case STD_LINE:				return "STD_LINE";
			case STD_ALPHA:				return "STD_ALPHA";
			case STD_NUM:				return "STD_NUM";
			case STD_ALNUM:				return "STD_ALNUM";
			case STD_PUNCT:				return "STD_PUNCT";
			case STD_CNTRL:				return "STD_CNTRL";

			default:
				_ASSERT(false);
		}
		return "";
	}

	//--------------------------------------------------------------------
	//Описывает положение в тексте: строка, столбец
	//--------------------------------------------------------------------
	struct text_pos
	{
		long m_nLine;
		long m_nCol;
		
		text_pos():m_nLine(0),m_nCol(0){
		}
		text_pos(long nLine, long nCol):m_nLine(nLine), m_nCol(nCol){
		}
		
		void clear(){
			m_nLine=m_nCol=0;
		}

		bool operator == (const text_pos & src)	const {
			return (m_nLine==src.m_nLine) && (m_nCol==src.m_nCol);
		}

		bool operator > (const text_pos & src)	const {
			return ((m_nLine>src.m_nLine) || 
					((m_nLine==src.m_nLine) && (m_nCol>src.m_nCol)));
		}

		bool operator >= (const text_pos & src) const {
			return this->operator == (src) || this->operator > (src);
		}
		
		bool operator < (const text_pos & src) const {
			return ! this->operator >= (src);
		}

		bool operator <= (const text_pos & src) const {
			return this->operator == (src) || this->operator < (src);
		}
	};

	//---------------------------------------------------------------------
	//Описывает положение в тексте: название файла, строка, столбец
	//--------------------------------------------------------------------
	struct text_location
	{
		string_std	m_strFileName;
		text_pos	m_Pos;	
		
		text_location(){
		}

		//Ма используем не cstr а string_std, потому что это эффективнее
		//string_std работает с ref_cnt
		text_location(const string_std & strFileName, long nLine, long nCol):m_strFileName(strFileName), m_Pos(nLine,nCol){
		}
		text_location(const string_std & strFileName, const text_pos & TextPos):m_strFileName(strFileName), m_Pos(TextPos){
		}

		operator text_pos() const	{return m_Pos;}
		const text_pos & GetPos() const {return m_Pos;}
		void SetPos(const text_pos & pos){m_Pos=pos;}

		void Clear(){
			m_strFileName.clear();
			m_Pos.clear();
		}
	};
	
	//--------------------------------------------------------------------
	//Описывает положение в тексте: название файла, (строка, столбец) в начале + (строка, столбец) в конце
	//--------------------------------------------------------------------
	struct text_start_end_location
	{
		string_std	m_strFileName;
		text_pos	m_StartPos;	
		text_pos	m_EndPos;	
		
		text_start_end_location(){
		}

		text_start_end_location(const string_std & strFileName, const text_pos & StartPos, const text_pos & EndPos)
			:m_strFileName(strFileName), m_StartPos(StartPos), m_EndPos(EndPos){
		}
		text_start_end_location(const text_location & loc, long nHeight, long nWidth)
			:m_strFileName(loc.m_strFileName), m_StartPos(loc.m_Pos), m_EndPos(loc.m_Pos.m_nLine+nHeight-1,loc.m_Pos.m_nCol+nWidth-1){
		}

		text_location start_location()	const	{return text_location(m_strFileName,m_StartPos);}
		text_location end_location()	const	{return text_location(m_strFileName,m_EndPos);}

		const text_pos & start_pos()	const	{return m_StartPos;}
		const text_pos & end_pos()		const	{return	m_EndPos;}

		void clear()	{m_strFileName.erase(); m_StartPos.clear(); m_EndPos.clear();};
	};

	//------------------------------------------------------------------------
	//класс для описания трех параметров: тект, имя файла и позиция начала и конца
	//------------------------------------------------------------------------
	class located_text
	{
	protected:
		typedef text_pos					text_pos;
		typedef text_location				text_location;
		typedef text_start_end_location		text_se_location;
		string_std			m_strText;
		text_se_location	m_Location;
	public:
		cstr						GetSzText()			const {return m_strText.c_str();}
		const string_std		&	Get_Str_Text()		const {return m_strText;}

		const text_se_location	&	GetLocation()		const {return m_Location;}
		text_location				GetStartLocation()	const {return GetLocation().start_location();}
		text_location				GetEndLocation()	const {return GetLocation().end_location();}

		located_text & operator = (const located_text & src){
			m_strText	= src.m_strText;
			m_Location	= src.m_Location;
			return *this;
		}
	};

	//--------------------------------------------------------------------
	// located_typed_text, abstract class
	//--------------------------------------------------------------------
	class located_typed_text: public located_text
	{
	public:
		virtual cstr GetSzTypeName()				const=0;
	};

	//--------------------------------------------------------------------
	// text_container
	//--------------------------------------------------------------------
	class text_container: public located_text
	{
	public:
		text_container(){
		}
		text_container(const string_std & strText, const text_se_location & Location){
			m_strText = strText;
			m_Location= Location;
		}
		
		text_container & operator = (const located_text & src){
			located_text::operator = (src);
			return *this;
		}

		void set(const string_std & strText)	{m_strText=strText;}
		void set(const text_se_location & loc)	{m_Location=loc;}

		void clear()							{m_strText.erase(); m_Location.clear();}
	};

	//--------------------------------------------------------------------
	// хранит в себе информацию о тексте ошибки и положении, где эта ошибка 
	// была обнаружена. Характеризуется текстом ошибки (Имя) и информацией 
	// о положении и исх тексте(Аргумент). Чтобы значение аргумента присутствовало
	// в общем сообщении, в имени нужно использовать %s 
	//--------------------------------------------------------------------
	class sntx_error
	{
	private:
		string_std		m_strName;
		text_container	m_Object;
	public:
		sntx_error() {}
		sntx_error(cstr szName): m_strName(szName) {};

		void Clear()								{m_strName.erase(); m_Object.clear();};
		void SetName(cstr szName)					{m_strName=szName;}
		void SetName(const string_std & strName)	{m_strName=strName;}

		void SetArg(const located_text & aObject)	{m_Object=aObject;}
		
		const string_std & GetName() const			{return m_strName;}
		string_smart GetText() const				{string_smart s(m_strName.c_str()); s.replace("%s",m_Object.Get_Str_Text().c_str()); return s;}
		
		bool IsError() const						{return !m_strName.isempty();}	

		text_pos		GetPos()		const		{return GetLocation();}
		text_location	GetLocation()	const		{return m_Object.GetStartLocation();}
	};

	//////////////////////////////////////////////////////////////////////
	//Единица символа, кроме самого символа еще хранит его тэг
	//////////////////////////////////////////////////////////////////////
	class litera
	{
	public:
		enum lex_tag{	TAG_NONE	= 0,
			TAG_LETTER	=(UINT)(0x1),	// 1
			TAG_DIGIT	=(UINT)(0x2),	// 2
			TAG_PUNCT 	=(UINT)(0x4),	// 4
			TAG_SPACE	=(UINT)(0x8),	// 8
			TAG_END		=(UINT)(0x10),	// 16
			TAG_ALT		=(UINT)(0x20),	// 32
			TAG_COMMENT	=(UINT)(0x40),	// 64
			TAG_SYMBOL	= TAG_PUNCT | TAG_SPACE | TAG_END,
			TAG_DL		= TAG_DIGIT | TAG_LETTER,
			TAG_DLAS	= TAG_DL | TAG_ALT | TAG_SPACE,
			TAG_ANY		= (UINT)(0xffffffff)
		};
	private:
		char_t	m_chName;
		lex_tag	m_nTag;
	public:
		char_t GetName() const 	{return m_chName;}
		lex_tag	GetTag() const	{return m_nTag;}

		__declspec(property(get=GetName)) char_t	Name;
		__declspec(property(get=GetTag))  lex_tag	Tag;

		litera(): m_nTag(TAG_NONE),m_chName(0){
		};

		litera(char_t ch, lex_tag nTag): m_nTag(nTag),m_chName(ch){
		};

		bool operator == (const litera & src){	
			if (m_chName!=0)
				return m_chName==src.m_chName;	//Если есть установленный определенный знак, используем его
			else
				return m_nTag==src.m_nTag;
		}

		litera & operator = (const litera & src)	{
			this->m_nTag=src.m_nTag;
			this->m_chName=src.m_chName;
			return *this;
		};

		void Clear() {
			m_nTag=TAG_NONE; m_chName=0;
		}

		void Set(char_t	ch, lex_tag	nTag){
			m_chName=ch;
			m_nTag=nTag;
		}
	};

	//--------------------------------------------------------------------
	// grm_base_lexeme
	//--------------------------------------------------------------------
	class grm_base_lexeme: public located_typed_text {
	private:
		ELexemeCLass	m_nClass;	
	protected:
	public:
		grm_base_lexeme (const grm_base_lexeme & src){
			*this=src;											
		}

		grm_base_lexeme	(ELexemeCLass id): m_nClass(id) {
		}

		grm_base_lexeme	(ELexemeCLass id, const string_std & strText): m_nClass(id) {
			located_text::m_strText=strText;
		}

		grm_base_lexeme	(): m_nClass(NO_CLASS) {
		}

		void SetText (const string_std & strText)
		{
			located_text::m_strText=strText;
		}

		void SetLocation(const text_se_location & Location){
			located_text::m_Location=Location;
		}

		void Clear()								
		{
			m_Location.clear();
			m_strText.erase();
			m_nClass=NO_CLASS;	
		}

		bool operator ==(const grm_base_lexeme & src) const {
			if (m_nClass==src.m_nClass)
				return m_strText == src.m_strText;
			else
				return false;
		}

		const text_pos & GetStartPos() const{
			return m_Location.start_pos();
		}

		const text_pos & GetEndPos() const{
			return m_Location.end_pos();
		}

		void SetClass(ELexemeCLass cls){
			m_nClass=cls;
		}

		ELexemeCLass GetClass() const {
			return m_nClass;
		}

		bool IsConstClass() const	{return m_nClass==CONST_CLASS_ID;}
		bool IsIDClass()	const	{return m_nClass==ID_CLASS_ID;}
		bool IsWordClass()	const	{return m_nClass==WORD_CLASS_ID;}
		bool IsOperatorClass()const	{return m_nClass==OPERATOR_CLASS_ID;}
		bool IsNoClass()	const	{return m_nClass==NO_CLASS;}

		//Чтобы отличать строку о символа было решено оставить вперели строки
		//кавычку. Теперь ее надо убрать. Поэтому приходиться вводить новый метод
		//место довольно скользкое
		cstr GetSzPureText() const {return GetSzText();} //return ConvertCompilerString(res);

		virtual void write_dmp(base_utils::stream  & s){
			s.write_dmp(GetClass());
			s.write_bstr(m_strText);
		}

		virtual void read_dmp(base_utils::stream  & s){
			s.read_dmp(m_nClass); 
			m_strText = s.read_bstr().c_str(); //safe обязательно
		}
	};

	//--------------------------------------------------------------------
	// Lexeme Group
	//--------------------------------------------------------------------
	class lexeme_group	//группа однотипных лексем - используется только для повышения производительности
	{
	private:
		typedef map <string_std, bool> map_lexeme;
		map_lexeme m_mapLexemes;
	public:
		typedef map_lexeme::const_iterator lexeme_it;

		lexeme_group(){
		}

		lexeme_group(const lexeme_group & src){
			*this=src;
		}

		lexeme_group(cstr szName,...)
		{
			m_mapLexemes[szName]=true;

			va_list marker;
			va_start( marker, szName);     

			while ((szName=(cstr) va_arg( marker, cstr))!=NULL)
				m_mapLexemes[szName]=true;
			va_end( marker );              
		}

		bool Find(const string_std & strName) const {
			return m_mapLexemes.find(strName)!=m_mapLexemes.end();
		}

		lexeme_it Begin() const{
			return m_mapLexemes.begin();
		}

		lexeme_it End() const{
			return m_mapLexemes.end();
		}

		const string_std &  GetAt(const lexeme_it & it) const {
			return (*it).first;
		}

		size_t GetCount() const{
			return m_mapLexemes.size();
		}

		void Clear(){
			m_mapLexemes.clear();
		}
	};

	//--------------------------------------------------------------------
	// sntx_analyze_context
	//--------------------------------------------------------------------
	//LexemeClass must be derived from grm_base_lexeme
	//SymbolClass must be derived from grm_base_sntx_term
	//RuleClass  must be derived from grm_base_sntx_rule
	template <class LexemeClass,class SymbolClass, class RuleClass>
	struct sntx_analyze_context 
	{
		bool		m_bFatalError;		// при установке этого флага необходимо завершить выполнение
		size_t		m_nRecursionLevel;
		size_t		m_nMaxRecursionLevel;
		size_t		m_nConjecturalSyntaxErrorLevel;
		text_location m_ConjecturalSyntaxErrorLocation;
		size_t		m_nRulesCompleted;	// Показывает количество узлов, участвовавших в разборе. Удобна для сравения различных способов оптимизации 
		typedef list<const RuleClass*> list_rules;
		typedef list<const SymbolClass*> list_symbols;
		
		list_rules m_ConjecturalErrorRules;
		list_symbols m_ConjecturalErrorTerms;

		void Clear(){
			m_bFatalError = false;
			m_nMaxRecursionLevel=m_nRecursionLevel=m_nConjecturalSyntaxErrorLevel=m_nRulesCompleted=0;
			m_ConjecturalSyntaxErrorLocation.Clear();
			m_ConjecturalErrorRules.clear();
			m_ConjecturalErrorTerms.clear();
		}

		sntx_analyze_context(){
			Clear();
		}
	};

	//--------------------------------------------------------------------
	//Class grm_base_sntx_term
	//LexemeClass must be derived from grm_base_lexeme
	//RuleClass must be derived from grm_base_sntx_rule
	//object of LexemeClass is created by this class
	//RuleClass is used only to store the pointers to existing objects.
	//--------------------------------------------------------------------
	template <class LexemeClass, class RuleClass>
	class grm_base_sntx_term
	{
	public:
		typedef size_t flags_t;
		//Flags for each item
		static const flags_t flagNone		= 0;
		static const flags_t flagOptional	= 1;
		static const flags_t flagChildOptional = 2;
		static const flags_t flagForbidBacktrack = 4;

		typedef grm_structs::sntx_error sntx_error;
	private:
		string_std				m_strLinkedRuleName;
		string_std	&			m_strRegExp;
		ETermClass				m_nClass;
		EStdTerminalClass		m_nStdTerminalClass;
		sntx_error				m_ErrorText;
		flags_t					m_nFlags; // See flags constants
		lexeme_group			m_TermGroup;
		void Init(){
			m_nClass=GE_NULL; 
			m_pTerminal=NULL; 
			m_szParentRuleName=NULL;
			m_strLinkedRuleName.erase();
			m_ErrorText.Clear();
			m_nFlags=flagNone;
			m_TermGroup.Clear();
			m_nStdTerminalClass = NO_STD_TERMINAL;
		}
	protected:
		union
		{
			LexemeClass*		m_pTerminal;
			RuleClass*			m_pNextRule;
		};

		void Clear(){
			if ((IsTerminal())&&(m_pTerminal)) 
				delete m_pTerminal;
			else if ((IsTerminalGroup())&&(m_pTerminal))
				delete m_pTerminal; 
			Init();
		}

		void SetError(const sntx_error & Err)	{m_ErrorText=Err;}
		void SetFlags(flags_t Flags)			{m_nFlags = Flags;}
		void SetClass(ETermClass nClass)			{m_nClass=nClass;}
	public:
		cstr	m_szParentRuleName; //Используется дл отладки

		//Операции определения типа элемента 
		bool IsNonterminal()		const	{return m_nClass==GE_NONTERMINAL;}
		bool IsNonterminalLinkByName() const {return m_nClass==GE_NONTERMINAL_LINK_BY_NAME;}
		bool IsTerminal()		const		{return m_nClass==GE_TERMINAL;}
		bool IsTerminalGroup()	const		{return m_nClass==GE_TERMINAL_GROUP;}
		bool IsTerminalStd()	const		{return m_nClass==GE_STANDARD_TERMINAL;}
		bool IsTerminalRegExp()	const		{return m_nClass==GE_REGEXP_TERMINAL;}
		bool IsTerminalEnd()	const		{return m_nClass==GE_END;}
		bool IsTerminalEmpty()	const		{return m_nClass==GE_EMPTY;}

		bool IsOptional()		const		{return (m_nFlags & flagOptional)!=0;}
		bool IsChildOptional()	const		{return (m_nFlags & flagChildOptional)!=0;}
		bool IsForbidBacktrack()const		{return (m_nFlags & flagForbidBacktrack)!=0;}

		ETermClass GetClass()					const	{return m_nClass;}
		EStdTerminalClass GetStdTerminalClass()	const	{_ASSERT(IsTerminalStd()); return m_nStdTerminalClass;}
		flags_t GetFlags()						const	{return m_nFlags;}
		cstr GetLinkedRuleName()				const	{_ASSERT(IsNonterminalLinkByName()|| IsNonterminal()); return m_strLinkedRuleName.c_str();}
		RuleClass& GetLinkedRule()				const	{_ASSERT(IsNonterminal()); return *m_pNextRule;}	
		LexemeClass & GetTerminal()				const	{_ASSERT(IsTerminal()); _ASSERT(m_pTerminal); return *m_pTerminal;}
		cstr GetRegExp()						const	{_ASSERT(IsTerminalRegExp()); return m_strRegExp.c_str();}

		const sntx_error& GetError()			const	{return m_ErrorText;}
		const lexeme_group& GetTermGroup()		const	{_ASSERT(IsTerminalGroup()); return m_TermGroup;}

		//Для оптимизации поиска. Если установить флаг, то откат от данного терминала 
		//назад уже свидетельствует о неминуемой ошибки
		void SetForbidBacktrackFlag(bool bValue)	{
			_ASSERT(IsTerminalStd() || IsTerminal() || IsTerminalGroup()); 
			if (bValue) m_nFlags|=flagForbidBacktrack; 
			else m_nFlags&=~flagForbidBacktrack;
		}

		string_smart GetTextRepresentation() const
		{
			switch(m_nClass) {
				case GE_TERMINAL_GROUP: return "GE_TERMINAL_GROUP";	//????
				case GE_NONTERMINAL: return  GetLinkedRuleName(); 
				case GE_NONTERMINAL_LINK_BY_NAME: return m_strLinkedRuleName.c_str();
				case GE_NULL: return "<<NULL>>";
				case GE_END: return "<<END>>";
				case GE_EMPTY: return "<<EMPTY>>";
				case GE_TERMINAL:
					switch (GetTerminal().GetClass()) {
						case CONST_CLASS_ID: return "<<CONSTANT>>";
						case ID_CLASS_ID: return "<<ID>>";
						default: return GetTerminal().GetSzText();
					}
				case GE_REGEXP_TERMINAL:
					return GetRegExp();
				case GE_STANDARD_TERMINAL:
					return GetStdTerminalClassName(GetStdTerminalClass());
				default: _ASSERT(false);
			}
			return string_smart("");
		}

		void LinkToRule(const RuleClass & SntxRule)	{
			_ASSERT(IsNonterminalLinkByName());
			m_nClass=GE_NONTERMINAL;
			m_pNextRule=const_cast<RuleClass*>(&SntxRule);
		}

		//save data to the stream
		virtual void write_dmp(base_utils::stream  & s){
			s.write_dmp(m_nFlags);
			s.write_dmp(m_nClass);

			if (IsNonterminal())
				s.write_bstr(m_strLinkedRuleName);
			else if (IsTerminal())
				m_pTerminal->write_dmp(s);
			else if (IsTerminalStd())
				s.write_dmp(m_nStdTerminalClass);
			else if (IsTerminalRegExp())
				s.write_bstr(m_strRegExp);
			
			s.write_bstr(m_ErrorText.GetName());
		}

		//load data from the stream
		virtual void read_dmp(base_utils::stream  & s){
			Clear();
			s.read_dmp(m_nFlags);
			s.read_dmp(m_nClass);

			if (IsNonterminal())
				m_strLinkedRuleName = s.read_bstr().c_str();
			else if (IsTerminal()){
				m_pTerminal=new LexemeClass(NO_CLASS);
				m_pTerminal->read_dmp(s);
			}
			else if (IsTerminalStd())
				s.read_dmp(m_nStdTerminalClass);
			else if (IsTerminalRegExp())
				m_strRegExp = s.read_bstr().c_str();
			
			m_ErrorText.SetName(s.read_bstr().c_str());
		}

		//----------------------------------------------------------------
		// Constructor/destructor
		//----------------------------------------------------------------

		// Class = GE_NULL
		grm_base_sntx_term ():m_strRegExp(m_strLinkedRuleName){
			Init();
		}

		// Class = GE_NULL,GE_EMPTY,GE_END
		grm_base_sntx_term (ETermClass nClass,flags_t Flags=flagNone):m_strRegExp(m_strLinkedRuleName){
			Init();
			m_nFlags = Flags;
			_ASSERT(oneof(nClass,GE_NULL,GE_EMPTY,GE_END));
			m_nClass=nClass;
		}

		//Class = GE_NONTERMINAL_LINK_BY_NAME
		grm_base_sntx_term (cstr szNextRule,flags_t Flags=flagNone):m_strRegExp(m_strLinkedRuleName){
			Init();
			m_nFlags = Flags;
			m_nClass=GE_NONTERMINAL_LINK_BY_NAME; 
			m_strLinkedRuleName= szNextRule;
		}

		// Class = GE_TERMINAL
		// no Back - оптимизационный прием. Означает, что после распознанного слова при возникновении ошибки искать другие варианты не надо
		// see define.h FORBID_BACK, PERMIT_BACK
		grm_base_sntx_term (const LexemeClass & Term, flags_t Flags=flagNone):m_strRegExp(m_strLinkedRuleName){
			Init();
			m_nFlags = Flags;
			m_nClass = GE_TERMINAL;
			m_pTerminal=new LexemeClass(Term);
		}

		// Class=GE_TERMINAL, создается new LexemeClass
		grm_base_sntx_term (ELexemeCLass clss, size_t Flags=flagNone):m_strRegExp(m_strLinkedRuleName){
			Init();
			m_nFlags = Flags;
			m_nClass = GE_TERMINAL;
			m_pTerminal=new LexemeClass(clss); 
		}

		//Class=GE_TERMINAL_GROUP; 
		grm_base_sntx_term (const lexeme_group & LG, size_t Flags=flagNone):m_strRegExp(m_strLinkedRuleName){
			Init();
			m_nFlags = Flags;
			m_nClass = GE_TERMINAL_GROUP;
			//m_pTerminal=new LexemeClass; 
			m_TermGroup = LG;
		}

		// Class=GE_STANDARD_TERMINAL
		grm_base_sntx_term (EStdTerminalClass StdTermClass, size_t Flags=flagNone):m_strRegExp(m_strLinkedRuleName){
			Init();
			m_nFlags = Flags;
			m_nClass = GE_STANDARD_TERMINAL;
			m_nStdTerminalClass=StdTermClass;
		}
		// Class=GE_REGEXP_TERMINAL
		grm_base_sntx_term (cstr szRegExp, double fDummy, size_t Flags=flagNone):m_strRegExp(m_strLinkedRuleName){
			Init();
			m_nFlags = Flags;
			m_nClass = GE_REGEXP_TERMINAL;
			m_strRegExp = szRegExp;
		}

		grm_base_sntx_term (const grm_base_sntx_term & src):m_strRegExp(m_strLinkedRuleName){
			Init();
			*this=src;
		}

		~grm_base_sntx_term(){
			Clear();
		}

		grm_base_sntx_term & operator=(const grm_base_sntx_term & src){
			Clear();
			m_szParentRuleName = src.m_szParentRuleName;
			m_strLinkedRuleName   = src.m_strLinkedRuleName;	//equal to m_strRegExp
			m_TermGroup=src.m_TermGroup;
			SetClass(src.GetClass());
			SetFlags(src.GetFlags());
			SetError(src.GetError());

			switch (GetClass())
			{
			case GE_TERMINAL_GROUP:
				m_TermGroup=src.m_TermGroup;//m_pTerminal=new LexemeClass;
				break;
			case GE_TERMINAL:
				m_pTerminal = new LexemeClass (src.GetTerminal());
				break;
			case GE_NONTERMINAL:
				m_pNextRule = src.m_pNextRule;
				break;
			case GE_STANDARD_TERMINAL:
				m_nStdTerminalClass = src.m_nStdTerminalClass;
				break;
			}
			return * this;
		}

		bool  operator ==(const grm_base_sntx_term & src) const
		{
			if (m_nClass!=src.m_nClass)	return false;
			switch(m_nClass) {
				case GE_TERMINAL:			return GetTerminal()==src.GetTerminal();
				case GE_TERMINAL_GROUP:		return m_TermGroup.GetCount()==src.m_TermGroup.GetCount(); // Внимание !!! В целях повышения производительности проверки массивов не производится
				case GE_STANDARD_TERMINAL:	return (m_nStdTerminalClass == src.m_nStdTerminalClass);
				case GE_REGEXP_TERMINAL:	return m_strRegExp==src.m_strRegExp;
				case GE_NONTERMINAL:		return m_pNextRule==src.m_pNextRule;
				case GE_NONTERMINAL_LINK_BY_NAME: return m_strLinkedRuleName==src.m_strLinkedRuleName;
				case GE_NULL:
				case GE_END:
				case GE_EMPTY:
					return true;
				default:
					_ASSERT(0);
			}
			return false;
		}
	};

	//--------------------------------------------------------------------
	// grm_base_sntx_variant
	//--------------------------------------------------------------------
	//LexemeClass must be derived from grm_base_lexeme
	//SymbolClass must be derived from grm_base_sntx_term
	template <class SymbolClass>
	class grm_base_sntx_variant
	{
	protected:
		vector<SymbolClass> m_lstItems;
		sntx_error m_ErrorText; 

		void AddTerm(const SymbolClass & src) {
			m_lstItems.push_back(src);
		}

		//Initializes the user-defined message about error, if any occures on the level of this variant
		void SetErrorText(const string_std & strText){
			m_ErrorText.SetName(strText);
		}
	public:
		typedef typename vector<SymbolClass>::iterator iterator;
		iterator Begin(){
			return m_lstItems.begin();
		}
		iterator End(){
			return m_lstItems.end();
		}

		void Clear() {
			m_lstItems.clear();
		}

		const SymbolClass& GetTerm(size_t nIndex) const {
			return m_lstItems[nIndex];
		}

		SymbolClass& GetTerm(size_t nIndex) {
			return m_lstItems[nIndex];
		}

		SymbolClass& GetLastTerm(){
			return m_lstItems.back();
		}

		SymbolClass& GetFirstTerm(){
			return m_lstItems.front();
		}

		size_t GetTermCount() const {
			return m_lstItems.size();
		}

		virtual void read_dmp(base_utils::stream & s)
		{
			size_t n; s>>n;
			for (size_t i = 0; i<n;i++)
			{
				SymbolClass x; x.read_dmp(s);
				AddTerm(x);
			}
		}

		virtual void write_dmp(base_utils::stream & s) 
		{
			s<<GetTermCount();
			for (size_t i = 0; i<GetTermCount();i++)
				GetTerm(i).write_dmp(s);
		}
	};

	//--------------------------------------------------------------------
	// grm_base_sntx_rule
	//--------------------------------------------------------------------
	//LexemeClass must be derived from grm_base_lexeme
	//SymbolClass must be derived from grm_base_sntx_term
	template <class VariantClass,class SymbolClass, int nHashSize>
	class grm_base_sntx_rule
	{
	public:
		typedef		vector<VariantClass>	list_variants;
		typedef		list <SymbolClass>		list_symbols;
		
		struct symbol_iterate_result {
			const SymbolClass & m_Symbol;
			size_t				m_nDeep;
			
			symbol_iterate_result(const SymbolClass & aSymbol, size_t nDeep): m_Symbol(aSymbol),m_nDeep(nDeep){
			}
			
			bool operator == (const symbol_iterate_result & src) const {
				return m_Symbol==src.m_Symbol; //не сравниваем m_nDeep
			}
		};
		
		typedef list<symbol_iterate_result>	list_symbol_iterate_results;
	private:
		string_std		m_strName;
		sntx_error		m_ErrorText; 
		list_variants	m_lstVariants;		

		void SetName(cstr szName)	{m_strName=szName;}
	protected:
		bool			m_bIterated;

		//Initializes the user-defined message about error, if any occures on the level of this hub
		void SetErrorText(const string_std & strText){
			m_ErrorText.SetName(strText);
		}
		
		void BeforeIterate() //Сбрасывает флаг m_bIterated
		{
			if (!m_bIterated)
				return;
			m_bIterated=false;
			for	(size_t	j=0;j<GetVariantCount();j++)
				for (size_t k = 0; k<GetVariant(j).GetTermCount();k++) 
					if (GetVariant(j).GetTerm(k).IsNonterminal())
						GetVariant(j).GetTerm(k).GetLinkedRule().BeforeIterate();
		}

		//Собирает информацию о том, какие терминалы входят в "алфавит"
		//данного правила
		void GetTerminalsSetImpl(list_symbol_iterate_results & lst, size_t nDeep)
		{
			if (m_bIterated)	//Защита от рекурсий
				return;

			m_bIterated=true;

			for	(size_t	j=0;j<GetVariantCount();j++)
				for (size_t k = 0; k<GetVariant(j).GetTermCount();k++) {
					const SymbolClass & GE = GetVariant(j).GetTerm(k);
					if (GE.IsTerminal() || GE.IsTerminalStd() || GE.IsTerminalGroup()){
						list_symbol_iterate_results::iterator it = find(lst.begin(),lst.end(),symbol_iterate_result(GE,nDeep));
						if (it==lst.end())
							lst.push_back(symbol_iterate_result(GE,nDeep));
						else 
							(*it).m_nDeep = min((*it).m_nDeep,nDeep);
					}
					else if (GE.IsNonterminal()) {
						GE.GetLinkedRule().GetTerminalsSetImpl(lst,nDeep+1);
					}
				}
		}
		
		//Собирает информацию о том, какие нетерминалы входят в "алфавит"
		//данного правила
		void GetNonterminalsSetImpl(list_symbol_iterate_results & lst, size_t nDeep)
		{
			if (m_bIterated)	//Защита от рекурсий
				return;

			m_bIterated=true;

			for	(size_t	j=0;j<GetVariantCount();j++)
				for (size_t k = 0; k<GetVariant(j).GetTermCount();k++) {
					const SymbolClass & GE = GetVariant(j).GetTerm(k);
					if (GE.IsNonterminal()) {
						list_symbol_iterate_results::iterator it = find(lst.begin(),lst.end(),symbol_iterate_result(GE,nDeep));
						if (it==lst.end()) {
							lst.push_back(symbol_iterate_result(GE,nDeep));
							GE.GetLinkedRule().GetNonterminalsSetImpl(lst,nDeep+1);
						}
						else {
							(*it).m_nDeep = min((*it).m_nDeep,nDeep);
						}
					}
				}
		}
		//Находит первые терминалы, в которые должен упереться разбор, 
		//по всем вариантам
		void GetFrontTerminalsImpl(list_symbol_iterate_results & lst, size_t nDeep)
		{
			if (m_bIterated)	//Защита от рекурсий
				return;

			m_bIterated=true;

			for	(size_t	j=0;j<GetVariantCount();j++)
				for (size_t k = 0; k<GetVariant(j).GetTermCount();k++)
				{
					const SymbolClass & GE = GetVariant(j).GetTerm(k);
					if (GE.IsTerminal() || GE.IsTerminalStd() || GE.IsTerminalGroup()) {
						list_symbol_iterate_results::iterator it = find(lst.begin(),lst.end(),symbol_iterate_result(GE,nDeep));
						if (it==lst.end())
							lst.push_back(symbol_iterate_result(GE,nDeep));
						else 
							(*it).m_nDeep = min((*it).m_nDeep,nDeep);
						break;
					}
					else if (GE.IsNonterminal()){
						GE.GetLinkedRule().GetFrontTerminalsImpl(lst,nDeep+1);
						break;
					}
				}

			m_bIterated = false;
		}

		//Находит первые терминалы, в которые должен упереться разбор, 
		//по всем вариантам
		grm_base_sntx_rule * DetectLeftSidedRecursionImpl ()
		{
			if (m_bIterated)	
				return this;

			grm_base_sntx_rule * pResult=NULL;

			m_bIterated=true;

			for	(size_t	j=0;j<GetVariantCount() && !pResult ;j++)
				for (size_t k = 0; k<GetVariant(j).GetTermCount();k++)
				{
					const SymbolClass & GE = GetVariant(j).GetTerm(k);
					if (GE.IsTerminal() || GE.IsTerminalStd() || GE.IsTerminalGroup()){
						break;
					}
					else if (GE.IsNonterminal()){
						pResult = GE.GetLinkedRule().DetectLeftSidedRecursionImpl();
						break;
					}
				}

			m_bIterated=false;
			return pResult;
		}
	public:
		grm_base_sntx_rule(cstr szName): m_strName(szName),m_bIterated(false){
		}

		~grm_base_sntx_rule(){
		}

		//The name of the hub
		cstr GetName()	const	 {
			return m_strName.c_str();
		}

		const sntx_error& GetError() const	{
			return m_ErrorText;
		}
	
		//----------------------------------------------------------------
		virtual void read_dmp(base_utils::stream & s)
		{
			SetName(s.read_bstr().c_str());
			SetErrorText(s.read_bstr().c_str());		//The description of syntax error;

			size_t nVariants =	s.read_dword();				//Количество деревьев
			for	(size_t	i=0; i<nVariants;i++)
			{
				VariantClass x; x.read_dmp(s);
				m_lstVariants.push_back(x);
			}

		}
		virtual void write_dmp(base_utils::stream & s)
		{
			s.write_bstr(GetName());
			s.write_bstr(GetError().GetName());			//The description of syntax error;

			s.write_dword(GetVariantCount());				//Количество деревьев
			for	(size_t	j=0;j<GetVariantCount();j++)
				(GetVariant(j)).write_dmp(s);
		}

		//----------------------------------------------------------------
		void AddVariant(const VariantClass & src){
			m_lstVariants.push_back(src);
		}

		VariantClass &	GetVariant(size_t nIndex)				{_ASSERT(nIndex<GetVariantCount()); return m_lstVariants[nIndex];}
		const VariantClass & GetVariant(size_t nIndex) const	{_ASSERT(nIndex<GetVariantCount()); return m_lstVariants[nIndex];}
		size_t	GetVariantCount()const							{return m_lstVariants.size();}


		//Находит первые терминалы, в которые должен упереться разбор, 
		//по всем вариантам
		void GetFrontTerminals(list_symbol_iterate_results & lst)
		{
			BeforeIterate();
			GetFrontTerminalsImpl(lst,0);
		}
		
		//Собирает информацию о том, какие терминалы входят в "алфавит"
		//данного правила
		void GetTerminalsSet(list_symbol_iterate_results & lst)
		{
			BeforeIterate();
			GetTerminalsSetImpl(lst,0);
		}
		
		//Собирает информацию о том, какие нетерминалы входят в "алфавит"
		//данного правила
		void GetNonterminalsSet(list_symbol_iterate_results & lst)
		{
			BeforeIterate();
			GetNonterminalsSetImpl(lst,0);
		}

		//Ищет замкнутый левостронний цикл
		grm_base_sntx_rule * DetectLeftSidedRecursion()
		{
			BeforeIterate();
			return DetectLeftSidedRecursionImpl();
		}

	};

	//--------------------------------------------------------------------
	//grm_base_sntx_analyzer
	//--------------------------------------------------------------------
	template <class RuleClass>
	class grm_base_sntx_analyzer
	{
	protected:
		typedef vector<RuleClass*> vector_sntx_rules;
		typedef typename vector_sntx_rules::iterator vector_sntx_rules_it;  
		typedef typename vector_sntx_rules::const_iterator vector_sntx_rules_cit;  

		vector_sntx_rules	m_arrRules;
		RuleClass*			m_pRoot;
	public:
		void Clear() {
			for (vector_sntx_rules_it i = m_arrRules.begin(); i!=m_arrRules.end(); i++)
				delete (*i);
			m_arrRules.clear();
		}
	
		void AddRule(const RuleClass & Rule) {
			m_arrRules.push_back(new RuleClass(Rule));
		}

		RuleClass & GetRoot() const {
			_ASSERT(m_pRoot); 
			return *m_pRoot;
		}

		void SetRoot(RuleClass *pRoot){
			_ASSERT(pRoot); 
			m_pRoot=pRoot;
		}

		RuleClass& GetRule(size_t nIndex) const{
			return *m_arrRules[nIndex];
		}

		size_t GetRuleCount() const {
			return m_arrRules.size();
		}
		
		//stores data to the stream		
		void write_dmp(base_utils::stream & s)
		{	
			s.write_dword(GetRuleCount());
			for (vector_sntx_rules_it i=m_arrRules.begin();i!=m_arrRules.end();++i) 
				(*i)->write_dmp(s);
		} 
		
		//reads data from the stream
		void read_dmp(base_utils::stream & s)
		{
			Clear();
			_ASSERT(GetRuleCount()==0);
			_ASSERT(s.isopen());

			size_t nRules = s.read_dword();
			for	(size_t	i=0;i<nRules;i++)
			{
				RuleClass Rule;
				Rule.read_dmp(s);
				AddRule(Rule);
			}
		}
	};

	//--------------------------------------------------------------------
	// object
	//--------------------------------------------------------------------
	class object 
	{
		variant m_variant;
	public:
		enum type{	vtNULL	= variant::vtNULL,
					vtBOOL	= variant::vtBOOL,		
					vtCHAR	= variant::vtCHAR,
					vtSMALL = variant::vtSMALL,
					vtSHORT = variant::vtSHORT,
					vtINT	= variant::vtINT,
					vtDOUBLE= variant::vtDOUBLE,
					vtSTRING= variant::vtSTRING,
					vtVARIANT=100
				};
	protected:
//		type m_nType;
	public:
		static variant::type object2variant(object::type aType){
			return (aType==vtVARIANT) ? variant::vtSTRING : (variant::type) aType;
		}

		static object::type variant2object(variant::type aType){
			return (object::type) aType;
		}

		static bool isordinal(type aType){
			return (aType!=vtVARIANT) && (variant::isordinal(object2variant(aType)));
		}
		
		static	bool isnumerical(type aType){
			return (aType!=vtVARIANT) && (variant::isnumerical(object2variant(aType)));
		}
		static	bool isreal(type aType){
			return (aType!=vtVARIANT) && (variant::isnumerical(object2variant(aType))) && (!variant::isordinal(object2variant(aType)));
		}
		static size_t size_of(type aType){
			return (aType==vtVARIANT) ? sizeof(variant) : variant::size_of(object2variant(aType));
		}
		static cstr get_type_string(type aType){
			return (aType==vtVARIANT) ? "variant" : variant::get_type_string(object2variant(aType));
		}
	};

	//--------------------------------------------------------------------
	//общая для всез проектов часть представления типа данных
	class type
	{
	public:
		enum type_class{T_SIMPLE, T_STRUCTURE,T_NO_CLASS};
	protected:

		string_std		m_strName;
		size_t			m_nID;		// индекс этого элемента в таблице
		size_t			m_nMemSize;		
		type_class		m_nClass;
		
		//---- for simple type ----
		object::type	m_nType; // variant::vtNULL, variant::vtCHAR, variant::vtBOOL and etc,
	public:	
		type():m_nMemSize(0),m_nClass(T_NO_CLASS),m_nID(0),m_nType(object::vtNULL){
		}

		//Эти функции только для структур ------------------------------------
		virtual size_t GetFieldCount() const =0;

		//Только функции для  IsSimple ---------------------------------------
		bool IsOrdinal() const{
			_ASSERT(IsSimple());
			return object::isordinal(m_nType);
		}

		bool IsReal() const{
			_ASSERT(IsSimple());
			return object::isreal(m_nType);
		}

		bool IsString() const{
			_ASSERT(IsSimple());
			return m_nType==object::vtSTRING;
		}

		bool IsVariant() const{
			_ASSERT(IsSimple());
			return m_nType==object::vtVARIANT;
		}

		//Эти функции для любого типа ----------------------------------------
		bool IsSimple() const {
			return m_nClass==T_SIMPLE;
		}

		bool IsStruct() const {
			return m_nClass==T_STRUCTURE;
		}

		object::type GetType() const {
			return m_nType;
		}

		type_class GetTypeClass() const	{
			return m_nClass;
		}
		
		size_t GetTypeID() const{
			return m_nID;
		}

		cstr GetTypeName() const {
			return m_strName.c_str();
		}

		size_t GetMemSize() const{
			return m_nMemSize;
		}

	};
	//--------------------------------------------------------------------
	//структуры для описания функций
	//--------------------------------------------------------------------
	typedef object::type var_type;
	
	const object::type at_bool		= object::vtBOOL;
	const object::type at_char		= object::vtCHAR;
	const object::type at_small		= object::vtSMALL;
	const object::type at_short		= object::vtSHORT;
	const object::type at_int		= object::vtINT;
	const object::type at_double	= object::vtDOUBLE;
	const object::type at_string	= object::vtSTRING;
	const object::type at_void		= object::vtNULL;
	const object::type at_variant	= object::vtVARIANT;
	
	enum pass_type {pt_byval, pt_byref};

	//Тип, определяющий выравнивание данных
	enum data_align{daBYTE=1,daWORD=2, daDWORD=4};

	//Представление типов данных в текстовом виде
	inline cstr var_type_cstr(var_type aType)
	{
		return object::get_type_string(aType);
	}

	//Представление типа передачи 
	inline cstr pass_type_cstr(pass_type aType)
	{
		switch (aType) {
			case pt_byval: return "";
			case pt_byref: return "&";
			default:
				_ASSERT(false);
		}
		return "";
	}

	struct arg_data
	{
	private:
		class variant_friend: public variant{
			friend struct arg_data;
		};
	public:
		var_type	m_VarType;
		pass_type	m_PassType;
		
		//Подсчитывает размер необходимой памяти для данного параметра в памяти
		//Параметр align указывает на способ выравнивания 
		size_t mem_size(data_align nAlign) const {	
			return (m_PassType==pt_byref) 
					? ceilgran(sizeof(void*),nAlign)				//Только адрес
					: ceilgran((int)object::size_of(m_VarType),nAlign);	//Полностью данные
		}

		//Укладывает значение переданного variant в виде дампа памяти c учетом 
		//своего типа. Необходимо помнить, что в случае с ссылкой
		//в дамп будет записан адрес переданного value, т.е. уничтожать этот value 
		//до использования дампа нельзя 
		raw_buffer<byte_t> mem_dmp(const variant & value, data_align nAlign) const {
			raw_buffer<byte_t> res(mem_size(nAlign));
			res.zero();
			//Передача адреса
			if (m_PassType==pt_byref){
				switch (m_VarType)
				{
					
				case at_variant:res.copy_value(&value); break;//копируем адрес
/*????*/		case at_void  : res.copy_value(& ((variant_friend&)value).m_iVal); break;
				case at_bool  : res.copy_value(& ((variant_friend&)value).m_bVal);
				case at_char  :
				case at_small : res.copy_value(& ((variant_friend&)value).m_cVal); break;
				case at_short : res.copy_value(& ((variant_friend&)value).m_wVal); break;
				case at_int   : res.copy_value(& ((variant_friend&)value).m_iVal); break;
				case at_double: res.copy_value(& ((variant_friend&)value).m_fVal); break;
				case at_string: res.copy_value(  ((variant_friend&)value).m_strVal.buffer()); break;
				default:
					_ASSERT(false);
				}
				
			}
			//Передача variant
			else if (m_VarType==at_variant)
				res.copy_value(value);
			else if (m_VarType==value.get_type())
				value.write_dump(res.ptr());
			else{
				variant v = value; v.set_type(object::object2variant(m_VarType));
				v.write_dump(res.ptr());
			}
			return res;
		}

		//Представление в текстовом виде
		string_smart str() const {
			return string_smart(var_type_cstr(m_VarType)).cat(pass_type_cstr(m_PassType));
		}
	};
	
	struct func_data
	{
		char_t		m_szLibName[255];	//Путь к библиотеке
		char_t		m_szFuncName[255];	//Под название функции отведено максимум 255 символов
		arg_data	m_RetValue;
		size_t		m_nArgCount;		//Кол-во действительных элементов в m_Args;
		arg_data	m_Args[255];		//Столь некрасивый способ, тем не менее избавляет нас от необходимости выделения памяти
		char_t		m_szCategory[128];	//Название категории, к которой принадлежит функция, например, Strings, Files, Date And Time
		char_t		m_szDesciption[1024];//Описание. 


		//Представление в текстовом виде
		string_smart str() const{
			string_smart s(m_RetValue.str()," ",m_szFuncName," (");
			for (size_t i=0; i<m_nArgCount;i++){
				s.cat(m_Args[i].str().c_str());
				if (i<m_nArgCount-1) s.cat(", ");
			}
			return s.cat(")");
		}
		bool operator == (const func_data & src) const{
			return	memcmp(this,&src,sizeof(func_data))==0;
		}
		
		const func_data & write_dump(stream & s) const{
			s.write_bstr(m_szLibName); 
			s.write_bstr(m_szFuncName);
			s.write_dmp(m_RetValue);
			s.write_dmp(m_nArgCount);
			s.write(m_Args,m_nArgCount*sizeof(m_Args[0]));
			return *this;
		}
		func_data & read_dump(stream & s){
			strcpy(m_szLibName,s.read_bstr().c_str());
			strcpy(m_szFuncName, s.read_bstr().c_str());
			s.read_dmp(m_RetValue);
			s.read_dmp(m_nArgCount);
			s.read(m_Args,m_nArgCount*sizeof(m_Args[0]));
			memset(&m_Args[m_nArgCount],0,(arrlength(m_Args)-m_nArgCount)*sizeof(m_Args[0]));
			return *this;
		}
		
		//Подсчитывает размер необходимой памяти для размещения всех параметров 
		//Параметр align указывает на способ выравнивания каждого элемента
		//на заданную границу. Имеет смысл указывать 1,2,4
		size_t calc_args_mem(data_align nAlign) const {
			size_t res=0;
			for (size_t i=0; i<m_nArgCount;i++)
				res+=m_Args[i].mem_size(nAlign);
			return res;
		}
	};
};
