/********************************************************************
	created:	2003/06/10
	created:	10:6:2003   20:49
	filename: 	$(Projects)\sources\grm compiler\smntanalyzer.h
	file path:	$(Projects)\sources\grm compiler
	file base:	smntanalyzer
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/
#pragma once

#include "BaseStructs.h"

class CSmntAnalyzer
{
private:
	class analyze_context: public ISmntAnalyzeContext
	{
	private:
		const structs::lexeme*	m_pLexeme;

		const ILexList * m_pLexList;
		const ISntxList* m_pSntxList;
		const ILexTable* m_pLexTable;
		
		IImportTable*	m_pImportTable;
		ISntxTable *	m_pSntxTable;
		IPCodeList*	 	m_pPCodeList;
		ITypeTable*		m_pTypeTable;
		const ILangGenTable*  m_pLangGenTable;
	protected:
		override const ILexTable& GetLexTable()	const {
			return *m_pLexTable;
		}
		override const ILexList& GetLexList() const{
			return *m_pLexList;
		}
		override const ISntxList& GetSntxList()const{
			return *m_pSntxList;
		}
		override const ILangGenTable&	GetLangGenTable()	const {
			return *m_pLangGenTable; 
		}
		override IImportTable& GetImportTable()const{
			return *m_pImportTable;
		}
		override IPCodeList& GetPCodeList()	const{
			return *m_pPCodeList;
		}
		override ITypeTable& GetTypeTable()	const{
			return *m_pTypeTable;
		}
		override ISntxTable& GetSntxTable()	const{
			return *m_pSntxTable;
		}
		override const structs::lexeme& GetLexeme() const {
			return *m_pLexeme;
		}

	public:
		void init(const ILexTable & LexTable, IImportTable & ImportTable, const ILangGenTable & LangGenTable,
				  ITypeTable & TypeTable, ISntxTable & SntxTable, 
						  const ILexList& Lexlist,const ISntxList& SntxList, IPCodeList & PCodeList){
			m_pLexTable=&LexTable;
			m_pLexList=&Lexlist;
			m_pSntxList=&SntxList;
			m_pPCodeList=&PCodeList;
			m_pTypeTable=&TypeTable;
			m_pImportTable=&ImportTable;
			m_pLangGenTable=&LangGenTable;
			m_pSntxTable = &SntxTable;
		}
		bool invoke(pf_sntx_func pFunc,const structs::lexeme & Lexeme)
		{
			m_pLexeme=&Lexeme;
			return pFunc(*this);
		}
	};

	base_utils::file_std m_DiagnosticFile;
	analyze_context m_Context;

	void WriteToDiagnosticFile(cstr szText,...);
public:
	bool Analyze(const ILexTable & LexTable,IImportTable & ImportTable, const ILangGenTable & LangGenTable, 
				 ITypeTable & TypeTable,ISntxTable & SntxTable,
				 const ILexList & LexList, ISntxList & SntxList, IPCodeList & PCodeList);
	void Clear()	{}

	CSmntAnalyzer(const IBuildContext & BuildContext, cstr szFileName);
	~CSmntAnalyzer(void);
};
