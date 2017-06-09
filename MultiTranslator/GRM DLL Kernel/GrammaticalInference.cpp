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

	//�� ������ ������� ����� ������� ������ � �������������
	if (oneof(m_Lexeme.m_nStandardTerminal,STD_NUM_CONST,STD_INT_CONST) && 
		oneof(s.m_Lexeme.m_nStandardTerminal,STD_NUM_CONST,STD_INT_CONST))
		return true;

	//�� ������ ������� ����� �������� ��������� � ����������
	if (oneof(m_Lexeme.m_nStandardTerminal,STD_DQMSTR_CONST, STD_SQMSTR_CONST) && 
		oneof(s.m_Lexeme.m_nStandardTerminal,STD_DQMSTR_CONST, STD_SQMSTR_CONST))
		return true;

	//��������� ���� ������������ ��������� ������ ����� ��������������� ���� �����
	if (m_Lexeme.m_nStandardTerminal == s.m_Lexeme.m_nStandardTerminal)
		return true;

	//������ ��������� ��������������� � ��������, ������� �� �������� ����� �����������
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

//�������� ������ �� ���������� �� ��� ����������
bool CGrammaticalInference::gi_rule::expose_link(gi_rule * pRule)
{
	list_symbols::iterator si = std::find(m_lstSymbols.begin(),m_lstSymbols.end(),pRule);
	if (si==m_lstSymbols.end()) 
		return false;
	
	//��� �� � ����� ��� �� ���� ��������
	list_symbols Lst = pRule->m_lstSymbols;
	m_lstSymbols.insert(m_lstSymbols.erase(si),Lst.begin(),Lst.end());
	return true;
}

void CGrammaticalInference::gi_rule::chunk(list_symbols & lst)
{
	//���������� ��� ������� ������  � �������� ������������������
	//� ������� ����� ���������������������� ������ �������
	list_symbols::iterator i = std::search(lst.begin(),lst.end(),m_lstSymbols.begin(),m_lstSymbols.end(),gi_symbol::compare);

	//���� N �������� ��������� ��������� � ��������, ������� ��
	//� ��������� ������ ��� ���� �������
	if (i!=lst.end()){
		list_symbols::iterator j=i;
		for (size_t x=0;x<m_lstSymbols.size();++x,++j);	//������� ����� ���������������������
		lst.insert(lst.erase(i,j),gi_symbol(this));//������� � ��������� ������ �� ����
	}
}

void CGrammaticalInference::gi_rule::process()
{
	//��������� ���� �������� �����������
	//�������� ����� �� ���� ������. ���������� ������ ������� ���� ��-�� ��������� ��������� ��� chunk
	for (list_gi_rule::iterator y = m_Owner.begin(); y!=m_Owner.end();++y)
		for (list_gi_rule::iterator x = m_Owner.begin(); x!=m_Owner.end();++x)
			if (*x!=this)
				(*x)->chunk(m_lstSymbols);
	
	//��� ���� ��� ������� 2 ������� �� 2 �������, ����� ���������� ��������
	if (m_lstSymbols.size()<4)
		return;

	//������ ����� ��������� ������ + ��������������, ��� ��� 
	//����� ������� ������ ����������� � �����
	//���������� ������ ��� ��������� �������, ��� ��� ���
	//��������� ���������� ��� ������� ������ �������
	list_symbols::iterator last = --m_lstSymbols.end();		//���������
	list_symbols::iterator prev_last = last; --prev_last;	//�������������
	
	list_symbols::iterator i = search(m_lstSymbols.begin(),prev_last,prev_last,m_lstSymbols.end(),gi_symbol::compare);
	if (i!=prev_last) //���� ����� ���������e
	{
		gi_rule * pNewRule = new gi_rule(m_Owner);
		list_symbols::iterator i1 = i;			//��� ��� ����, ������� ������� ���������
		list_symbols::iterator i2 = i; ++i2;
		pNewRule->add(*i1);		
		pNewRule->add(*i2);	

		//������� ���� �� ��������� �������
		//��������� ������ ���� ������ �� ����� �������
		m_lstSymbols.insert(m_lstSymbols.erase(i1,++i2),pNewRule);
		m_lstSymbols.pop_back();		//������� ����� ��������� 2 ��������
		m_lstSymbols.pop_back();		
		m_lstSymbols.push_back(pNewRule);	//�� �� ���� ����� ��������� ������
		
		process(); //��������� �������
	}
}

