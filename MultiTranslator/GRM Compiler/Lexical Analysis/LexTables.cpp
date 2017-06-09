/********************************************************************
	created:	2003/06/11
	created:	11:6:2003   11:57
	filename: 	$(Projects)\sources\grm compiler\lexical analysis\lextables.cpp
	file path:	$(Projects)\sources\grm compiler\lexical analysis
	file base:	lextables
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "LexTables.h"
#include "EWQueue.h"

#ifdef __DIAGNOSTIC__ 
	#define REPORT(x)	WriteToDiagnosticFile("Adding %s\n",static_cast<cstr>(x));
#else
	#define REPORT(x)
#endif

CLexTable::CLexTable(const IBuildContext & BuildContext,cstr szFileName)
{
	string_smart strFileName (include_backslash(BuildContext.GetIntermediateDirectory()),extract_filename(szFileName),".lextbl");
	
	remove(strFileName.c_str());

	#ifdef __DIAGNOSTIC__
	if (!m_DiagnosticFile.open(strFileName.c_str(),"wt"))
		errors::w_file_was_not_opened(strFileName.c_str());
	#endif
}
//------------------------------------------------------------------------
CLexTable::~CLexTable()
{
#ifdef __DIAGNOSTIC__
	m_DiagnosticFile<<"\n\n-------------------------- Contents -------------------------\n\n";
	m_DiagnosticFile<<"\n--- Word Table -------------\n";
	for (size_t i=0;i<m_lstWords.size();i++)
		m_DiagnosticFile<<Format("W%d: %s\n",i,m_lstWords[i]);
	m_DiagnosticFile<<"\n--- Operator Table ---------\n";
	for (size_t i=0;i<m_lstOperators.size();i++)
		m_DiagnosticFile<<Format("O%d: %s\n",i,m_lstOperators[i]);
	m_DiagnosticFile<<"\n--- ID Table ---------------\n";
	for (size_t i=0;i<m_lstIDs.size();i++)
		m_DiagnosticFile<<Format("I%d: %s\n",i,m_lstIDs[i]);

	m_DiagnosticFile<<"\n--- Const Table ------------\n";
	for (size_t i=0;i<m_lstConsts.size();i++)
	{
		const const_data & cdata= m_lstConsts[i];
		switch (cdata.m_value.get_type())
		{
		case variant::vtCHAR:
			m_DiagnosticFile<<Format("C%d: %c <character>\n",i,cdata.m_value.as_char());
			break;
		case variant::vtBOOL:
			m_DiagnosticFile<<Format("C%d: %d <bool>\n",i,cdata.m_value.as_bool());
			break;
		case variant::vtSMALL:
			m_DiagnosticFile<<Format("C%d: %d <small>\n",i,cdata.m_value.as_small());
			break;
		case variant::vtSHORT:
			m_DiagnosticFile<<Format("C%d: %d <short>\n",i,cdata.m_value.as_short());
			break;
		case variant::vtINT:
			m_DiagnosticFile<<Format("C%d: %d <int>\n",i,cdata.m_value.as_int());
			break;
		case variant::vtDOUBLE:
			m_DiagnosticFile<<Format("C%d: %f <double>\n",i,cdata.m_value.as_double());
			break;
		case variant::vtSTRING:
			m_DiagnosticFile<<Format("C%d: %s <string>\n",i,cdata.m_value.as_string().trans_ctrl().c_str());
			break;
		case variant::vtNULL:
		default:
			m_DiagnosticFile<<"C"<<i<<": \'"<<"?????"<<"\'-unknown\t";
		}
	}
	m_DiagnosticFile<<"\n";
#endif

	//Идентификаторы
	m_mapIDs.clear();
	for (lst_IDs::iterator i = m_lstIDs.begin(); i!=m_lstIDs.end(); ++i)
		free((void*)*i);
	//Константы
	for (size_t i=0; i<arrlength(m_mapConsts);i++)
		m_mapConsts[i].clear();
	for (lst_consts::iterator i = m_lstConsts.begin(); i!=m_lstConsts.end(); ++i)
		free((void*)(*i).m_szName);
	//Зарезервированные слова
	m_mapWords.clear();
	for (lst_words::iterator i = m_lstWords.begin(); i!=m_lstWords.end(); ++i)
		free((void*)*i);
	//Операторы
	m_mapOperators.clear();
	for (lst_operators::iterator i = m_lstOperators.begin(); i!=m_lstOperators.end(); ++i)
		free((void*)*i);
}
//------------------------------------------------------------------------
void CLexTable::WriteToDiagnosticFile(cstr szText,...)
{
}
//------------------------------------------------------------------------
ret_t CLexTable::AddWords(cstr src,...)
{
	va_list marker;
	va_start( marker,src);     /* Initialize variable arguments. */

	//////// block of creating new string and adding to table ////////
	cstr szBuffer;
	string_smart(src).release_buffer(szBuffer);
	
	m_lstWords.push_back(szBuffer);
	m_mapWords[szBuffer]=m_lstWords.size()-1;
	///////////////////////// end block //////////////////////////////
	cstr pstr;
	while((pstr = (cstr) va_arg( marker, cstr))!=NULL)
	{      
		string_smart(pstr).release_buffer(szBuffer);
		m_lstWords.push_back(szBuffer);
		m_mapWords[szBuffer]=m_lstWords.size()-1;
	}
	
	va_end( marker );              /* Reset variable arguments.      */			

	_ASSERT(!m_mapWords.empty());	
	_ASSERT(m_lstWords.size());
	return TRUE;
}
//------------------------------------------------------------------------
ret_t CLexTable::AddOperators(cstr src,...)
{
	va_list marker;
	va_start( marker,src);     /* Initialize variable arguments. */

	//////// block of creating new string and adding to table ////////
	cstr szBuffer;
	string_smart(src).release_buffer(szBuffer);

	m_lstOperators.push_back(szBuffer);
	m_mapOperators[szBuffer]=m_lstOperators.size()-1;
	///////////////////////// end block //////////////////////////////
	cstr pstr;
	while ((pstr = (cstr) va_arg( marker, cstr))!=NULL)
	{      
		string_smart(pstr).release_buffer(szBuffer);
		m_lstOperators.push_back(szBuffer);
		m_mapOperators[szBuffer]=m_lstOperators.size()-1;
	}
	va_end( marker );              /* Reset variable arguments.      */

	_ASSERT(!m_mapOperators.empty());	
	_ASSERT(m_lstOperators.size());
	return TRUE;
}
//------------------------------------------------------------------------
const structs::lexeme & CLexTable::AddID(cstr src)
{
	// пытаемся найти существ объект идентификторах
	if (LookupID(src,LastUsedObject))
		return LastUsedObject;
	
	// пытаемся найти существ объект в зарезервированных словах
	if (LookupWord(src,LastUsedObject))
		return LastUsedObject;

	// регистрируемся 
	cstr szBuffer;
	string_smart(src).release_buffer(szBuffer);
	m_lstIDs.push_back(szBuffer);
	m_mapIDs[szBuffer]=m_lstIDs.size()-1;
	
	_ASSERT(!m_mapIDs.empty());	
	_ASSERT(m_lstIDs.size());

	LastUsedObject.Set(ID_CLASS_ID,m_lstIDs.size()-1,m_lstIDs.back());	
	REPORT(src);
	return LastUsedObject;
}
//------------------------------------------------------------------------
const structs::lexeme & CLexTable::AddConst(cstr src, variant::type nType)
{
	//Пытаемся найти среди зарегестрированных константах
	if (LookupConst(src,nType,LastUsedObject))
		return LastUsedObject;

	const_data data;
	//Определяем значение и тип
	data.m_value=src;
	data.m_value.set_type(nType);

	//Регистрируемся
	string_smart(src).release_buffer(data.m_szName);
	m_lstConsts.push_back(data);
	m_mapConsts[nType][data.m_szName]=m_lstConsts.size()-1;

	_ASSERT(!m_mapConsts[nType].empty());	

	LastUsedObject.Set(CONST_CLASS_ID,m_lstConsts.size()-1,data.m_szName);

	REPORT(src);
	// NewConst не рушит свой m_pstring, т.к. мы присвоили m_dwType=NULL
	return LastUsedObject; 
}
//------------------------------------------------------------------------
bool CLexTable::LookupWord(cstr szName,structs::lexeme & Lexeme) const
{
	map_words::const_iterator i;
	if ((i=m_mapWords.find(szName))!=m_mapWords.end()){
		Lexeme.Set(WORD_CLASS_ID,(*i).second,(*i).first.c_str());	
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
bool CLexTable::LookupOperator(cstr szName,structs::lexeme & Lexeme) const
{
	map_operators::const_iterator i;
	if ((i=m_mapOperators.find(szName))!=m_mapOperators.end())
	{
		Lexeme.Set(OPERATOR_CLASS_ID,(*i).second,(*i).first.c_str());	
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
bool CLexTable::LookupID(cstr szName,structs::lexeme & Lexeme) const
{
	map_IDs::const_iterator i;
	if ((i=m_mapIDs.find(szName))!=m_mapIDs.end()){
		Lexeme.Set(ID_CLASS_ID,(*i).second,(*i).first.c_str());	
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
bool CLexTable::LookupConst(cstr szName,variant::type nType,structs::lexeme & Lexeme) const
{
	map_consts::const_iterator i;
	if ((i=m_mapConsts[nType].find(szName))!=m_mapConsts[nType].end())
	{
		const variant & value = m_lstConsts[(*i).second].m_value;

		Lexeme.Set(CONST_CLASS_ID,(*i).second,(*i).first.c_str());	
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
const structs::lexeme & CLexTable::GetReservedWord(cstr src) const
{
	if (LookupWord(src,LastUsedObject))
		return LastUsedObject;

	_ASSERT(false);
	throw exception("Reserved word not found");
}
//------------------------------------------------------------------------
const structs::lexeme & CLexTable::GetOperator(cstr src) const
{
	if (LookupOperator(src,LastUsedObject))
		return LastUsedObject;

	_ASSERT(false);
	throw exception("Operator not found");
}
//------------------------------------------------------------------------
const variant & CLexTable::GetConstant(size_t nIndex) const
{
	_ASSERT(nIndex<m_lstConsts.size());

	return m_lstConsts[nIndex].m_value;
}
//------------------------------------------------------------------------
cstr CLexTable::GetConstantName(size_t nIndex) const
{
	_ASSERT(nIndex<m_lstConsts.size());
	return m_lstConsts[nIndex].m_szName;
}
//------------------------------------------------------------------------
structs::lexeme & CLexTable::FormLexeme(cstr src) const
{
	static structs::lexeme Temporary;

	//if (!LookupConst(src,Temporary)) 
		if (!LookupID(src,Temporary)) 
			if (!LookupWord(src,Temporary)) 
				if (!LookupOperator(src,Temporary)) 
					_ASSERT(false);

	return Temporary;
}
//------------------------------------------------------------------------
structs::lexeme & CLexTable::FormLexeme(const ELexemeCLass &TableClass, cstr src) const
{
	static structs::lexeme Temporary;

	switch (TableClass)
	{
	case CONST_CLASS_ID:
	//	if (!LookupConst(src,Temporary))
		_ASSERT(false); //Сюда не может прийти константа
		break;
	case ID_CLASS_ID:
		if (!LookupID(src,Temporary)) 
			_ASSERT(false);
		break;
	case WORD_CLASS_ID:
		if (!LookupWord(src,Temporary)) 
			_ASSERT(false);
		break;
	case OPERATOR_CLASS_ID:
		if (!LookupOperator(src,Temporary)) 
			_ASSERT(false);
		break;
	default:	
		_ASSERT(false);
	}
	return Temporary;
}
//------------------------------------------------------------------------
size_t CLexTable::GetIndex(ELexemeCLass Class, cstr src) const
{
	switch (Class)
	{
	case CONST_CLASS_ID:
		//if (!LookupConst(src,variant::vtCHAR,LastUsedObject)) 
		_ASSERT(false); //Сюда не может прийти константа
		break;
	case ID_CLASS_ID:
		if (!LookupID(src,LastUsedObject)) 
		 _ASSERT(0);
		break;
	case WORD_CLASS_ID:
		if (!LookupWord(src,LastUsedObject)) 
			_ASSERT(0);
		break;
	case OPERATOR_CLASS_ID:
		if (!LookupOperator(src,LastUsedObject)) 
			_ASSERT(0);
		break;
	default:	
		_ASSERT(0);
	}
	return LastUsedObject.Index;
}


