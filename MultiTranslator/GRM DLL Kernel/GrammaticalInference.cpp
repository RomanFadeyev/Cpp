/********************************************************************
	created:	2004/06/08
	created:	8:6:2004   18:42
	filename: 	X:\MultiTranslator\sources\grm dll kernel\grammaticalinference.cpp
	file path:	X:\MultiTranslator\sources\grm dll kernel
	file base:	grammaticalinference
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/
#include "stdafx.h"
#include "GrammaticalInference.h"

using namespace grmkrnl_structs;
using namespace grmkrnl_exports;

long CGrammaticalInference::gi_rule::m_nID=0;

//------------------------------------------------------------------------
//-- gi_lexeme --
//------------------------------------------------------------------------
void CGrammaticalInference::gi_lexeme::dump(base_utils::stream & s) const
{
	if (!is_std_terminal())
		s<<GetSzText();
	else
		s<<grm_structs::GetStdTerminalClassName(m_nStandardTerminal);
}

//------------------------------------------------------------------------
//-- gi_symbol --
//------------------------------------------------------------------------
bool CGrammaticalInference::gi_symbol::like(const gi_symbol & s) const 
{
	if (is_terminal()!=s.is_terminal())
		return false;
	if (!is_terminal())
		return (m_pLinkedRule==s.m_pLinkedRule);

	//Не делаем разницы между числами целыми и вещественными
	if (oneof(m_Lexeme.m_nStandardTerminal,STD_NUM_CONST,STD_INT_CONST) && 
		oneof(s.m_Lexeme.m_nStandardTerminal,STD_NUM_CONST,STD_INT_CONST))
		return true;

	//не делаем разницы между двойными кавычками и одинарными
	if (oneof(m_Lexeme.m_nStandardTerminal,STD_DQMSTR_CONST, STD_SQMSTR_CONST) && 
		oneof(s.m_Lexeme.m_nStandardTerminal,STD_DQMSTR_CONST, STD_SQMSTR_CONST))
		return true;

	//остальные типы стандартного терминала должны точно соответствовать друг другу
	if (m_Lexeme.m_nStandardTerminal == s.m_Lexeme.m_nStandardTerminal)
		return true;

	//теперь переходим непосредственно к лексемам, которые не являются ничем стандартным
	return m_Lexeme.Get_Str_Text().cmpi(s.m_Lexeme.Get_Str_Text());
}

void CGrammaticalInference::gi_symbol::dump(base_utils::stream & s) const
{
	if (is_terminal())
		m_Lexeme.dump(s);
	else
		s<<"<"<<m_pLinkedRule->name_cstr()<<">";
}

//------------------------------------------------------------------------
//-- gi_rule --
//------------------------------------------------------------------------
void CGrammaticalInference::gi_rule::get_linked_rules(list_gi_rule & lst)
{
	for (list_symbols::iterator i = m_lstSymbols.begin(); i!=m_lstSymbols.end();++i)
		if (!(*i).is_terminal())
			lst.push_back((*i).get_linked_rule());
}

//Заменяет ссылку на нетерминал на его содержимое
bool CGrammaticalInference::gi_rule::expose_link(gi_rule * pRule)
{
	list_symbols::iterator si = std::find(m_lstSymbols.begin(),m_lstSymbols.end(),pRule);
	if (si==m_lstSymbols.end()) 
		return false;
	
	//Вот мы и нашли кто на него ссылался
	list_symbols Lst = pRule->m_lstSymbols;
	m_lstSymbols.insert(m_lstSymbols.erase(si),Lst.begin(),Lst.end());
	return true;
}

void CGrammaticalInference::gi_rule::chunk(list_symbols & lst)
{
	//Сравниваем все символы подряд  в исходной последовательности
	//в попытке найти продпоследовательность нашего правила
	list_symbols::iterator i = std::search(lst.begin(),lst.end(),m_lstSymbols.begin(),m_lstSymbols.end(),gi_symbol::compare);

	//Если N символов полностью совпадают с правилом, удаляем их
	//и вставляем вместо них наше правило
	if (i!=lst.end()){
		list_symbols::iterator j=i;
		for (size_t x=0;x<m_lstSymbols.size();++x,++j);	//Находим конец подпоследовательности
		lst.insert(lst.erase(i,j),gi_symbol(this));//Удаляем и вставляем ссылку на себя
	}
}

void CGrammaticalInference::gi_rule::process()
{
	//Позволяем всем правилам попробовать
	//заменить текст на свою ссылку. Приходится делать двойной цикл из-за каскадных изменений при chunk
	for (list_gi_rule::iterator y = m_Owner.begin(); y!=m_Owner.end();++y)
		for (list_gi_rule::iterator x = m_Owner.begin(); x!=m_Owner.end();++x)
			if (*x!=this)
				(*x)->chunk(m_lstSymbols);
	
	//Нам надо как минимум 2 правила по 2 символа, чтобы образовать иерархию
	if (m_lstSymbols.size()<4)
		return;

	//Всегда берем последний символ + предопоследний, так как 
	//новые символы всегда добавляются в конец
	//Сравниваем только два последних символа, так как эта
	//процедура вызывается для каждого нового символа
	list_symbols::iterator last = --m_lstSymbols.end();		//Последний
	list_symbols::iterator prev_last = last; --prev_last;	//Предпоследний
	
	list_symbols::iterator i = search(m_lstSymbols.begin(),prev_last,prev_last,m_lstSymbols.end(),gi_symbol::compare);
	if (i!=prev_last) //Есть такое повторениe
	{
		gi_rule * pNewRule = new gi_rule(m_Owner);
		list_symbols::iterator i1 = i;			//Вот эта пара, которая требует замещения
		list_symbols::iterator i2 = i; ++i2;
		pNewRule->add(*i1);		
		pNewRule->add(*i2);	

		//Удаляем пару из исходного правила
		//Добавляем вместо него ссылку на новое правило
		m_lstSymbols.insert(m_lstSymbols.erase(i1,++i2),pNewRule);
		m_lstSymbols.pop_back();		//Удаляем также последние 2 элемента
		m_lstSymbols.pop_back();		
		m_lstSymbols.push_back(pNewRule);	//На их тоже место вставляем ссылку
		
		process(); //Повторяем процесс
	}
}

void CGrammaticalInference::gi_rule::merge()
{
	for (list_gi_rule::iterator i = m_Owner.begin(); i!=m_Owner.end();++i)
	{
		//Если на правило существует всего одна ссылка, правило нужно совместить
		if ((*i!=this) && ((*i)->RefCnt()==1))
		{
			gi_rule * pRule = (*i);

			for (list_gi_rule::iterator j = m_Owner.begin(); j!=m_Owner.end();++j)
			{
				//Вот мы и нашли кто на него ссылался
				if ((*j)->expose_link(pRule))
				{
					merge();
					return;
				}
			}
			
			//Мы сюда не должны попасть, мы обязательно должны найти ссылающийся элемент
			_ASSERT(false);
		}
	}
}

void CGrammaticalInference::gi_rule::add_lexeme(const gi_lexeme & lex)
{
	m_lstSymbols.push_back(gi_symbol(lex));

	//Пока меньше 3-х символов, выводить какую-либо зависимость невозможно
	if (count()<3)
		return;
	
	process();
	merge();
}

void CGrammaticalInference::gi_rule::dump(base_utils::stream & s) const
{
	s<<name_cstr()<<" ::= ";
	for (list_symbols::const_iterator i = m_lstSymbols.begin(); i!=m_lstSymbols.end(); ++i)
	{
		(*i).dump(s);
		s<<" ";
	}
	s<<"\n";
}

void CGrammaticalInference::gi_rule::parse(list_gi_lexeme_it & itCur)
{
	for (list_symbols::iterator i = m_lstSymbols.begin(); i!=m_lstSymbols.end();++i)
	{
		m_lstParsedLexemes.push_back(*itCur);

		if ((*i).is_terminal()){
			_ASSERT((*i).like(*itCur));
			++itCur;
		}
		else {
			(*i).get_linked_rule()->parse(itCur);
		}
	}
}

void CGrammaticalInference::gi_rule::dump_parse_result(base_utils::stream & s) const
{
	s<<name_cstr()<<": ";	
	for (list_gi_lexeme::const_iterator i = m_lstParsedLexemes.begin();i!=m_lstParsedLexemes.end();++i)
		s<<(*i).GetSzText()<<" ";
	s<<"\n";
}

//------------------------------------------------------------------------
//-- CGrammaticalInference --
//------------------------------------------------------------------------
void CGrammaticalInference::ParseLexical(cstr szDumpFile)
{
	list<lexeme_ex> lstLex;
	m_LexSource.Parse(lstLex);
	
	m_lstLexeme_Parse.clear();
	for (list<lexeme_ex>::iterator i= lstLex.begin(); i!=lstLex.end();++i)
		m_lstLexeme_Parse.push_back(*i);

	//Сохраняем файл
	file_std file(szDumpFile,"wt");
	int n=0;
	for (list_gi_lexeme::iterator i =m_lstLexeme_Parse.begin(); i!=m_lstLexeme_Parse.end(); ++i,++n)
	{
		gi_lexeme & L = (*i); L.m_nIndex=n;	//Расставляем всем лексемам порядковые номера
		file<<itos(L.m_nIndex)<<": ";
		L.dump(file);
		if (L.is_std_terminal())
			file<<" \t"<<L.GetSzText();
		file<<"\n";
	}
};

void CGrammaticalInference::FindCorrespondence(cstr szDumpFile)
{
}

void CGrammaticalInference::BuildSyntaxStructure(cstr szDumpFile)
{
	m_lstRule_BSS.clear();
	m_pRoot = new gi_rule(m_lstRule_BSS); //Это будет корневой внершиной
	int n=0;

	for (list_gi_lexeme::iterator i =m_lstLexeme_Parse.begin(); i!=m_lstLexeme_Parse.end(); ++i,++n)
	{
		gi_lexeme & L = (*i);
		m_pRoot->add_lexeme(L);

	}
	
	//Постаряемся расставить правила в порядке их глобальности
	for (list_gi_rule::iterator i = m_lstRule_BSS.begin(); i!=m_lstRule_BSS.end(); ++i) 
		for (list_gi_rule::iterator i = m_lstRule_BSS.begin(); i!=m_lstRule_BSS.end(); ++i) 
		{
			gi_rule * pRule =(*i); 
			list_gi_rule lr; pRule->get_linked_rules(lr);
			//Находим все правила, на которые ссылается наше правило и вставляем их за нашим правилом
			for (list_gi_rule::iterator x = lr.begin(); x!=lr.end(); ++x){
				gi_rule * pLinkedRule = (*x);
				pLinkedRule->m_nLinkLevel = max(pLinkedRule->m_nLinkLevel,pRule->m_nLinkLevel+1);
			}
		}
	m_lstRule_BSS.sort(gi_rule::compare_level);

	//Из-за постоянных удалений вставок имена правил абсолютно непоследовательны
	//Приводим их в порядок
	n=1;
	for (list_gi_rule::iterator j = m_lstRule_BSS.begin(); j!=m_lstRule_BSS.end(); ++j,++n) {
		(*j)->set_name((Format("Rule %d",n)).c_str());
	}

	//Дамп
	file_std file(szDumpFile,"wt");
	for (list_gi_rule::iterator j = m_lstRule_BSS.begin(); j!=m_lstRule_BSS.end(); ++j)
	{
		gi_rule * pRule = *j;
		pRule->dump(file);
	}
}

CGrammaticalInference::list_wrd * CGrammaticalInference::SearchWordRepetition(cstr szDumpFile)
{
	m_pRoot->parse(m_lstLexeme_Parse.begin());

	//Дамп
	file_std file(szDumpFile,"wt");
	for (list_gi_rule::iterator j = m_lstRule_BSS.begin(); j!=m_lstRule_BSS.end(); ++j)
	{
		list_gi_lexeme lst = (*j)->m_lstParsedLexemes;
		lst.unique();
		//Выбираем только те, которые все правило занимают целиком
		if (lst.size()==1){
		}

		gi_rule * pRule = *j;
		pRule->dump_parse_result(file);
	}
	return NULL;
}

