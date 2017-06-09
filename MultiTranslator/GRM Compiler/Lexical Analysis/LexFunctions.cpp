// m_strExpression.cpp: implementation of the CLexFunctions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LexFunctions.h"

bool		 CLexFunctions::m_bIsAddingEnabled=true;
string_smart CLexFunctions::m_strExpression;
variant::type	 CLexFunctions::m_nTagIn=variant::vtNULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
ret_t CLexFunctions::TryReplaceEscSeq(const ILexAnalyzeContext & Context) 
{
	if (m_strExpression.get_last()=='\\')
		switch (Context.GetLexSource().GetCurSymbol().Name)
		{
			case '0':
				DisableAdding();
				m_strExpression.del_last();
				break;
			case 'a':
				DisableAdding();
				m_strExpression.set_last('\a');
				break;
			case 'n':
				DisableAdding();
				m_strExpression.set_last('\n');
				break;
			case 't':
				m_strExpression.set_last('\t');
				DisableAdding(); 
				break;
			case 'r':
				DisableAdding();
				m_strExpression.set_last('\r');
				break;
			case 'b':
				DisableAdding();
				m_strExpression.set_last('\b');
				break;
			case '"':
				m_strExpression.set_last('"');
				DisableAdding();
				break;
			case '\\':
				DisableAdding();
				break;
			default:
				m_strExpression.del_last();
		}
	return TRUE;
}
//------------------------------------------------------------------------
ret_t CLexFunctions::GetAllSimilary (const ILexAnalyzeContext & Context)
{
	const structs::litera & Etalon = Context.GetEtalonLitera();
	const ILexSource & LexSource = Context.GetLexSource();

	_ASSERT(Etalon.Tag & LexSource.GetCurSymbol().Tag);	//Мы должны стоять на этой позиции

	do{
		m_strExpression.cat(LexSource.GetCurSymbol().Name);
		LexSource.MoveForward();
	}
	while (Etalon.Tag & LexSource.GetCurSymbol().Tag);

	LexSource.MoveBack();
	DisableAdding();
	
	return true;
}
//------------------------------------------------------------------------
ret_t CLexFunctions::AddConst(const ILexAnalyzeContext & Context)
{
	//Если это тип vtINT, то необходимо еще раз проверить, нельзя ли его подсократить
	if (m_nTagIn==variant::vtINT){
		variant v=m_strExpression.c_str();
		v.set_type(m_nTagIn);
		
		int nVal=v.as_int(); 
		if (inrange(nVal,(int)-128,(int)127))
			m_nTagIn=variant::vtSMALL; 
		else if (inrange(nVal,-32768,32767))
			m_nTagIn=variant::vtSHORT;
	}
	
	cstr szSrc;
	if (m_nTagIn==variant::vtSTRING){
		_ASSERT(m_strExpression.c_str()[0]=='\"');	//Так построены правила ЛА
		szSrc = m_strExpression.c_str()+sizeof(char_t);//что впереди всегда следует кавычка
	}
	else
		szSrc = m_strExpression.c_str();

	//if (m_nTagIn==variant::vtCHAR)	//Проблема: если в коде встретятся "a" и 'a', то дело плохо
	//	m_nTagIn=variant::vtSTRING;	//поэтому избавляемся от char
	
	if (m_nTagIn==variant::vtCHAR)
		_ASSERT(m_strExpression.length()==1);

	structs::lexeme L = Context.GetLexTables().AddConst(szSrc,m_nTagIn);

	structs::text_pos endP	(Context.GetLexSource().GetCurPos());
	structs::text_pos startP(endP.m_nLine, endP.m_nCol-strlen(szSrc));

	L.SetLocation(grm_structs::text_start_end_location(Context.GetLexSource().GetFileName(),startP,endP));
	Context.GetLexList().Add(L);
	return true;
}
//------------------------------------------------------------------------
ret_t CLexFunctions::AddID(const ILexAnalyzeContext & Context)
{
	structs::lexeme L = Context.GetLexTables().AddID(m_strExpression.c_str());

	structs::text_pos endP	(Context.GetLexSource().GetCurPos());
	structs::text_pos startP(endP.m_nLine, endP.m_nCol-m_strExpression.length());

	L.SetLocation(grm_structs::text_start_end_location(Context.GetLexSource().GetFileName(),startP,endP));
	Context.GetLexList().Add(L);
	return true;
}
//------------------------------------------------------------------------
ret_t CLexFunctions::AddOperator(const ILexAnalyzeContext & Context)
{
	structs::lexeme L = Context.GetLexTables().GetOperator(m_strExpression.c_str());

	structs::text_pos endP	(Context.GetLexSource().GetCurPos());
	structs::text_pos startP(endP.m_nLine, endP.m_nCol-m_strExpression.length());

	L.SetLocation(grm_structs::text_start_end_location(Context.GetLexSource().GetFileName(),startP,endP));
	Context.GetLexList().Add(L);

	return true;
}