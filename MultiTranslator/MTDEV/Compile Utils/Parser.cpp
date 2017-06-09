/********************************************************************
	created:	2002/03/23
	created:	23:3:2002   0:05
	filename: 	$(Projects)\Sources\MTDEv\Parser.cpp
	file path:	$(Projects)\Sources\MTDEv

	file base:	Parser
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Разборщик текста грамматик
*********************************************************************/

#include "stdafx.h"
#include <ParseUtils.h>
#include <grmcExports.h>
#include <algorithm>

#include "Parser.h"
#include "Names.h"

using namespace i_objects;
using namespace parse_utils;
using namespace grmc_exports;
//Создание переходника для объекта
static i_objects::object_thunk<CParser> Parser(__MSG_PREFIX__);

//////////////////////////////////////////////////////////////////////
// CParser class
//////////////////////////////////////////////////////////////////////
static inline int cmpn(cstr s1, cstr s2, int nHow){
	if (grmc_exports::bCaseInsensivity)
		return strnicmp(s1,s2,nHow);
	else
		return strncmp(s1,s2,nHow);
}

static bool cmpwords(const string_std & x, const string_std & y){
	_ASSERTE(x.c_str());
	_ASSERTE(y.c_str());

	if (grmc_exports::bCaseInsensivity)
		return strcmpi(x.c_str(),y.c_str())<0;
	else
		return strcmp(x.c_str(),y.c_str())<0;
}
CParser::CParser():CCObjectEx(), m_nUnicID(0), m_pCoClass(NULL),
	m_lstKeywords(pszKeywords,pszKeywords+arrlength(pszKeywords)),
	m_lstReservedFuncs(pszReservedFuncs,pszReservedFuncs+arrlength(pszReservedFuncs)),
	m_lstReservedVars(pszReservedVars,pszReservedVars+arrlength(pszReservedVars))

