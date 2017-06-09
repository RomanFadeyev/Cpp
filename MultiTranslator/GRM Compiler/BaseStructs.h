
#if !defined(AFX_BASE_STRUCTS_H__0939AA68_7120_11CF_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_BASE_STRUCTS_H__0939AA68_7120_11CF_A1F4_0000E8D9F1D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"
#include <limits>
#include "Define.h"
#include "DefineCommands.h"
#include <GrmKrnlExports.h>

interface ISmntAnalyzeContext;
interface ILexAnalyzeContext;
interface ILexList;
interface ISntxList;

typedef bool ret_t;
typedef ret_t (*pf_lex_func1) ();
typedef ret_t (*pf_lex_func2) (const ILexAnalyzeContext &);
typedef ret_t (*pf_sntx_func) (const ISmntAnalyzeContext & src);

extern bool UserBreakFlag;
extern char_t VALID_SYMBOLS_FOR_ID[];
extern char_t VALID_SYMBOLS_FOR_CONST[];

//using namespace grm_structs;

class CSntxRule;
//class sntx_variant;

namespace structs
{
	//Redeclaration of the basic types in the scope of our workspace
	typedef grm_structs::sntx_error sntx_error;
	typedef grm_structs::object object;
	typedef grm_structs::located_text located_text;
	typedef grm_structs::located_typed_text located_typed_text;
	typedef grm_structs::text_location text_location;
	typedef grm_structs::text_pos text_pos;
	typedef grm_structs::func_data func_data;
	typedef grm_structs::text_container text_container;
	typedef grm_structs::litera litera;
};

namespace structs
{
	class type;

	//--------------------------------------------------------------------
	// Lexeme
	//--------------------------------------------------------------------
	class lexeme: public grm_structs::grm_base_lexeme
	{
	protected:
		//Этот метод доступен через located_typed_text. От прямого использования он скрыт
		override cstr GetSzTypeName()	const;
	public:
		size_t			Index; // индекс размещения в лексичесокй таблице. Вместе m_nClass & Index представляют собой имя переменной
		structs::type*	pType; // для переменных, чей тип находится в таблице, предполагается, что будет использоваться при генерации кода для передачи через стек

		void Set (ELexemeCLass id, int index, const string_std & strText=STR_NULL)
		{
			SetClass(id); 
			SetText(strText);
			Index=index; 
		}

		void Set (const string_std & strText)
		{
			m_strText=strText;
		}

		lexeme (const lexeme & src){
			*this=src;											
		}

		lexeme	(ELexemeCLass id): grm_base_lexeme(id), Index(0), pType(NULL){
		}

		lexeme	(ELexemeCLass id, int index): grm_base_lexeme(id), Index(index), pType(NULL){
		}

		lexeme	(ELexemeCLass id, int index, const string_std & strText): grm_base_lexeme(id,strText), Index(index), pType(NULL){
		}

		lexeme	(): Index(0), pType(NULL){
		}

		bool  operator ==(const lexeme & src) const
		{
			if ((GetClass()==src.GetClass()) && (Index==src.Index))
				return true;
			return false;
		}
	};

	//--------------------------------------------------------------------
	// lexeme_group
	//--------------------------------------------------------------------
	typedef grm_structs::lexeme_group lexeme_group;


	//--------------------------------------------------------------------
	// grm_term
	//--------------------------------------------------------------------
	typedef grm_structs::grm_base_sntx_term<structs::lexeme, CSntxRule> grm_term_t;
	class grm_term: public grm_term_t
	{
	public:
		pf_sntx_func	m_pFunc;

		//Class=GE_TERMINAL_GROUP; 
		grm_term (const lexeme_group & LG, const sntx_error& Err=sntx_error(NULL),pf_sntx_func F=NULL,size_t Flags=flagNone );

		// Class = GE_NULL
		grm_term (pf_sntx_func F=NULL);			

		// Class = GE_NULL,GE_EMPTY,GE_END
		grm_term (ETermClass nClass, pf_sntx_func F);

		// Class = GE_TERMINAL
		// no Back - оптимизационный прием. Означает, что после распознанного слова при возникновении ошибки искать другие варианты не надо
		// see define.h FORBID_BACK, PERMIT_BACK
		grm_term (const structs::lexeme & Term,pf_sntx_func F=NULL, size_t Flags=flagNone);
		
