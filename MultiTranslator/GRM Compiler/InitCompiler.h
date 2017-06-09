/********************************************************************
	created:	2003/06/01
	created:	1:6:2003   10:43
	filename: 	$(Projects)\sources\grm compiler\initcompiler.h
	file path:	$(Projects)\sources\grm compiler
	file base:	initcompiler
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once

#include "LexTables.h"
#include "SntxTables.h"
#include "SntxAnalyzer.h"
#include "LexAnalyzer.h"
#include "TypesTable.h"
#include "Linker.h"

namespace init_compiler
{
	bool InitLexTable(CLexTable & LT);
	bool InitTypesTable(CTypesTable & TT, const CLexTable & LexTables);
	bool InitStdVariables(CLexTable & LT, CSntxTable & ST,CTypesTable & TT, CLinker & Linker);

	bool InitLexAnalyzer(CLexAnalyzer & LA);
	bool InitSntxAnalyzer(CSntxAnalyzer & SyntaxAnalyzer, const CLexTable & LT);
}