void CGrammaticalInference::gi_rule::merge()
{
	for (list_gi_rule::iterator i = m_Owner.begin(); i!=m_Owner.end();++i)
	{
		//���� �� ������� ���������� ����� ���� ������, ������� ����� ����������
		if ((*i!=this) && ((*i)->RefCnt()==1))
		{
			gi_rule * pRule = (*i);

			for (list_gi_rule::iterator j = m_Owner.begin(); j!=m_Owner.end();++j)
			{
				//��� �� � ����� ��� �� ���� ��������
				if ((*j)->expose_link(pRule))
				{
					merge();
					return;
				}
			}
			
			//�� ���� �� ������ �������, �� ����������� ������ ����� ����������� �������
			_ASSERT(false);
		}
	}
}

void CGrammaticalInference::gi_rule::add_lexeme(const gi_lexeme & lex)
{
	m_lstSymbols.push_back(gi_symbol(lex));

	//���� ������ 3-� ��������, �������� �����-���� ����������� ����������
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

	//��������� ����
	file_std file(szDumpFile,"wt");
	int n=0;
	for (list_gi_lexeme::iterator i =m_lstLexeme_Parse.begin(); i!=m_lstLexeme_Parse.end(); ++i,++n)
	{
		gi_lexeme & L = (*i); L.m_nIndex=n;	//����������� ���� �������� ���������� ������
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
	m_pRoot = new gi_rule(m_lstRule_BSS); //��� ����� �������� ���������
	int n=0;

	for (list_gi_lexeme::iterator i =m_lstLexeme_Parse.begin(); i!=m_lstLexeme_Parse.end(); ++i,++n)
	{
		gi_lexeme & L = (*i);
		m_pRoot->add_lexeme(L);

	}
	
	//����������� ���������� ������� � ������� �� ������������
	for (list_gi_rule::iterator i = m_lstRule_BSS.begin(); i!=m_lstRule_BSS.end(); ++i) 
		for (list_gi_rule::iterator i = m_lstRule_BSS.begin(); i!=m_lstRule_BSS.end(); ++i) 
		{
			gi_rule * pRule =(*i); 
			list_gi_rule lr; pRule->get_linked_rules(lr);
			//������� ��� �������, �� ������� ��������� ���� ������� � ��������� �� �� ����� ��������
			for (list_gi_rule::iterator x = lr.begin(); x!=lr.end(); ++x){
				gi_rule * pLinkedRule = (*x);
				pLinkedRule->m_nLinkLevel = max(pLinkedRule->m_nLinkLevel,pRule->m_nLinkLevel+1);
			}
		}
	m_lstRule_BSS.sort(gi_rule::compare_level);

	//��-�� ���������� �������� ������� ����� ������ ��������� �����������������
	//�������� �� � �������
	n=1;
	for (list_gi_rule::iterator j = m_lstRule_BSS.begin(); j!=m_lstRule_BSS.end(); ++j,++n) {
		(*j)->set_name((Format("Rule %d",n)).c_str());
	}

	//����
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

	//����
	file_std file(szDumpFile,"wt");
	for (list_gi_rule::iterator j = m_lstRule_BSS.begin(); j!=m_lstRule_BSS.end(); ++j)
	{
		list_gi_lexeme lst = (*j)->m_lstParsedLexemes;
		lst.unique();
		//�������� ������ ��, ������� ��� ������� �������� �������
		if (lst.size()==1){
		}

		gi_rule * pRule = *j;
		pRule->dump_parse_result(file);
	}
	return NULL;
}