		// Class = GE_TERMINAL
		// no Back - оптимизационный прием. Означает, что после распознанного слова при возникновении ошибки искать другие варианты не надо
		// see define.h FORBID_BACK, PERMIT_BACK
		grm_term (const structs::lexeme & Term, const sntx_error& Err,pf_sntx_func F=NULL, size_t Flags=flagNone);
		
		//Class = GE_NONTERMINAL_LINK_BY_NAME
		grm_term (cstr szNextHub,pf_sntx_func F=NULL,size_t Flags=flagNone);
		
		// Class=GE_TERMINAL, создается new structs::lexeme
		grm_term (ELexemeCLass clss, pf_sntx_func F=NULL,size_t Flags=flagNone);
		
		// пустой элемент. Нужен в случае необходимости добавить несколько функций к одному элементу в правиле
		// в этом случае и вставляются пустые элементы
		// первый параметр служит только для отличия от других конструкторов
		// Class=GE_EMPTY
		grm_term (double EmptyElementFlag,pf_sntx_func F=NULL );


		grm_term (const grm_term & src);
		~grm_term();

		grm_term & operator=(const grm_term & src);
		bool  operator ==(const grm_term & src) const;
	};

	//--------------------------------------------------------------------
	// sntx_variant
	//--------------------------------------------------------------------
	typedef grm_structs::grm_base_sntx_variant<structs::grm_term> grm_sntx_variant_t;
	class sntx_variant: public grm_sntx_variant_t
	{
	};

	//--------------------------------------------------------------------
	// sntx_rule
	//--------------------------------------------------------------------
	typedef grm_structs::grm_base_sntx_rule<sntx_variant, structs::grm_term,ALPHABET_SIZE> sntx_rule_t;
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

	//--------------------------------------------------------------------
	//sntx_analyze_context
	//--------------------------------------------------------------------
	typedef grm_structs::sntx_analyze_context<structs::lexeme, structs::grm_term, CSntxRule> sntx_analyze_context_t;
	class sntx_analyze_context:public sntx_analyze_context_t
	{
	private:
		base_utils::file_std m_DiagnosticFile;
	public:
		const ILexList *	m_pLexList; //ReadOnly
		ISntxList *			m_pSntxtList;
		sntx_error			m_ConjecturalSyntaxError;
		CSntxRule*			m_pConjecturalErrorRule;

		sntx_analyze_context(): m_pConjecturalErrorRule(NULL){
		}

		bool OpenDiagnosticFile(cstr szFileName){
			return m_DiagnosticFile.open(szFileName,"wt");
		}
		
		void CloseDiagnosticFile(){
			m_DiagnosticFile.close();
		}
		
		void WriteToDiagnosticFile(cstr szText,...)
		{
			if (!m_DiagnosticFile)
				return;

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

			if (m_nRecursionLevel>=1)	//В разборе всегда m_nRecursionLevel не может быть <1
				m_DiagnosticFile<<strnchar(' ',(m_nRecursionLevel-1)*2);
			m_DiagnosticFile<<strRes;
			m_DiagnosticFile.flush();
		}

		const ILexList & GetLexList() const { //Read Only
			_ASSERT(m_pLexList); return *m_pLexList;
		}

		ISntxList & GetSntxList() const { //Read/Write
			_ASSERT(m_pSntxtList); return *m_pSntxtList;
		}
	};
};

enum ESntxIDProp {NO_PROP=0,BY_REF};

struct SSntxID: public structs::located_typed_text
{
protected:
	size_t				m_nElementsCount;		// для массива
public:
	SSntxID()
		:m_nReferencesCount(0),m_nElementsCount(1),pType(NULL)
	{
	}

	SSntxID(structs::type* ptype, const located_text & Object, size_t nElementsCount=1, ESntxIDProp Prop= NO_PROP)
		:m_nReferencesCount(0), m_nElementsCount(nElementsCount)
	{ 
		located_text::operator = (Object);
		this->pType=ptype;
		this->ExtraProperties=Prop;
		_ASSERT(m_nElementsCount!=0);
	}	
	
	override cstr GetSzTypeName()	const;

