/********************************************************************
	created:	2002/03/22
	created:	22:3:2002   23:03
	filename: 	$(Projects)\Sources\MTDEv\Parser.h
	file path:	$(Projects)\Sources\MTDEv

	file base:	Parser
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Разборщик текста грамматик
*********************************************************************/

#if !defined(AFX_PARSER_H__3E4D234F_E659_45CB_B8F2_9F80CD1EAEBB__INCLUDED_)
#define AFX_PARSER_H__3E4D234F_E659_45CB_B8F2_9F80CD1EAEBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CObjectEx.h>
#include <BaseStructs.h>

#include "COMParser.h"	//COM  

class CParser:	public CCObjectEx, public IParserLink,
				public IDocumentLink::IEventSink
{
public:
	CParser();
	virtual ~CParser();
protected:
	//COM Support
	COM::Parser *	m_pCoClass;	//Текущий CoClass
	friend class COM::Parser;

	//Обманка для компилятора, чтобы добраться до защищенных данных  
	//Нельзя добавлять реальных полей
	class syntax_item_friend: public syntax_item {	
	public:
		syntax_item::m_Units;	
		syntax_item::m_nID;
		syntax_item_friend & GetUnitAt(int index){
			return (syntax_item_friend&) __super::GetUnitAt(index);
		}
		syntax_item_friend & operator = (const syntax_item & src){
			(syntax_item&)(*this)=(src); return *this;
		}
	};

	typedef vector <string_std> str_vector;
	typedef str_vector::iterator str_vector_it;
	vector <string_std> m_lstKeywords; 		//Таблица ключевых слов
	vector <string_std> m_lstReservedFuncs; //Таблица зарезервированных функций
	vector <string_std> m_lstReservedVars;	//Таблица зарезервированных переменных
	size_t				m_nUnicID;
	
	//Расширенный syntax_info для хранения закешированных данных
	struct syntax_info_cache: public syntax_info{
		string_smart strText;			//Буфер для хранения текста
		FILETIME	 ftLastModified;	//Для загрузки из файла
		time_t		 tLastModified;		//Для открытых документов
		size_t		 nID;			
		syntax_info_cache(): tLastModified(-1){
			ftLastModified.dwHighDateTime = ftLastModified.dwLowDateTime = -1;
		}
	};
	typedef map<string_std,syntax_info_cache *> map_cache;//Кэш разобранных значений
	typedef map_cache::iterator map_cache_it;		//ключ- название файла
	map_cache m_mapSyntaxCache;

	//Проверяет, если szTextOld=szTextNew, то инкрементирует szTextNew
	//Назначение функции - защита от бесконечного цикла
	cstr SafeInc(cstr szTextOld,cstr szTextNew){
		//Старый и новый указатели равны. Или новый указатель меньше  старого.Ошибка!!!
		ASSERT(szTextNew>szTextOld); 
		if ((szTextNew==NULL) || (szTextNew!=szTextOld)) 
			return szTextNew;
		return ++szTextNew;	//Для RELEASE-версии
	}

	//Возвращает данные из кеша по указанному файлу. Если такой записи не было
	//она добавляется
	syntax_info_cache * GetCacheData(cstr szFileName);

	void Report(LPCTSTR szMsg, LPCTSTR szMsg2, DWORD dwPos);
	//Возвращает загруженный текст с указанного файла. Файл может быть открыт в редакторе, а 
	//может быть всзят с диска. Возвращает ссылку на строку из кэша. Модифицировать 
	//эту строку нельзя.
	const string_smart & DoGetText(cstr szFileName);
	//Ищет в указанном тексте объявление одного правила
	//Возвращает указатель на следующую позицию в тексте после разобранного правила
	//dwPosOffset указывает смещение текста относительно его начала. 
	//Нужно для формирования корректного nPosStart  и nPosEnd 	
	cstr DoParseOneRule(cstr szText, long nLen, syntax_item & itemRoot, long dwPosOffest, cstr szFileName);
	//Ищет в указанном тексте объявление одной функции
	cstr DoParseOneFunc(cstr szText, long nLen, syntax_item & itemRoot, long dwPosOffest, cstr szFileName);
	//Ищет в указанном тексте объявление одной структуры
	cstr DoParseOneStruct(cstr szText, long nLen, syntax_item & itemRoot, long dwPosOffest, cstr szFileName);
	//Разбирает указанный текст. szFileName записывается в каждую добавленный элемент,
	//не более того
	void DoParseText(const string_smart & strText, syntax_info & Result, cstr szFileName,bool bAllowUseCache);

	override void IDocumentLink::IEventSink::evOnOpenDocument  (evdOpenDocumentInfo &)	  {}
	override void IDocumentLink::IEventSink::evOnChangeDocument(evdChangeDocumentInfo &) {}
	override void IDocumentLink::IEventSink::evOnCloseDocument (evdCloseDocumentInfo &);

	//Наследники от IParser
	override void ParseSourceText	 (const string_smart & strText, cstr szFileName, syntax_info & Result, bool bAllowUseCache=true);
	override void ParseSourceFile	 (cstr szFileName, syntax_info & Result, bool bAllowUseCache=true);
	override void ParseWholeProject	 (IProjectDocLink & IProject, syntax_info & Result, bool bAllowUseCache=true);

	override bool FindRule			 (const string_smart & strText, cstr szRuleName, size_t & nStart, size_t & nEnd);
	override ps_res FindDeclaration	 (cstr szItemName, ITEMLOCATION aMetAt, ITEMLOCATION & aFoundAt);
	override ps_res DetectSyntaxItem (cstr szFileName, size_t nPos, OUT syntax_item & aFoundItem);
	override bool SuggestAutoComlete (cstr szItemName, const string_list & lstMostUsed, ITEMLOCATION aMetAt, string_list & aSuggestions);

	override void AssembleToText	 (syntax_info & SyntaxData, string_smart & strOutText);
	override void AssembleToXBNF	 (syntax_info & SyntaxData, string_smart & strOutText);

	override void BuildRuleDependence(syntax_item_root & m_Rules);
	override word_class GetWordClass (cstr szItemName);

	override string_smart ComposeMainFunction(cstr szRootRule, bool bIgnoreCase, list<string_pair> lstIgnore, list<string_pair> lstReplace);
	override void FindTranslationDirections(IN IProjectDocLink & IProject, OUT string_list & InputDirs,  OUT string_list & OutputDirs);

	//COM
	override ref_cnt_ptr<COM::IParser> GetComInterface();
};

#endif // !defined(AFX_PARSER_H__3E4D234F_E659_45CB_B8F2_9F80CD1EAEBB__INCLUDED_)
