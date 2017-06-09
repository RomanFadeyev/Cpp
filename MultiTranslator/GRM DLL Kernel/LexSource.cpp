// LexSource.cpp: implementation of	the	LexSource class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LexSource.h"

//------------------------------------------------------------------------
CLexSource::CLexSource():	m_bGetStdTerminalOK(false),
							m_bIgnoreWhiteSpaces(true), //игнорировать лишние пробелы
							m_bSolitaryWords(true), //Слова друг от друга разделяются чем-то (не так как в фортране)
							m_bIgnoreCase(false) //Регистр букв имеет значение
{
	m_CurrentState.m_SourcePos = m_PrevState.m_SourcePos = m_PrevTerminalPos = m_lstSource.end();
	m_strDelimiters = "{}<>?!@#$%*()_+|-[],.;=:/~^&|\\/";
}
//------------------------------------------------------------------------
CLexSource::~CLexSource()
{
}
//------------------------------------------------------------------------
void CLexSource::SetParseCallBck(const grmkrnl_exports::parse_info_functor & Func)
{
	m_ParseCallBckFunc =Func;
}	
//------------------------------------------------------------------------
long CLexSource::GetCurLine() const		
{
	src_pos pos = GetCurrentPos();
	if (pos==m_lstSource.end())	//Не очень правильно
		--pos;
	return (*pos).m_nLine;
}
//------------------------------------------------------------------------
long CLexSource::GetCurCol() const		
{
	src_pos pos = GetCurrentPos();
	if (pos==m_lstSource.end())//Не очень правильно
		--pos;
	return (*pos).m_nPos;
}
//------------------------------------------------------------------------
void CLexSource::GetSymbol(litera & symbol)
{
	char_t			ch=(*m_CurrentState.m_SourcePos).m_cValue;
	litera::lex_tag	nTag=litera::TAG_NONE;

	if ((UCHAR)ch>127)	
		nTag=litera::TAG_ALT;
	//Буква
	else if (isalpha(ch)) 
		nTag=litera::TAG_LETTER;
	//Цифра
	else if (isdigit(ch)) 
		nTag=litera::TAG_DIGIT;
	//ПРобел и проч
	else if (isspace(ch)) 
		nTag=litera::TAG_SPACE;
	//Знак пунктуации
	else if (ispunct(ch)) 
		nTag=litera::TAG_PUNCT;
	else if (ch==0) 
		nTag=litera::TAG_END;
	else
		_ASSERT(false);

	symbol.Set(ch,nTag);
}
//------------------------------------------------------------------------
void CLexSource::StepBack()
{
	if (m_CurrentState.m_nOverEnd>0)
		--m_CurrentState.m_nOverEnd;
	else
		--m_CurrentState.m_SourcePos;	//Передвигаем маркер назад
}
//------------------------------------------------------------------------
CLexSource::litera	CLexSource::GetNextChar()
{
	litera result;

	if (m_CurrentState.m_SourcePos!=m_lstSource.end())
	{
		GetSymbol(result);
		++m_CurrentState.m_SourcePos;	//Передвигаем маркер
	}
	else
	{
		result.Set(0,litera::TAG_END);
		++m_CurrentState.m_nOverEnd;
	}
	return result;
}
//------------------------------------------------------------------------
CLexSource::litera	CLexSource::LookNextChar()
{
	litera result;
	GetSymbol(result);
	return result;
}
//------------------------------------------------------------------------
string_smart CLexSource::GetTextAhead(size_t nHowMany) const
{
	string_smart res;
	src_pos Pos = m_CurrentState.m_SourcePos;

	for (size_t i=0; (i<nHowMany) && (Pos!=m_lstSource.end()); ++i,++Pos)
		res.cat((*Pos).m_cValue);
	return res;
}
//------------------------------------------------------------------------
string_smart CLexSource::GetTextBehind(size_t nHowMany) const
{
	string_smart res;
	src_pos Pos = m_CurrentState.m_SourcePos;

	if (Pos==m_lstSource.begin())
		return res;
	--Pos;

	for (size_t i=0; i<nHowMany ; ++i,--Pos){
		res.insert((*Pos).m_cValue,0);
		if (Pos==m_lstSource.begin())
			break;
	}
		
	return res;
}
//------------------------------------------------------------------------
bool CLexSource::GetStandardTerminal(EStdTerminalClass	TerminalType)
{
	_ASSERT(TerminalType!=NO_STD_TERMINAL);

	if ((m_PrevTerminalPos==m_CurrentState.m_SourcePos)&& (TerminalType==m_nPrevTerminalType) &&(m_bGetStdTerminalOK==false))	// Внимание!!! Оптимизация
		return false;
	m_PrevTerminalPos=m_CurrentState.m_SourcePos;
	m_nPrevTerminalType=TerminalType;

	StoreState();

	litera symbol;
	do 
	{
		symbol=GetNextChar();
	} while	((symbol.Tag & litera::TAG_SPACE) && (TerminalType!=STD_LINE));

	if (symbol.Tag & litera::TAG_END) goto BAD;
	
	m_strTerminalText.clear();
	switch (TerminalType)
	{
	case STD_LINE:
		while ((symbol.Name!='\n') && (!(symbol.Tag & litera::TAG_END)))
		{
			m_strTerminalText.cat(symbol.Name);
			symbol=GetNextChar();
		}
		goto GOOD;
		break;
	case STD_COMMON_CONST:
		if ((symbol.Tag & litera::TAG_LETTER) ||	(symbol.Name=='_')) goto	M_ID;
		else if	((symbol.Tag & litera::TAG_DIGIT) ||	(symbol.Name=='-'))
			goto M_DIGIT_CONST;
		else if	(symbol.Name=='\"')
			goto M_DOUBLE_QUOTES_CONST;
		else if	(symbol.Name=='\'')
			goto M_SINGLE_QUOTES_CONST;
		else
			goto BAD;
	case STD_ID:
M_ID:
		//symbol=GetNextChar(); m_CurrentState.m_bStepBack	not	enable !!!
		if (!(symbol.Tag & litera::TAG_LETTER) && (symbol.Name!='_'))	goto BAD;
		
		while ((symbol.Tag & litera::TAG_LETTER)	|| (symbol.Tag & litera::TAG_DIGIT) || (symbol.Name=='_'))
		{
			m_strTerminalText.cat(symbol.Name);
			symbol=GetNextChar();
		}

		break;
	case STD_INT_CONST:
//M_INT_CONST:
		//symbol=GetNextChar(); m_CurrentState.m_bStepBack	not	enable !!!
		if (!(symbol.Tag & litera::TAG_DIGIT) && (symbol.Name!='-'))	goto BAD;
		
		if (symbol.Name=='-'){
			m_strTerminalText.cat(symbol.Name);
			symbol=GetNextChar();
			while ((symbol.Name == '	') || (symbol.Name == '\t'))	
				symbol=GetNextChar();
			if (!(symbol.Tag & litera::TAG_DIGIT))
				goto BAD;
		}

		while (symbol.Tag & litera::TAG_DIGIT)
		{
			m_strTerminalText.cat(symbol.Name);
			symbol=GetNextChar();
		}


		break;

	case STD_NUM_CONST:	
M_DIGIT_CONST:
	{
		bool Dot=false,E=false,ESign=false,EIm=false;
		if ((!(symbol.Tag & litera::TAG_DIGIT)) && (symbol.Name!='.')	&& (symbol.Name!='-'))  goto BAD;

		if (symbol.Name=='-')
		{
			m_strTerminalText.cat(symbol.Name);
			symbol=GetNextChar();
			while ((symbol.Name == '	') || (symbol.Name == '\t'))	
				symbol=GetNextChar();
			if (!(symbol.Tag & litera::TAG_DIGIT) && (symbol.Name!='.'))
				goto BAD;
		}

		while ((symbol.Tag & litera::TAG_DIGIT)|| (symbol.Name=='.')	||
			   (symbol.Name=='E') ||	(symbol.Name=='e') ||
			   (symbol.Name=='+') ||	(symbol.Name=='-'))
		{
			if (symbol.Name=='.')
			{
				if (Dot) break;
				else Dot=true;
				EIm=false;
			}
			else if	((symbol.Name=='E')|| (symbol.Name=='e'))
			{
				if (E) goto	BAD;
				E=true;
				Dot=true;
				EIm=true;
			}
			else if	((symbol.Name=='+')|| (symbol.Name=='-'))
			{
				if ((ESign)	&& (!EIm)) goto	BAD;
				if (!EIm) break;
				ESign=true;
				EIm=false;
			}
			m_strTerminalText.cat(symbol.Name);
			symbol=GetNextChar();
		}
		if (!(symbol.Tag & litera::TAG_SYMBOL) && m_bIgnoreWhiteSpaces)
			goto BAD;
		break;
	}
	case STD_DQMSTR_CONST:
M_DOUBLE_QUOTES_CONST:
		//symbol=GetNextChar();
		if (symbol.Name!='\"')
			goto BAD;
		do
		{
			m_strTerminalText.cat(symbol.Name);
			symbol=GetNextChar();
			if (symbol.Tag & litera::TAG_END)
				goto BAD;
		}while (symbol.Name!='"');
		
		m_strTerminalText.cat(symbol.Name);
		GetNextChar();
		break;
	case STD_SQMSTR_CONST:
M_SINGLE_QUOTES_CONST:
		//symbol=GetNextChar();
		if (symbol.Name!='\'')
			goto BAD;
		do
		{
			m_strTerminalText.cat(symbol.Name);
			symbol=GetNextChar();
			if (symbol.Tag & litera::TAG_END)
				goto BAD;
		}while (symbol.Name!='\'');
		
		m_strTerminalText.cat(symbol.Name);
		GetNextChar();
		break;	
	case STD_STR_CONST:
		if (symbol.Name=='\"')
			goto M_DOUBLE_QUOTES_CONST;
		else if (symbol.Name=='\'')
			goto M_SINGLE_QUOTES_CONST;
		goto BAD;
	case STD_ALPHA:
		if (!isalpha(symbol.Name))
			goto BAD;
		m_strTerminalText.cat(symbol.Name);
		GetNextChar();
		break;
	case STD_NUM:
		if (!isdigit(symbol.Name))
			goto BAD;
		m_strTerminalText.cat(symbol.Name);
		GetNextChar();
		break;
	case STD_ALNUM:
		if (!isalnum(symbol.Name))
			goto BAD;
		m_strTerminalText.cat(symbol.Name);
		GetNextChar();
		break;
	case STD_PUNCT:
		if (!ispunct(symbol.Name))
			goto BAD;
		m_strTerminalText.cat(symbol.Name);
		GetNextChar();
		break;
	case STD_CNTRL:
		if (!iscntrl(symbol.Name))
			goto BAD;
		m_strTerminalText.cat(symbol.Name);
		GetNextChar();
		break;
	}
	//	Проверяем, если это не ключевое слово, все нормально
	#pragma message(__MSG_PREFIX__"11.04.2006 THE BUG. THERE IS NO CASE ISENCE SUPPORT")
	if (m_bIgnoreCase) //11.04.2006. Не учитывался реестр букв
	{
		_ASSERTE(false);
	}
	else 
	{
		if (m_mapKeyWords.find(m_strTerminalText.c_str())==m_mapKeyWords.end())
			goto GOOD;
	}
BAD:
	m_bGetStdTerminalOK=false;
	ReStoreState();
	return false;
GOOD:
	StepBack();
	m_bGetStdTerminalOK=true;
	return true;
}
//------------------------------------------------------------------------
bool CLexSource::GetKeyword(OUT string_smart & strResult)
{
	for (map_keywords::iterator i = m_mapKeyWords.begin(); i!=m_mapKeyWords.end();++i)
		if (GetAndCompare((*i).first.c_str())){
			strResult = (*i).first.c_str();
			return true;
		}
	return false;
}
//------------------------------------------------------------------------
bool CLexSource::GetAndCompare(const cstr LexemeName)
{
	_ASSERT(LexemeName);
	if (!*LexemeName)	//Лексема пуста, а
		return true;	//значит, вполне подходит
						//эта возможность создавать	пустые
						//лексемы была добавлена в ver 2.6.
	StoreState();

	litera	Char; int i;
	for	(i=0;LexemeName[i];i++)
	{
		Char=GetNextChar();
		if (LexemeName[i]>=32) // if <32, Не обращая внимание на настройки,	разбираем
		{
			if ((Char.Tag &	litera::TAG_SPACE) && (m_bIgnoreWhiteSpaces))
			{
				i--;
				continue;
			}
		}
		if (m_bIgnoreCase)
		{
			if (str_to_upper(Char.Name)!=str_to_upper(LexemeName[i]))
			{
				ReStoreState();
				return false;
			}
		}
		else
		{
			if (Char.Name!=LexemeName[i])
			{
				ReStoreState();
				return false;
			}
		}
	}
	if ((m_bSolitaryWords) && (LexemeName[i-1]>=32))
	{
		Char = LookNextChar();	//Просматриваем, что за следующий символ
		if ((Char.Tag &	litera::TAG_END) ||	(Char.Tag &	litera::TAG_SPACE) || (m_strDelimiters.pos(Char.Name,0)!=-1))
			; // good
		else if (m_strDelimiters.pos(LexemeName[i-1],0)!=-1)
			; //good
		else
		{
			ReStoreState();
			return false;
		}
	}
	return true;
}
//------------------------------------------------------------------------
bool CLexSource::GetRegExpTerminal(cstr szRegExp)
{
	class my_regex: public boost::regex {friend bool CLexSource::GetRegExpTerminal(cstr szRegExp);};
	typedef boost::re_detail::is_byte<char>::width_type width_type;


	boost::match_results<re_it> what;
	boost::regex_constants::match_flag_type	flags = boost::match_default; 
	
	map_regex::iterator m_it = m_mapRegex.find(szRegExp);
	boost::regex & expr = (m_it==m_mapRegex.end())
		? m_mapRegex[szRegExp]=boost::regex(szRegExp)
		: m_it->second;
	my_regex & expression = (my_regex &)(expr);

	StoreState();

	litera symbol;
	if (m_bIgnoreWhiteSpaces){
		do 
		{
			symbol=GetNextChar();
		} while	((symbol.Tag & litera::TAG_SPACE));
		StepBack();
	}

	re_it first(m_CurrentState.m_SourcePos); 
	re_it last(m_lstSource.end());

	char_t first_symbol = (*first);
	if (expression.can_start(first_symbol, expression.get_map(expression), boost::re_detail::mask_any, width_type())) {
		if (boost::regex_search(first, last, what, expression,flags))
		{
			// what[0] contains the whole string
			// what[1] contains the response code
			// what[2] contains the separator character
			// what[3] contains the text message.
			m_CurrentState.m_SourcePos=what[0].second;
			m_strTerminalText.clear();
			for (re_it it = what[0].first; it!=what[0].second;++it)
				m_strTerminalText.cat(*it);
			return true;
		}
	}

	ReStoreState();
	return false;
}
//------------------------------------------------------------------------
bool CLexSource::OpenFile(cstr szFileName)
{
	if (*szFileName==0)
	{
		//CLexMessage::Out("Error: empty filename");
		//CLexMessage::Out(ENDL);
		return false;
	}

	string_smart strSourceText;
	if (!base_utils::load_file(szFileName,strSourceText))
		return false;

	//Добавляем текст во временный список
	vector<symbol_info_ex> lstSource;
	typedef vector<symbol_info_ex>::iterator scr_ex_it;
	lstSource.reserve(strSourceText.length());

	size_t nLine=1, nPos =1;
	for	(size_t i=0;i<strSourceText.length();i++){
		lstSource.push_back(symbol_info_ex(strSourceText.c_str()[i],nLine,nPos));

		if (strSourceText.c_str()[i]=='\n'){
			nPos=1; nLine++;}
		else
			nPos++;
	}

	//Находим в исходном тексте все ReplaceBlock
	for	(lst_r_blocks::iterator i=m_lstReplace.begin();i!=m_lstReplace.end();++i)
		for (cstr pText = strSourceText.c_str(); pText; pText+=(*i).m_strOld.length())
		{
			string_parse strParse;
			pText=strParse.find(pText,(*i).m_strOld.c_str());
			if (!pText)	break;

			long x = pText-strSourceText.c_str();
			if (m_ParseCallBckFunc)
			{
				string_smart	strMsgText = (*i).m_strOld.c_str();;
				grm_parse_packet	Msg;
				Msg.Type=GE_NULL;
				Language==LANG_RUSSIAN		? Msg.szMessage="Меняем	"
											: Msg.szMessage="Replace ";
				Msg.szText=strMsgText.c_str();
				Msg.szCurHubName=NULL;
				Msg.dwCmd=grmkrnl_exports::parse_info_packet::nfoMSG;
				Msg.dwRecursionLevel=0;
				Msg.szCurHubName = szFileName;
				Msg.dwLine = lstSource[x].m_nLine;
				Msg.dwPos  = lstSource[x].m_nPos;
				m_ParseCallBckFunc(Msg);
			}

			lstSource[x].m_nAction=ac_replace;
			lstSource[x].m_szData= (*i).m_strNew.c_str();
			for (size_t j=1; j<(*i).m_strOld.length();j++)
				lstSource[x+j].m_nAction = ac_remove;
		}

	//Выкидываем из исходного кода все IgnoreBlock

	string_parse strParse;					//Нельзя искать внутри строк
	strParse.AddSkipRanges("\"","\"");		//не совсем универсальный подход
	strParse.AddSkipRanges("\'","\'");		//если строки будут описываться как-то особо, мы пролетим, надо дорабатывать

	//?? for (lst_i_blocks::iterator i=m_lstIgnore.begin();i!=m_lstIgnore.end();++i)
	{
		for	(lst_i_blocks::iterator i=m_lstIgnore.begin();i!=m_lstIgnore.end();++i)
			for (cstr pText = strSourceText.c_str(); pText; )
			{
				string_smart strStart = (*i).m_strStart.c_str();
				string_smart strEnd = (*i).m_strEnd.c_str();

				cstr szLeftRange = strParse.find(pText,strStart.c_str());
				if (szLeftRange==NULL) 
					break;
				cstr szRightRange= strParse.find(szLeftRange+strStart.length(),strEnd.c_str());
		
				size_t x1 = szLeftRange-strSourceText.c_str();
				size_t x2 = (szRightRange!=NULL)
									? szRightRange-strSourceText.c_str()+strEnd.length()
									: strSourceText.length();
				_ASSERT(x2>=x1);
				memset((str)szLeftRange,255,x2-x1);
				
				if (m_ParseCallBckFunc)
				{
					string_smart		strMsgText((*i).m_strStart.c_str(),"...");
					grm_parse_packet	Msg;
					Msg.Type=GE_NULL;
					Language==LANG_RUSSIAN
						? Msg.szMessage="Игнорируем "
						: Msg.szMessage="Ignore ";
					Msg.szText=strMsgText.c_str();
					Msg.szCurHubName=NULL;
					Msg.dwCmd=grmkrnl_exports::parse_info_packet::nfoMSG;
					Msg.dwRecursionLevel=0;
					Msg.szCurHubName = szFileName;
					Msg.dwLine = lstSource[x1].m_nLine;
					Msg.dwPos  = lstSource[x1].m_nPos;
					m_ParseCallBckFunc(Msg);
				}
				
				for (size_t j = x1; j<x2; j++)
					lstSource[j].m_nAction = ac_remove;

				if (szRightRange==NULL)
					break;
				pText=szRightRange+strEnd.length(); //Передвигаем маркер как можно дальше до граничы проверенной зоны
			}
	}

	//Теперь перекачиваем получившийся временный массив в постоянный
	m_lstSource.clear();
	for (scr_ex_it i = lstSource.begin(); i!=lstSource.end();++i)
		switch ((*i).m_nAction){
		case ac_none:	
			m_lstSource.push_back(symbol_info(*i)); 
			break;
		case ac_replace: 
			for (size_t j=0, l = strlen((*i).m_szData); j<l; j++)
				m_lstSource.push_back(symbol_info((*i).m_szData[j],(*i).m_nLine,(*i).m_nPos)); 
		}
	
	string_smart strInterMed(szFileName,".imd");

	file_std File;
	if (File.open(strInterMed.c_str(),"wt"))
		for (src_pos i = m_lstSource.begin(); i!=m_lstSource.end(); ++i)
			File<<(*i).m_cValue;

	m_CurrentState.m_SourcePos=m_lstSource.begin();

	m_strFileName = szFileName;
	return true;
}
//------------------------------------------------------------------------
void CLexSource::ResetPos()
{
	m_CurrentState.m_SourcePos=m_lstSource.begin();
	m_CurrentState.m_nOverEnd=0;
	m_PrevState= m_CurrentState;
}
//------------------------------------------------------------------------
bool CLexSource::isEnd() const
{
	//Для случая с пропускаемыми пробелами заглядываем вперед
	if (m_bIgnoreWhiteSpaces)
	{
		char_t S;
		src_pos CSP=m_CurrentState.m_SourcePos;
		while (CSP!=m_lstSource.end())
		{
			S=(*CSP).m_cValue;	
			if (!isspace(S)) 
				break;
			CSP++;
		}
		return (CSP==m_lstSource.end());
	}
	//Для простейшего случая просто смотрим позицию
	else
	{
		return m_CurrentState.m_SourcePos==m_lstSource.end();
	}
}
//------------------------------------------------------------------------
void CLexSource::AddIgnoreBlock(cstr szStart, cstr szEnd)
{
	m_lstIgnore.push_back(ignore_block_data(szStart,szEnd));
}
//------------------------------------------------------------------------
void CLexSource::AddReplaceBlock(cstr szOld, cstr szNew)
{
	m_lstReplace.push_back(replace_block_data(szOld,szNew));
}
//------------------------------------------------------------------------
bool CLexSource::ParseNext(OUT string_smart & strResult, OUT ELexemeCLass & nLexClass, OUT EStdTerminalClass & nStdTermClass)
{
	litera symbol;
	do 
	{
		symbol=GetNextChar();
	} while	((symbol.Tag & litera::TAG_SPACE));

	if (symbol.Tag & litera::TAG_END) 
		return false;

	strResult.fill_char(symbol.Name,1);
	nLexClass = OPERATOR_CLASS_ID;
	nStdTermClass=NO_STD_TERMINAL;

	if ((symbol.Name!='\"') && (symbol.Name!='\''))	//надо подумать о чем-то более надежном
		if (m_bIgnoreCase){
			if (m_strDelimiters.posi(symbol.Name)!=-1)
				return true;
		}
		else{
			if (m_strDelimiters.pos(symbol.Name)!=-1)
				return true;
		}

	if (GetKeyword(strResult)){
		nLexClass = WORD_CLASS_ID;
		nStdTermClass = NO_STD_TERMINAL;
		return true;
	}

	if ((GetStandardTerminal((nLexClass=ID_CLASS_ID,nStdTermClass=STD_ID))) ||
		(GetStandardTerminal((nLexClass=CONST_CLASS_ID,nStdTermClass=STD_NUM_CONST))) ||
		(GetStandardTerminal((nLexClass=CONST_CLASS_ID,nStdTermClass=STD_INT_CONST))) ||
		(GetStandardTerminal((nLexClass=CONST_CLASS_ID,nStdTermClass=STD_DQMSTR_CONST))) ||
		(GetStandardTerminal((nLexClass=CONST_CLASS_ID,nStdTermClass=STD_SQMSTR_CONST))) ||
		(GetStandardTerminal((nLexClass=CONST_CLASS_ID,nStdTermClass=STD_COMMON_CONST))))
	{
		strResult=m_strTerminalText.c_str();
		return true;
	}
		
	nLexClass=OPERATOR_CLASS_ID;
	nStdTermClass=NO_STD_TERMINAL;
	return true;
}
//------------------------------------------------------------------------
void CLexSource::AddKeyword(cstr szKeyword)
{
	m_mapKeyWords[szKeyword]=true;
}
//------------------------------------------------------------------------
bool CLexSource::Parse(OUT list<lexeme_ex> & LexLst)
{
	ResetPos();
	
	string_smart s;
	grmkrnl_exports::parse_info_packet info;
	EStdTerminalClass nStdTermClass;
	ELexemeCLass nLexClass;

	while (ParseNext(s,nLexClass,nStdTermClass))
	{
		if (m_ParseCallBckFunc)
		{
			//----
			info.dwLine = GetCurLine();
			info.dwPos  = GetCurCol();
			info.szFileName = m_strFileName.c_str();
			info.szText = s.c_str();
			info.LexemeType = nLexClass;
			info.StandardTerminal = nStdTermClass;
			info.dwCmd=grmkrnl_exports::parse_info_packet::nfoFOUND;
			//---
			m_ParseCallBckFunc(info);
		}
		lexeme_ex L(s.c_str(),nLexClass,nStdTermClass);
		L.SetLocation(grm_structs::text_start_end_location(GetCurrentLocation(),1,s.length()));
		LexLst.push_back(L);
	}

	return true;
}