	bool IsByRef() const	{return ExtraProperties==BY_REF;}
	bool IsByVal() const	{return ExtraProperties!=BY_REF;}
	bool IsArray() const	{return m_nElementsCount>1;}

	//Для массива
	size_t	GetSize()const	{return m_nElementsCount;}	

	ESntxIDProp ExtraProperties;	//  доп свойства	
	structs::type*		pType; // only pointer, not Object!!!
	mutable size_t		m_nReferencesCount;
};

namespace structs
{
	class type: public grm_structs::type
	{
	protected:
		//---- for structure
		struct field_data{
			SSntxID			m_Field;
			size_t			m_nOffset;
			string_std		m_strName;
			
			field_data(const SSntxID & Field,size_t nOffset): m_Field(Field),m_nOffset(nOffset),m_strName(Field.Get_Str_Text()){
			}
		};
		typedef vector<field_data>	list_field;
		list_field m_lstFields;

		type(cstr szTypeName): grm_structs::type(){
			m_strName=szTypeName;
		}
	public:
		type(const type &src)	{
			*this=src;
		}

		~type(){
		}

		//Эти функции только для структур ------------------------------------
		override size_t GetFieldCount() const{
			return m_lstFields.size();
		}

		const SSntxID & GetFieldAt(size_t nIndex) const{
			_ASSERT(nIndex<GetFieldCount());
			return m_lstFields[nIndex].m_Field;
		}

		bool AddField(SSntxID & ID)
		{
			_ASSERT(IsStruct());
			if (FindField(ID.Get_Str_Text()))
				return false;

			m_lstFields.push_back(field_data(ID,m_nMemSize));
			m_nMemSize+=ID.pType->m_nMemSize*ID.GetSize();
			return true;
		}

		bool FindField(const string_std & strFieldName) const
		{
			_ASSERT(IsStruct());
			SSntxID s;
			return FindField(strFieldName,s);
		}

		bool FindField(const string_std & strFieldName, SSntxID & ResID) const
		{
			_ASSERT(IsStruct());
			size_t Index;
			return FindField(strFieldName,ResID,Index);
		}

		bool FindField(const string_std & strFieldName, SSntxID & ResID, size_t & Index) const
		{
			_ASSERT(IsStruct());
			_ASSERT(!strFieldName.isempty());
			for (size_t i = 0; i<m_lstFields.size();++i)
			{
				const field_data & data = m_lstFields[i];
				if (data.m_strName==strFieldName){
					ResID = data.m_Field;
					Index=i;
					return true;
				}
			}
			return false;	
		}
	};

	class code_operand:public grm_structs::located_text
	{
	public:
		enum op_class{	ocEmpty =0, 
						ocConst	='C',	//Constant
						ocID	='V',	//IDentificator
						ocLabel ='L',	//Label
						ocType  ='T',	//Type
						ocAddr	=' ',	//Direct Address
						ocFunc	='F',	//Function 
						ocSub	='S'};	//Subroutine
	private:
		op_class m_nClass;
	public:
		char_t GetSignature()	const				{return m_nClass;}
	};
	//--------------------------------------------------------------------
	// class command
	//--------------------------------------------------------------------
	class code_cmd
	{
	public:
		structs::lexeme Op1;
		structs::lexeme Op2;
		structs::lexeme Op3;
		structs::lexeme Op4;
		ECodeOperation Operation;

		code_cmd()	{};

		code_cmd(ECodeOperation Operation)	{
			this->Operation=Operation;
			//Op1, Op2 and Op3 automatically set  CLASS=NO_CLASS, TYPE=DBVT_NULL and etc.
		}

		code_cmd(structs::lexeme Op1, ECodeOperation Operation) {
			this->Op1=Op1;
			this->Operation=Operation;
			// Op2 and Op3 automatically set  CLASS=NO_CLASS, TYPE=DBVT_NULL and etc.
		}
		code_cmd(structs::lexeme Op1, structs::lexeme Op2,ECodeOperation Operation) {
			this->Op1=Op1;
			this->Op2=Op2;
			this->Operation=Operation;
		}
		code_cmd(structs::lexeme Op1, structs::lexeme Op2, structs::lexeme Op3,ECodeOperation Operation) {
			this->Op1=Op1;
			this->Op2=Op2;
			this->Op3=Op3;
			this->Operation=Operation;
		}
		code_cmd(structs::lexeme Op1, structs::lexeme Op2, structs::lexeme Op3,structs::lexeme Op4,ECodeOperation Operation) {
			this->Op1=Op1;
			this->Op2=Op2;
			this->Op3=Op3;
			this->Op4=Op4;
			this->Operation=Operation;
		}