{
	std::sort(m_lstKeywords.begin(),m_lstKeywords.end(),cmpwords);
	std::sort(m_lstReservedFuncs.begin(),m_lstReservedFuncs.end(),cmpwords);
	std::sort(m_lstReservedVars.begin(),m_lstReservedVars.end(),cmpwords);
}
//------------------------------------------------------------------------
CParser::~CParser() 
{
	for (map_cache_it it = m_mapSyntaxCache.begin();it!=m_mapSyntaxCache.end();it++)
		delete (*it).second;
}
//------------------------------------------------------------------------
void CParser::Report(LPCTSTR szMsg, LPCTSTR szMsg2, DWORD dwPos)
{
	_RPT3(_CRT_WARN,"Wizard Error: %s <%s>, Line %d\n", szMsg, szMsg2,dwPos);
}
//------------------------------------------------------------------------
CParser::syntax_info_cache * CParser::GetCacheData(cstr szFileName)
{
	syntax_info_cache* pCachedInfo;
	map_cache_it mit = m_mapSyntaxCache.find(szFileName);
	if (mit!=m_mapSyntaxCache.end())
		pCachedInfo = (*mit).second;
	else {
		pCachedInfo=new syntax_info_cache;
		m_mapSyntaxCache[szFileName]=pCachedInfo;
	}
	return pCachedInfo;
}
//------------------------------------------------------------------------
void CParser::evOnCloseDocument(evdCloseDocumentInfo &Info)
{
	ASSERT(Info.in_strDocumentPath.c_str());
	
	//Мы обрабатываем только события закрытия проекта. Остальные пропускаем
	if (Info.in_nDocID==evdCloseProjectInfo::idProject){
		//Работаем с информацией, как с информацией о проекте
		IProjectDocLink & pProject = CAST_DATA(Info.in_DocInterface,IProjectDocLink);

		//Перебираем все файлы и выкидываем их из нашего кэша
		for (cstr szFileName = pProject .GetFileFirst();szFileName;szFileName=pProject.GetFileNext()){
			map_cache_it it = m_mapSyntaxCache.find(szFileName);
			if (it!=m_mapSyntaxCache.end()){
				delete (*it).second;
				m_mapSyntaxCache.erase(it);
			}
		}
	}
}
//------------------------------------------------------------------------
ref_cnt_ptr<COM::IParser> CParser::GetComInterface()
{
	if (!m_pCoClass)
	{
		m_pCoClass = new CComObject<COM::Parser>;
		m_pCoClass->LinkToOwner(this);
	}
	return m_pCoClass;
}
//------------------------------------------------------------------------
const string_smart & CParser::DoGetText(cstr szFileName)
{
	syntax_info_cache * pCacheData= GetCacheData(szFileName);
	IDocMngrLink * pDocMngrLink = i_objects::GetInterface<IDocMngrLink>();
	ASSERT_RETURN(pDocMngrLink,pCacheData->strText);

	//Пытаемся найти открытый документ. Не менять dynamic_cast на CAST_DATA!
	ISourceDocLink * pDocIface = dynamic_cast <ISourceDocLink *> (pDocMngrLink->FindDocument(szFileName));
	FILETIME tWriteFile; 	

	//Если есть открытый документ исходного файла
	if (pDocIface){
		time_t tLastChanged = pDocIface->GetTimeOfLastChange();
		if (pCacheData->tLastModified!=tLastChanged)
			 pDocIface->GetViewLink()->GetText(pCacheData->strText);
		//ОБновляем информацию о времени изменения
		pCacheData->tLastModified = tLastChanged;
		pCacheData->ftLastModified.dwLowDateTime = -1;
		pCacheData->ftLastModified.dwHighDateTime = -1;
	}
	//Открытого документа нет, и файла такого тоже нет
	else if (!is_file_exist(szFileName)){
		pCacheData->strText.erase();
		pCacheData->tLastModified = -1;
		pCacheData->ftLastModified.dwLowDateTime = -1;
		pCacheData->ftLastModified.dwHighDateTime = -1;
	}
	//Открытого документа нет, и были ошибки с определением даты, но прочитать файл можно
	else if (!get_file_time(szFileName,NULL,NULL,&tWriteFile)){
		load_file(szFileName,pCacheData->strText);
		pCacheData->tLastModified = -1;
		pCacheData->ftLastModified.dwLowDateTime = -1;
		pCacheData->ftLastModified.dwHighDateTime = -1;
	}
	//Файл найден, дата создания определена, все нормально
	else {
		if (CompareFileTime(&pCacheData->ftLastModified,&tWriteFile)!=0)
			load_file(szFileName,pCacheData->strText);
		pCacheData->tLastModified = -1;
		pCacheData->ftLastModified = tWriteFile;
	}
	return pCacheData->strText;
}
//------------------------------------------------------------------------
cstr  CParser::DoParseOneRule(cstr szText, long anLen,syntax_item & itemRoot, long dwPosOffest,cstr szFileName)
{
	cstr   pStartOfText=szText;
	cstr   pText=pStartOfText;
	cstr   pHelp;
	DWORD   Len;

	while (SearchWord(pText,"rule"))
	{
		pHelp=pText-4;
		if (!GetStringInBraces(pText,Len))
		{
			Report("Wrong rule declaration.",NULL,pText-pStartOfText);
			continue;
		}   //Error
		
		string_smart s(pText,Len);
		if (!SearchSymbol(pText,'{',true)) // change pText
		{
			Report("{ not found for rule.",s.c_str(), pText-pStartOfText);
			continue;
		}   
		if (!GetEndOfBlock(pText,Len)) // pText - const 
		{
			Report("} not found for rule",s.c_str(), pText-pStartOfText);
			continue;
		}              

		// Add rule     
		syntax_item & Rule = itemRoot.AddUnit();
		Rule.strName   = s; //Здесь указатель отнимается у s!
		Rule.nClass    = syntax_item::icRule;
		Rule.nPosStart = pHelp-pStartOfText+dwPosOffest;
		Rule.nPosEnd   = pText-pStartOfText+Len+dwPosOffest;
		Rule.strFilename=szFileName;
		Rule.strData.assign(pHelp,Rule.nPosEnd-Rule.nPosStart);
		
		long nRuleEnd= pText-pStartOfText+Len;
		// Add variants ==================================================
		while (SearchWord(pText, "variant",nRuleEnd- (DWORD)(pText-pStartOfText)))
		{
			pHelp=pText-7;
			string_smart s= Format("variant № %d",Rule.GetUnitsCount()+1);
			if (!SearchSymbol(pText,'{',true))
			{
				Report("{ not found for variant of rule ",s.c_str(), pText-pStartOfText);
				continue;
			}   
			if (!GetEndOfBlock(pText,Len))
			{
				Report("} not found for variant of rule ",s.c_str(), pText-pStartOfText);
				continue;
			}               
			syntax_item & Variant=Rule.AddUnit();
			Variant.strName   = s;
			Variant.nClass = syntax_item::icVariant;
			Variant.nPosStart = pHelp-pStartOfText+dwPosOffest;
			Variant.nPosEnd	  = pText-pStartOfText+Len+dwPosOffest;
			Variant.strFilename=szFileName;
			
			long nVariantEnd = pText-pStartOfText+Len;	//Без учета dwPosOffest
			// Add terms ==================================================
			while (Search1WordOutOf2(pText,"term","symbol",nVariantEnd  - (DWORD)(pText-pStartOfText)))
			{
				pHelp=pText-4;
				syntax_item::symbol_class TermClass=syntax_item::tcTerminal; //Предположение. В случае сбоя осанется именно этот тип
				syntax_item::symbol_kind  TermKind=syntax_item::tkNormal;
				SkipUselessSpace(pText);
TERM_TYPE_ANALYZE:
				switch (*pText)
				{
				case '[':
					pText++;
					SkipUselessSpace(pText);
					TermKind = syntax_item::tkOptional;
					goto TERM_TYPE_ANALYZE;
				case '<': // Nonterminal
					pText++;
					if (!GetString(pText,Len)) // pText - start of string
					{
						Report("wrong declaration for symbol ???",NULL,pText-pStartOfText);
						continue;
					}                                   
					ASSERT(*pText=='\"');
					TermClass = syntax_item::tcNonTerminal;
					break;
				case '\"':  //Terminal
					Len=0;
					GetEndOfString(pText,Len); //pText -const
					Len++;// был индекс, стал длина
					if (!*pText)
					{
						Report("wrong declaration for symbol ???",NULL,pText-pStartOfText);
						continue;
					}                                   
					TermClass = syntax_item::tcTerminal;
					break;
				default: // Standard terminal
					for (Len=0;__iscsym(pText[Len]);Len++);
					TermClass = syntax_item::tcStdTerminal;
				}   
				string_smart s(pText,Len); 
				
				pText+=Len;	SkipUselessSpace(pText);
				if (oneof(*pText,'>',']')) SkipUselessSpace(++pText);
				if (oneof(*pText,'>',']')) SkipUselessSpace(++pText);

				if ((pText) && (*pText=='*') && (TermKind==syntax_item::tkOptional))
					TermKind=syntax_item::tkChildOptional;

				if (!GetStartOfBlock(pText,'{','}'))
				{
					Len=0; //Это нормально
				}   
				else if (!GetEndOfBlock(pText,Len))
				{
					Report("} not found for symbol ",s.c_str(),pText-pStartOfText);
					continue;
				}                       
				
				syntax_item & Term = Variant.AddUnit();
				Term.nClass		= syntax_item::icTerm;
				Term.nSymbolKind	= TermKind;
				Term.nSymbolClass = TermClass;
				Term.strName	= s;
				Term.nPosStart	= pHelp-pStartOfText+dwPosOffest;
				Term.nPosEnd	= pText-pStartOfText+Len+dwPosOffest;
				Term.strFilename=szFileName;
				pText+=Len; 
			}
			ASSERT(pText==pStartOfText+Variant.nPosEnd-dwPosOffest);
		}
		ASSERT(pText==pStartOfText+Rule.nPosEnd-dwPosOffest);
		break; //Нашли одно правило, выходим
	}	//end of while 
	return pText;
}

