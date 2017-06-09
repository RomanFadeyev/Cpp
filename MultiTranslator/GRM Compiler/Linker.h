/********************************************************************
	created:	2003/06/12
	created:	12:6:2003   20:50
	filename: 	$(Projects)\sources\grm compiler\linker.h
	file path:	$(Projects)\sources\grm compiler
	file base:	linker
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#pragma once

#include "BaseStructs.h"
#include <list>

class CLinker
{
private:
	struct module_data{
		const IConstTable& m_ConstTable; 
		const ITypeTable & m_TypeTable;
		const IPCodeList & m_PCodeList;
		string_smart m_strFileName;
		
		module_data(cstr szFileName,const IConstTable & ConstTable, const ITypeTable & TypeTable, const IPCodeList & PCodeList):	
			m_ConstTable(ConstTable),m_TypeTable(TypeTable), m_PCodeList(PCodeList), m_strFileName(szFileName)
		{}
	};
	const IImportTable * m_pImportTable;

	typedef structs::code_cmd code_cmd;
	typedef list<module_data> list_module;
	typedef list<code_cmd> list_commands;
	list_module		m_lstModules;
	size_t			m_nStart;
	list_commands	m_lstCommands;
	list_commands	m_lstInitCommands;

	const IBuildContext & m_BuildContext;

	bool	SaveDebugInfo(stream &, const code_cmd & cmd);
	bool	Save2BinFile(cstr szFileName, bool isDebug);
	bool	Save2SFXFile(cstr szFileName, bool isDebug);
	bool	SaveData(stream & Stream, stream & DebugInfoStream);
public:
	void AddModule(cstr szModuleName,const IConstTable & ConstTable, const ITypeTable & TypeTable, const IPCodeList & PCodeList);

	//Сохраняет все модули в виде obj-файлов
	void SaveObjectFiles();
	
	//Линкует все собранные модули в один исполняемый файл
	//Расширение можно не передавать, так как оно формируется автоматически
	bool Link(cstr szOutputFileName, bool bMakeSfx,const IImportTable & ImportTable);


	void AddInitCommand(code_cmd cmd)	{
		m_lstInitCommands.push_back(cmd);
	}
	void Clear()		{
		m_lstModules.clear(); m_lstInitCommands.clear();m_lstCommands.clear();
	}

	CLinker(const IBuildContext & BuildContext);
	~CLinker(void);
};