		void Clear() {
			Op1.Clear();
			Op2.Clear();
			Op3.Clear();
			Op4.Clear();
			Operation=NOP;
		}

		size_t GetSourceLine() const {

			size_t nLine=0,nLn;
			
			nLn=((Op1.GetStartPos().m_nLine==0)  ?Op1.GetEndPos().m_nLine :Op1.GetStartPos().m_nLine);
			if (nLine<nLn) nLine=nLn;
			nLn=((Op2.GetStartPos().m_nLine==0)  ?Op2.GetEndPos().m_nLine :Op2.GetStartPos().m_nLine);
			if (nLine<nLn) nLine=nLn;
			nLn=((Op3.GetStartPos().m_nLine==0)  ?Op3.GetEndPos().m_nLine :Op3.GetStartPos().m_nLine);
			if (nLine<nLn) nLine=nLn;
			nLn=((Op4.GetStartPos().m_nLine==0)  ?Op4.GetEndPos().m_nLine :Op4.GetStartPos().m_nLine);
			if (nLine<nLn) nLine=nLn;

			return nLine;
		}
	};
			
};



//------------------------------------------------------------------------
//Интерфейсы
//------------------------------------------------------------------------
interface ILexSource{
	typedef size_t state;

	virtual bool IsEOF()				const =0;
	virtual void MoveToBegin()			const =0;
	virtual void MoveBack()				const =0;
	virtual void MoveForward()			const =0;
	
	virtual  state StoreState() const =0;
	virtual  void  RestoreState(state s)=0;

	virtual const structs::litera		GetCurSymbol()		const =0;
	virtual const structs::litera		GetNextSymbol()		const =0;

	virtual const string_std&			GetFileName()		const =0;
	virtual structs::text_pos		GetCurPos()			const =0;
	virtual structs::text_location	GetCurLocation()	const =0;

};

interface ILexList
{
	virtual void Add(const structs::lexeme & src)=0;

	virtual const structs::lexeme & GetAt(size_t nIndex)	const =0;
	virtual const structs::lexeme & Get()					const =0;

	virtual size_t	GetCount()						const =0;

	virtual size_t	GetPos()						const =0;
	virtual void	SetPos(size_t p)				const =0;
	virtual bool	IsEnd()							const =0;
	virtual bool	IsStart()						const =0;

	virtual void	Next()							const =0;
	virtual void	Prev()							const =0;

	// these functions don't move pointer 
	virtual const structs::lexeme & GetPrev()		const =0;
	virtual const structs::lexeme & GetNext()		const =0;
};

//TermClass must be derived from grm_base_sntx_term
interface ISntxList
{
	virtual void Add(const structs::grm_term & aItem)=0;

	virtual structs::grm_term & GetAt(size_t nIndex)=0;	
	virtual structs::grm_term & GetBack()=0;

	virtual size_t	GetCount()	const =0;

	virtual void	EraseFrom(size_t nPos)	=0;
	virtual void	Reserve(size_t nSize)	=0;
};

interface IConstTable{
	virtual size_t			GetConstantCount() const =0;
	virtual const variant&	GetConstant(size_t Index) const =0;
	virtual cstr			GetConstantName(size_t Index) const =0;
};

interface ILexTable: public IConstTable{
	virtual const structs::lexeme & AddID			(cstr src) =0;
	virtual const structs::lexeme & AddConst		(cstr src,variant::type nType) =0;
	virtual const structs::lexeme & GetReservedWord	(cstr src) const =0; 
	virtual const structs::lexeme & GetOperator		(cstr src) const =0; 
	virtual size_t GetIndex(ELexemeCLass Class, const cstr src) const =0;
};

interface ITypeTable{
	typedef structs::object object;
	virtual bool CreateSimple(cstr szTypeName, object::type VarType)=0; 
	virtual bool CreateStructure(cstr szTypeName) =0;
	virtual bool AddFieldToLastStructure(SSntxID &ID)=0;

