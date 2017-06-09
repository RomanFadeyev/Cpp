/********************************************************************
	created:	2003/06/08
	created:	8:6:2003   19:00
	filename: 	$(Projects)\sources\grm compiler\lexical analysis\expression.h
	file path:	$(Projects)\sources\grm compiler\lexical analysis
	file base:	expression
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#if !defined(AFX_EXPRESSION_H__FD4741A6_FF6D_11D3_A1F4_0000E8D9F1D7__INCLUDED_)
#define AFX_EXPRESSION_H__FD4741A6_FF6D_11D3_A1F4_0000E8D9F1D7__INCLUDED_

#pragma once

#include "BaseStructs.h"

class CLexFunctions  
{
private:
	CLexFunctions(){}	//Конструктор запрещен, так как объет создавать нельзя

	static string_smart		m_strExpression;
	static bool				m_bIsAddingEnabled;
	static variant::type	m_nTagIn;
public:

	//Следующие три метода используются напрямую лексическим анализатором
	static void	Clear()					{ m_strExpression.erase();m_nTagIn=variant::vtNULL;}
	static void	BeginTranslate()		{ m_bIsAddingEnabled = true;}
	static void	EndTranslate(char_t S)	{ if (m_bIsAddingEnabled) m_strExpression.cat(S);}

	static ret_t	SetBool			()	{ return m_nTagIn=variant::vtBOOL,true;  }
	static ret_t	SetChar			()	{ return m_nTagIn=variant::vtCHAR,true;  }
	static ret_t	SetInteger		()	{ return m_nTagIn=variant::vtINT,true;   }
	static ret_t	SetDouble		()	{ return m_nTagIn=variant::vtDOUBLE,true;}
	static ret_t	SetString		()	{ return m_nTagIn=variant::vtSTRING,true;}
	static ret_t	EnableAdding	()	{ return m_bIsAddingEnabled=true,true;	}
	static ret_t	DisableAdding	()	{ return m_bIsAddingEnabled=false,true;	}
	static ret_t	DeleteLast		()	{ m_strExpression.del_last(); return !m_strExpression.isempty();}
	
	static ret_t AddConst		(const ILexAnalyzeContext & Context);
	static ret_t AddID			(const ILexAnalyzeContext & Context);
	static ret_t AddOperator		(const ILexAnalyzeContext & Context);

	static ret_t TryReplaceEscSeq(const ILexAnalyzeContext & Context);	
	static ret_t GetAllSimilary	(const ILexAnalyzeContext & Context);
};

#endif // !defined(AFX_EXPRESSION_H__FD4741A6_FF6D_11D3_A1F4_0000E8D9F1D7__INCLUDED_)