//------------------------------------------------------------------------
cstr CParser::DoParseOneFunc(cstr szText, long anLen,syntax_item & itemRoot, long dwPosOffest, cstr szFileName)
{
	cstr pStartOfText=szText;
	cstr pText=pStartOfText;
	cstr pType,pTypeEnd, pName, pNameEnd;
	DWORD   Len;
	
	//Сканируем
	while (SearchAnyWordOutsideAnyBraces(pText,pType))
	{
		pTypeEnd=pText;
		if (GetWord(pText,pName))
		{
			pNameEnd=pText;
			SkipUselessSpace(pText);
			if (*pText=='(')
			{
				string_smart s(pType,pTypeEnd-pType);
				s.cat(" ");
				s.cat(pName,pNameEnd-pName);

				if (!GetEndOfBlock(pText,Len,'(',')')) // pText - const 
				{
					Report(") not found for Function",s.c_str(),pText-pStartOfText);
					continue;
				}               
				s.cat(pText,Len);

				syntax_item & Func = itemRoot.AddUnit();
				Func.strName = s.c_str();
				Func.nClass	 = syntax_item::icFunc;
				Func.nPosStart = pType-pStartOfText+dwPosOffest;
				Func.nPosEnd   = pText-pStartOfText+Len+dwPosOffest;
				Func.strFilename=szFileName;
				Func.strData.assign(pType,Func.nPosEnd-Func.nPosStart);
				return pType+Func.nPosEnd-Func.nPosStart; //Нашли одну функцию, выходим
			}
		}
	}

	if ((!pText) || (!*pText) || (pText!=szText))
		return pText;

	return pText+1;	//Если мы попали сюда, значит, не ничего не нашли
}
//------------------------------------------------------------------------
cstr CParser::DoParseOneStruct(cstr szText, long anLen, syntax_item & itemRoot,long dwPosOffest, cstr szFileName)
{
	cstr pStartOfText=szText;
	cstr pHelp,pName,pText=pStartOfText;
	DWORD	nLen;

	while (SearchWord(pText,"struct"))
	{
		pHelp=pText-5;
		
		if (!GetWord(pText,pName))
		{
			Report("wrong structure declaration",NULL, pText-pStartOfText);
			continue;
		} 
		
		// Add Structure        
		string_smart s(pName,pText-pName);
		
		if (!SearchSymbol(pText,'{',true)) // change pText
		{
			Report("{ not found for structure",s.c_str(), pText-pStartOfText);
			continue;
		}   
		if (!GetEndOfBlock(pText,nLen)) // pText - const 
		{
			Report("} not found for structure",s.c_str(), pText-pStartOfText);
			continue;
		}   

		syntax_item & Struct = itemRoot.AddUnit();
		Struct.strName   = s;
		Struct.nClass	 = syntax_item::icStruct;
		Struct.nPosStart = pHelp-pStartOfText+dwPosOffest;
		Struct.nPosEnd   = pText-pStartOfText+nLen+dwPosOffest;
		Struct.strFilename=szFileName;
		Struct.strData.assign(pHelp,Struct.nPosEnd-Struct.nPosStart);
		break; //Нашли одну структуру, выходим
	}
	return pText;
}
//------------------------------------------------------------------------
void CParser::DoParseText(const string_smart & strText, syntax_info & Result, cstr szFileName, bool bAllowUseCache)
{

	const int CacheTreshold = 100;  // 

	ASSERT_RETURN_VOID(!strText.isempty());
	ASSERT_RETURN_VOID(szFileName);

	size_t nTextLen = -1;
	cstr szText = strText.c_str();

	m_nUnicID++; //Обязательно увеличиваем ID операции. Это позволит отличить старые 
				 //данные в кешах от новых

	//Создаем кэш
	syntax_info_cache* pCachedInfo = GetCacheData(szFileName);
	syntax_item_friend *siCache[3] = 
		{(syntax_item_friend * )&pCachedInfo->m_Functions,
		 (syntax_item_friend * )&pCachedInfo->m_Structs,
		 (syntax_item_friend * )&pCachedInfo->m_Rules
		};
	//Элементы для записи
	syntax_item_friend *siItems[3] = 
		{(syntax_item_friend * )&Result.m_Functions,
		 (syntax_item_friend * )&Result.m_Structs,
		 (syntax_item_friend * )&Result.m_Rules
		};
	//Функции разбора
	typedef cstr (CParser::*pParseFunc)(cstr, long, syntax_item &, long, cstr);
	pParseFunc siFuncs[3] = {DoParseOneFunc,DoParseOneStruct,DoParseOneRule};

	//Использование ниже SafeInc не имеет никаких оснований, используется
	//только "а всякий случай", чтобы избежать бесконечного цикла

	for (int classes = 0; classes < arrlength(siCache);classes++)
	{
		//Основной разбор
		size_t it; cstr szStart; 
		for (szStart = szText, it=0, nTextLen = strText.length();(szStart) && (*szStart); it++){
			//Создаем или находим элемент в кэше
			if (it>=siCache[classes]->GetUnitsCount())
				siCache[classes]->AddUnit();
			syntax_item_friend & si = siCache[classes]->GetUnitAt(it);	//Из кэша

			//Пробуем кэш
			if ((si.nPosStart>=0) &&				//Значение действительное
				(si.nPosStart<(long)strText.length()) &&	//Размера текста позволяют
				(strncmp(szText+si.nPosStart,si.strData.c_str(),si.strData.length())==0)) //Тело не изменилось
			{
				si.m_nID = m_nUnicID;
				syntax_item & Struct = siItems[classes]->AddUnit();
				Struct = si;
				szStart = SafeInc(szStart,szText+si.nPosEnd); 
			}
			//Кеш не подошел
			else
			{
				size_t nOldSize = siItems[classes]->GetUnitsCount();
				pParseFunc pFunc = siFuncs[classes];
				szStart = SafeInc(szStart,(this->*pFunc)(szStart,nTextLen,*(siItems[classes]), szStart-szText,szFileName));
				//Добавляем в кеш
				if (siItems[classes]->GetUnitsCount()>nOldSize){
					_ASSERT(siItems[classes]->GetUnitsCount()-nOldSize==1);
					syntax_item & NewItem = siItems[classes]->GetUnitAt(siItems[classes]->GetUnitsCount()-1);
					si		=NewItem;
					si.m_nID=m_nUnicID;
				}
			}

			ASSERT(long(szStart)<=long(szText)+(long)strText.length());
			nTextLen = strText.length()-(szStart-szText);
		}

		//Выкидываем старые данные из кеша. Если кеш содержит CacheTreshold лищних элементов 
		if (int(siItems[classes]->GetUnitsCount() - it) > CacheTreshold)
			//Выкидываем лищние последние элементы
			for (long iit = siItems[classes]->GetUnitsCount()-1; iit>(long)it;iit--){
				syntax_item_friend & si = siItems[classes]->GetUnitAt(iit);
				if (m_nUnicID!=si.m_nID) //на всякий случай
					siItems[classes]->m_Units.erase(siItems[classes]->m_Units.begin()+iit);
			}
	}
}
//------------------------------------------------------------------------
void CParser::ParseSourceText(const string_smart & strText, cstr szFileName, syntax_info & Result, bool bAllowUseCache/*=false*/)
{
	if (!strText.isempty())
		DoParseText(strText,Result,szFileName,bAllowUseCache);
}
//------------------------------------------------------------------------
void CParser::ParseSourceFile	(cstr szFileName, syntax_info & Result, bool bAllowUseCache/*=false*/)
{
	const string_smart & strBuffer=DoGetText(szFileName);
	if (!strBuffer.isempty())
		DoParseText(strBuffer.c_str(),Result,szFileName,bAllowUseCache);
}
//------------------------------------------------------------------------
void CParser::ParseWholeProject	(IProjectDocLink & IProject, syntax_info & Result, bool bAllowUseCache/*=false*/)
{
	for (cstr szFile = IProject.GetFileFirst();szFile; szFile = IProject.GetFileNext())
		if (IProject.GetFileType(szFile)==IProjectDocLink::fileSource)
			ParseSourceFile(szFile,Result);
}
//------------------------------------------------------------------------
bool CParser::FindRule (const string_smart & strText, cstr szRuleName, size_t & nStart, size_t & nEnd)
{
	syntax_info info;
	ParseSourceText(strText,"xxx",info);
	for (si_iterator i=info.m_Rules.GetFirstUnitPos();
		 !info.m_Rules.IsEndUnitPos(i);) 
	{
		syntax_item SItemRule = info.m_Rules.GetNextUnit(i);
		if (SItemRule.strName==szRuleName){
			nStart = SItemRule.nPosStart;
			nEnd = SItemRule.nPosEnd;
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------
IParserLink::ps_res CParser::FindDeclaration (cstr szItemName, ITEMLOCATION aMetAt, ITEMLOCATION & aFoundAt)
{
	//Это - ключевое слово?
	if (oneof(GetWordClass(szItemName),wcKeyWord,wcReservedFunc,wcReservedVar))
		return psrImpossible;

	typedef list<const string_smart*> list_buffer_data;
	typedef list_buffer_data::iterator  list_buffer_it;
	list_buffer_data lstBuffers;

	string_list lstFiles;
	string_smart strItemName(szItemName);
	
	//Для начала - указанный файл, он самый приоритетный
	if (!aMetAt.strFileName.isempty())
		lstFiles.items().add(aMetAt.strFileName.c_str());

	IDocMngrLink * pDocMngr = i_objects::GetInterface<IDocMngrLink>();
	_ASSERTE(pDocMngr);
	
	if (pDocMngr){
		//Затем - файлы текущего проекта
		IProjectDocLink * pActiveProj = pDocMngr->GetActiveProject();
		if (pActiveProj){
			string_list lst;
			pActiveProj->GetFiles(lst,IProjectDocLink::fileSource);
			lstFiles.items().append(lst.items());
		}
		//После - файлы всех оставшихся проектов
		#pragma message(__MSG_PREFIX__"Multiproject FindDeclaration not implemented.") 
	}
	
	lstFiles.items().remove_coincidences();

	//Загружаем тексты модулей с одновременным поиском правила. На первом месте идет 
	//указанный на входе модуль
	for (size_t i=0; i<lstFiles.items().count();i++){
		const string_smart & strBuffer = DoGetText(lstFiles.items()[i]);
		lstBuffers.push_back(&strBuffer);

		if (FindRule(strBuffer,szItemName,aFoundAt.nStartPos,aFoundAt.nEndPos)){
			aFoundAt.strFileName=lstFiles.items()[i];

			//Небольшая заплатка - нам необходимо выделять только само слово непосредственно
			//в то время как FindRule возращает пределы всего правила, поэтому делаем следующую операцию
			string_smart strRuleBody(strBuffer.c_str()+aFoundAt.nStartPos,aFoundAt.nEndPos-aFoundAt.nStartPos);
			long n = (grmc_exports::bCaseInsensivity) 
								? strRuleBody.posi(szItemName)
								: strRuleBody.pos(szItemName);
			if (n!=-1) {
				aFoundAt.nStartPos+=n;
				aFoundAt.nEndPos=aFoundAt.nStartPos+strItemName.length();
			}
			return psrOk;
		}
	}
		
	_ASSERTE(lstFiles.items().count()==lstBuffers.size());

	//Первое вхождение слова
	list_buffer_it j=lstBuffers.begin();
	for (size_t i=0; i<lstFiles.items().count();i++,j++){
		const string_smart & strBuffer = **j;

		aFoundAt.nStartPos = (grmc_exports::bCaseInsensivity) 
								? strBuffer.pos(szItemName)
								: strBuffer.posi(szItemName);
		if (aFoundAt.nStartPos!=-1){
			aFoundAt.nEndPos=aFoundAt.nStartPos+strItemName.length();
			aFoundAt.strFileName=lstFiles.items()[i];
			return psrOk;
		}
	}
	
	return psrFail;
}
//------------------------------------------------------------------------
IParserLink::ps_res CParser::DetectSyntaxItem (cstr szFileName, size_t nPos, OUT syntax_item & aFoundItem)
{
	syntax_info SInfo;
	ParseSourceFile(szFileName,SInfo);

	for (IParserLink::sir_iterator j=SInfo.GetBeginPos();j!=SInfo.GetEndPos();j++)
		for (IParserLink::si_iterator i=SInfo.GetRoot(j).GetFirstUnitPos();!SInfo.GetRoot(j).IsEndUnitPos(i);) {
			IParserLink::syntax_item & SItem = SInfo.GetRoot(j).GetNextUnit(i);
			if (inrange((long)nPos,SItem.nPosStart,SItem.nPosEnd)){
				aFoundItem = SItem;
				return psrOk;
			}
		}

	return psrFail;
}
//------------------------------------------------------------------------
IParserLink::word_class CParser::GetWordClass(cstr szItemName)
{
	//Это - ключевое слово?
	if (find(m_lstKeywords.begin(),m_lstKeywords.end(),szItemName)!=m_lstKeywords.end())
		return wcKeyWord;
	if (find(m_lstReservedFuncs.begin(),m_lstReservedFuncs.end(),szItemName)!=m_lstReservedFuncs.end())
		return wcReservedFunc;
	if (find(m_lstReservedVars.begin(),m_lstReservedVars.end(),szItemName)!=m_lstReservedVars.end())
		return wcReservedVar;
	
	for (cstr sz=szItemName;sz && *sz; sz++){
		if ((sz==szItemName) && (!__iscsymf(*sz))) //Первый символ должен быть A-Z или _
			return wcUnknown;
		if (!__iscsym(*sz))
			return wcUnknown;
	}
	return wcUserIndentifier;
}
//------------------------------------------------------------------------
bool CParser::SuggestAutoComlete(cstr szItemName, const string_list & lstMostUsed, ITEMLOCATION aMetAt, string_list & aSuggestions)
{
	long nItemLen = _tcslen(szItemName);

	vector <string_std> res;

	//KeyWords
	for (str_vector_it i=m_lstKeywords.begin();i!=m_lstKeywords.end();i++)
		if (cmpn(szItemName,(*i).c_str(),nItemLen)==0)
			res.push_back((*i).c_str());
	//ReservedFuncs			
	for (str_vector_it i=m_lstReservedFuncs.begin();i!=m_lstReservedFuncs.end();i++)
		if (cmpn(szItemName,(*i).c_str(),nItemLen)==0)
			res.push_back((*i).c_str());
	//ReservedVars
	for (str_vector_it i=m_lstReservedVars.begin();i!=m_lstReservedVars.end();i++)
		if (cmpn(szItemName,(*i).c_str(),nItemLen)==0)
			res.push_back((*i).c_str());
	//User
	for (size_t i=0; i<lstMostUsed.items().count();i++)
		if (cmpn(szItemName,lstMostUsed.items()[i],nItemLen)==0)
			res.push_back(lstMostUsed.items()[i]);
			
	std::sort(res.begin(),res.end(),cmpwords);
	aSuggestions.items().clear();
	aSuggestions.items().append(res.begin(),res.end());
	return aSuggestions.items().count()>0;
}

//------------------------------------------------------------------------
void CParser::AssembleToText(syntax_info & SyntaxData, string_smart & strOutText)
{
	//Правила
	for	(si_iterator i=SyntaxData.m_Rules.GetFirstUnitPos();
		!SyntaxData.m_Rules.IsEndUnitPos(i);)
	{
		syntax_item & SItemRule = SyntaxData.m_Rules.GetNextUnit(i);
		//Заголовок правила
		strOutText.cat(string_smart ("\r\n//",strnchar('-',78).c_str(),"\r\n").c_str());
		
		SItemRule.nPosStart= strOutText.length(); //Начало правила
		strOutText.cat(Format("rule <\"%s\">\r\n{",SItemRule.strName.c_str()).c_str());
		
		//Первый уровень вложенности правила - вариант
		for	(si_iterator i=SItemRule.GetFirstUnitPos();
			!SItemRule.IsEndUnitPos(i);)
		{
			syntax_item & SItemVariant = SItemRule.GetNextUnit(i);
			//Заголовок варианта
			strOutText.cat("\r\n\t");
			SItemVariant.nPosStart=strOutText.length();//Позиция варианта
			strOutText.cat("variant\r\n\t{");

			//Второй уровень вложенности  - символы
			for	(si_iterator i=SItemVariant.GetFirstUnitPos();
				!SItemVariant.IsEndUnitPos(i);)
			{
				syntax_item & SItemTerm = SItemRule.GetNextUnit(i);
				strOutText.cat("\r\n\t\t");
				SItemTerm.nPosStart=strOutText.length(); //Позиция терма
				strOutText.cat("symbol ");
				
				if (oneof(SItemTerm.nSymbolKind, syntax_item::tkOptional, syntax_item::tkChildOptional))
					strOutText.cat("[");

				//Нетерминал берем в кавычки
				if (SItemTerm.nSymbolClass==syntax_item::tcNonTerminal) 
					strOutText.cat("<");
				
				if (SItemTerm.nSymbolClass!=syntax_item::tcStdTerminal)
					strOutText.cat("\"");

				strOutText.cat(SItemTerm.strName.c_str());

				if (SItemTerm.nSymbolClass!=syntax_item::tcStdTerminal)
					strOutText.cat("\"");

				if (SItemTerm.nSymbolClass==syntax_item::tcNonTerminal) 
					strOutText.cat(">");

				if (SItemTerm.nSymbolKind==syntax_item::tkOptional)
					strOutText.cat("]");
				else if (SItemTerm.nSymbolKind==syntax_item::tkChildOptional)
					strOutText.cat("]*");

				//Окончание символа
				strOutText.cat("\r\n\t\t{");
				if (!SItemTerm.strActions.isempty()){ //Список действий
					string_list strLines = SItemTerm.strActions.c_str();
					strOutText.cat("\r\n");
					for (size_t i=0; i<strLines.items().count();++i)
						strOutText.append("\t\t\t",strLines.items()[i],"\r\n");
					strOutText.cat("\r\n\t\t");
				}
				strOutText.cat("}");
				SItemVariant.nPosEnd=strOutText.length()-1-2; //-2 == \r\n
			}
			//Окончание варианта
			if (!SItemVariant.strActions.isempty()){ //Список действий
				string_list strLines = SItemVariant.strActions.c_str();
				strOutText.cat("\r\n");
				for (size_t i=0; i<strLines.items().count();++i)
					strOutText.append("\t\t",strLines.items()[i],"\r\n");
			}
			
			strOutText.cat("\r\n\t}");
			SItemVariant.nPosEnd=strOutText.length()-1;
		}
		//Окончание правила
		if (!SItemRule.strActions.isempty()){ //Список действий
			string_list strLines = SItemRule.strActions.c_str();
			strOutText.cat("\r\n");
			for (size_t i=0; i<strLines.items().count();++i)
				strOutText.append("\t",strLines.items()[i],"\r\n");
		}
		strOutText.cat("\r\n}"); //\r\n
		SItemRule.nPosEnd=strOutText.length()-1-2; //-2 == \r\n
	}
	//Функции 
	//не готово
	//Структуры
	//не готово
}
//------------------------------------------------------------------------
void CParser::AssembleToXBNF(syntax_info & SyntaxData, string_smart & strOutText)
{
	//Правила
	for	(si_iterator i=SyntaxData.m_Rules.GetFirstUnitPos(); !SyntaxData.m_Rules.IsEndUnitPos(i);)
	{
		syntax_item & SItemRule = SyntaxData.m_Rules.GetNextUnit(i);
	
		//Заголовок правила
		strOutText.cat(string_smart ("\r\n//",strnchar('-',78).c_str(),"\r\n").c_str());
		
		//Название всех нетерминалов не должно включать кавычки
		string_smart strUntermName = SItemRule.strName.c_str();
		strUntermName.frames_delete('\"','\"');

		string_smart s(Format("<%s> = ",strUntermName.c_str()).c_str());
		int nRuleMargin = s.length()-2; //-2 is "= "

		//Первый уровень вложенности правила - вариант
		for	(si_iterator i=SItemRule.GetFirstUnitPos();	!SItemRule.IsEndUnitPos(i);)
		{
			syntax_item & SItemVariant = SItemRule.GetNextUnit(i);
			//Второй уровень вложенности  - термы
			bool bSeparate = false;
			bool bSquareBracket=false;
			for	(si_iterator i=SItemVariant.GetFirstUnitPos(); !SItemVariant.IsEndUnitPos(i); )
			{
				if (s.isempty()){
					s.cat(strnchar(' ',nRuleMargin).c_str());
					s.cat("= ");
				}
					
				syntax_item & SItemTerm = SItemRule.GetNextUnit(i);
				SItemTerm.nPosStart=strOutText.length(); //Позиция терма

				//Если не продолжение опционального набора
				if (SItemTerm.nSymbolKind!=syntax_item::tkChildOptional){
					if (bSquareBracket)
						s.cat(']');
					bSquareBracket=false;
					//Новый опционал
					if (SItemTerm.nSymbolKind==syntax_item::tkOptional){
						if (bSeparate)		//Отсоединяем от предыдущего
							s.cat(' ');		//
						s.cat('[');
						bSquareBracket=true;	//Отмечаем открывающуюся скобку
						bSeparate= false;		//ОТмечаем, что пробел не нужен
					}
				}
	
			
				if (bSeparate)	//Отсоединяем от предыдущего
					s.cat(' ');		//

				//Нетерминал берем в кавычки
				if (SItemTerm.nSymbolClass==syntax_item::tcNonTerminal) {
					strUntermName = SItemTerm.strName.c_str();
					strUntermName.frames_delete('\"','\"');
					s.cat("<");
					s.cat(strUntermName.c_str());					
					s.cat(">");
				}
				else {
					s.cat(SItemTerm.strName.c_str());
				}
				
				bSeparate = true;
			}
			if (bSquareBracket)
				s.cat(']');

			strOutText.cat(s.c_str());
			strOutText.cat("\r\n");
			s.clear();
		}
		//Окончание правила
		strOutText.cat("\r\n");
	}
}
//------------------------------------------------------------------------
void CParser::BuildRuleDependence(syntax_item_root  & m_Rules)
{
	typedef map<string_smart, IParserLink::syntax_item *> map_rule;
	typedef map_rule::iterator mr_iterator;
	map_rule mapRuleNames;
	
	for (IParserLink::si_iterator i=m_Rules.GetFirstUnitPos();!m_Rules.IsEndUnitPos(i);){
		IParserLink::syntax_item& SRule = m_Rules.GetNextUnit(i);
		SRule.pParents.clear();
		SRule.pChildren.clear();
		mapRuleNames[SRule.strName.c_str()]=&SRule;		
	}
	
	//Rules
	for (IParserLink::si_iterator i=m_Rules.GetFirstUnitPos();!m_Rules.IsEndUnitPos(i);){
		IParserLink::syntax_item & SRule = m_Rules.GetNextUnit(i);
		//variants
		for (IParserLink::si_iterator j=SRule.GetFirstUnitPos();!SRule.IsEndUnitPos(j);){
			IParserLink::syntax_item & SVar = SRule.GetNextUnit(j);
			//symbols
			for (IParserLink::si_iterator k=SVar.GetFirstUnitPos();!SVar.IsEndUnitPos(k);)
			{
				IParserLink::syntax_item & STerm = SVar.GetNextUnit(k);
				
				if (STerm.nSymbolClass==syntax_item::tcNonTerminal)
				{
					mr_iterator m = mapRuleNames.find(STerm.strName.c_str());
					if (m!=mapRuleNames.end()){
						IParserLink::syntax_item & parent = SRule;
						IParserLink::syntax_item & child  = *(*m).second;
						_ASSERTE(_CrtIsValidPointer(&child,4,TRUE));
						parent.pChildren.push_back(&child);
						child.pParents.push_back(&parent);
					}
				}
			}
		}
	} // of for
}
//------------------------------------------------------------------------
string_smart CParser::ComposeMainFunction(cstr szRootRule, bool bIgnoreCase, list<string_pair> lstIgnore, list<string_pair> lstReplace)
{
	HRSRC hSFXRes=FindResource(NULL,(char_t*)IDR_F_MAIN_TMPL1,"F_MAIN_TMPL");
	_ASSERT(hSFXRes);
	HGLOBAL MemArea= LoadResource(NULL,hSFXRes);
	_ASSERT(MemArea);

	string_smart result; 
	result.cat((cstr)LockResource(MemArea),SizeofResource(NULL,hSFXRes));
	result.replace("$IGNORE_CASE$", bIgnoreCase ? "on" : "off");
	result.replace("$ROOT_RULE$", szRootRule);
	
	string_smart strIgnore;
	for (list<string_pair>::iterator i = lstIgnore.begin();i!=lstIgnore.end();++i)
		strIgnore.append("IgnoreBlock(\"",(*i).left().c_str(),"\",\"",(*i).right().c_str(),"\"); ");
	result.replace("$IGNORE_BLOCK$", strIgnore.c_str());

	string_smart strReplace;
	for (list<string_pair>::iterator i = lstReplace.begin();i!=lstReplace.end();++i)
		strReplace.append("ReplaceBlock(\"",(*i).left().c_str(),"\",\"",(*i).right().c_str(),"\"); ");
	result.replace("$REPLACE_BLOCK$", strReplace.c_str());

	return result;
}
//------------------------------------------------------------------------
void CParser::FindTranslationDirections(IN IProjectDocLink & IProject, OUT string_list & InputDirs,  OUT string_list & OutputDirs)
{
	typedef map<string_std,bool> str_map;
	str_map mapInput,mapOutput;

	for (cstr szFile = IProject.GetFileFirst();szFile; szFile = IProject.GetFileNext())
		if (IProject.GetFileType(szFile)==IProjectDocLink::fileSource)
		{
			const string_smart & strBuffer=DoGetText(szFile);
			cstr pText = strBuffer.c_str();
			
			while (SearchWord(pText,"IL"))
			{
				cstr pWord;
				if (GetWord(pText,pWord))
					mapInput[string_smart(pWord,pText-pWord)]=true;
			}

			pText = strBuffer.c_str();
			while (SearchWord(pText,"OL"))
			{
				cstr pWord;
				if (GetWord(pText,pWord))
					mapOutput[string_smart(pWord,pText-pWord)]=true;
			}
		}

	for (str_map::iterator i=mapInput.begin();i!=mapInput.end();++i)
		InputDirs.items().add((*i).first.c_str());
	for (str_map::iterator i=mapOutput.begin();i!=mapOutput.end();++i)
		OutputDirs.items().add((*i).first.c_str());
}