	virtual bool FindType(cstr szTypeName,const structs::type*& TypePtr)const=0;
	virtual bool FindType(cstr szTypeName)								const=0;
	//Ищет под заданный тип variant::type подходящий стандартный тип в таблице
	virtual bool FindStdType(const object::type nVarType,const structs::type*& pType)const=0;

	virtual size_t GetCount()							const =0;
	virtual const structs::type& GetAt(size_t nIndex)	const =0;
};

interface IImportTable{
	typedef structs::func_data func_data;
	enum find_function_result{ffrOK, ffrHasNoFunction, ffrHasNoEntryPoint};

	virtual bool AddLibary(cstr szPath)=0; 
	virtual find_function_result FindFunction(cstr szFuncName, OUT func_data & fdata,OUT size_t & nID) const =0;
	virtual bool AddFunction(cstr szLibName, cstr szFuncName, const func_data & fdata, OUT size_t & nID) =0;

	//Кол-во используемых функций, которые уже занесены в таблицу
	virtual size_t	GetCount()							const =0;
	virtual const func_data & GetAt(size_t)				const =0;
};

interface ILangGenTable{
	virtual bool AddLibary(cstr szPath)=0; 
	virtual void OnGrmItem(cstr szRuleName,cstr szVariantName, cstr szItemName,size_t nItemID) const =0;
};

interface ILexAnalyzeContext{
	virtual ILexSource &	GetLexSource()				const =0;
	virtual ILexList&		GetLexList()				const =0;
	virtual ILexTable&		GetLexTables()				const =0;
	virtual	const structs::litera & GetEtalonLitera()	const =0;
};

interface ISntxTable
{
	virtual bool RegisterID (const SSntxID & ID) =0;	// return false if ID already exists
	virtual bool RegisterID (const SSntxID &ID, size_t & Level, size_t & Index) =0;
	virtual void IncLevel() = 0;
	virtual void DecLevel() = 0; 

	virtual bool Find(const string_std& strName) const =0;	
	virtual bool Find(const string_std& strName, size_t &Level) const =0;
	virtual bool Find(const string_std& strName, SSntxID & dest) const =0;
	virtual bool Find(const string_std& strName, SSntxID & dest, size_t &Level) const =0;
	virtual bool Find(const string_std& strName, SSntxID & dest, size_t & Level, size_t & Index) const =0;
	
	virtual bool GetUnreferencedFirst(SSntxID & dest)	const =0;
	virtual bool GetUnreferencedNext(SSntxID & dest)	const =0; 

	virtual bool SetStructCurrent(structs::type * Struct) =0;
	virtual structs::type* GetStructCurrent()=0;
	virtual bool ClearStructCurrent()=0;
	
	virtual size_t GetLevel()				const = 0;
};

interface IPCodeList
{
	virtual void Add(const structs::code_cmd & src)=0;
	virtual void InsertFront(const structs::code_cmd & src)=0;
	virtual void CreateNewFragment()=0;
	virtual void DestroyFragment(bool AppendToParent)=0;

	virtual size_t	GetCount()						const =0;
	virtual const structs::code_cmd & GetAt(size_t)		const =0;
	
	virtual void	EraseFrom(long nPos)	=0;
	virtual void	Reserve(size_t nSize)	=0;
};

interface ISmntAnalyzeContext{
	virtual	const structs::lexeme&  GetLexeme()				const =0;
	
	virtual const ILexList&		GetLexList()		const =0;
	virtual const ISntxList&	GetSntxList()		const =0;
	virtual const ILexTable&	GetLexTable()		const =0;
	
	virtual	const ILangGenTable&GetLangGenTable()	const =0;
	virtual IImportTable&	GetImportTable()	const =0;

	virtual ISntxTable&		GetSntxTable()		const =0;
	virtual ITypeTable&		GetTypeTable()		const =0;

	virtual	IPCodeList&		GetPCodeList()		const =0;
};

interface IBuildContext{
	virtual string_smart GetOutputDirectory() const =0;
	virtual string_smart GetIntermediateDirectory() const =0;
	virtual string_smart GetResultFileName() const =0;
};

#endif // !defined(AFX_BASE_STRUCTS_H__0939AA68_7120_11CF_A1F4_0000E8D9F1D7__INCLUDED_)
