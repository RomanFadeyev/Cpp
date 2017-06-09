/********************************************************************
	created:	2003/06/12
	created:	12:6:2003   20:55
	filename: 	$(Projects)\sources\grm compiler\linker.cpp
	file path:	$(Projects)\sources\grm compiler
	file base:	linker
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "StdAfx.h"
#include <GrmcExports.h>

#include "linker.h"
#include "Resource.h"
#include "EWQueue.h"
#include "DLLSupport.h"

#include "PCode.h"

//------------------------------------------------------------------------
CLinker::CLinker(const IBuildContext & BuildContext):m_BuildContext(BuildContext)
{
}

CLinker::~CLinker(void)
{
}
//------------------------------------------------------------------------
void CLinker::AddModule(cstr szFileName,const IConstTable & ConstTable, const ITypeTable & TypeTable, const IPCodeList & PCodeList)
{
	m_lstModules.push_back(module_data(szFileName,ConstTable,TypeTable,PCodeList));
}
//------------------------------------------------------------------------
void CLinker::SaveObjectFiles()
{
}
//------------------------------------------------------------------------
// формат исполняемого файла
// Первым запоминаем таблицу констант. Первые 4 байта - количество записей в таблице 
// затем - записи. Запись состоит из 4байт типа, /*4байт размерности*/ и собственно константы(поля байтов)
bool CLinker::Save2BinFile(cstr szFileName, bool isDebug)	
{
	base_utils::file_os ExecFile;
	base_utils::file_os DebugFile;
	if (!ExecFile.open(szFileName,GENERIC_WRITE,CREATE_ALWAYS))
	{
		errors::e_file_was_not_opened((cstr)szFileName);
		return false;
	}
	//Файл с DebugInfo
	if (isDebug)
	{
		string_smart strDebugFile= change_fileext(szFileName,".pdb");
		if (!DebugFile.open(strDebugFile.c_str(),GENERIC_WRITE,CREATE_ALWAYS))
		{
 			errors::e_file_was_not_opened(strDebugFile.c_str());
			return false;
		}
	}
	return SaveData(ExecFile,DebugFile);
}
//------------------------------------------------------------------------
bool CLinker::Save2SFXFile(cstr szFileName, bool isDebug)
{
	base_utils::file_os ExecFile;
	base_utils::file_os DebugFile;
	
	//Исполняемый файл
	if (!ExecFile.open(szFileName,GENERIC_WRITE,CREATE_ALWAYS))
	{
 		errors::e_file_was_not_opened((cstr)szFileName);
		return false;
	}

	//Файл с DebugInfo
	if (isDebug)
	{
		string_smart strDebugFile= change_fileext(szFileName,".pdb");
		if (!DebugFile.open(strDebugFile.c_str(),GENERIC_WRITE,CREATE_ALWAYS))
		{
 			errors::e_file_was_not_opened(strDebugFile.c_str());
			return false;
		}
	}
	// Идея: с помощью проекта ExecSFX компилируем EXE, всовываем его сюда в ресурс
	// Затем при генерации достаем, по определенному адресу записываем сигнатуру
	// и его размер, а в конец дописываем BIN - код
	// когда такой файл выполняется, он считывает по известному адресу сигнатуру и 
	// свой собственный размер , и с полученного таким образом адреса начинает чтение BIN
	// Необходимо следить, чтобы в реурсе лежал последний  вариант EXE - файла

	HRSRC hSFXRes=FindResource(NULL,(char_t*)IDR_SFX_EXECUTER,"SFX_EXECUTER");
	_ASSERT(hSFXRes);
	HGLOBAL MemArea= LoadResource(NULL,hSFXRes);
	_ASSERT(MemArea);
	size_t   ResSize= SizeofResource(NULL,hSFXRes);
	_ASSERT(ResSize>0x1000);
	char_t * RawData= (char_t*) LockResource(MemArea);
	_ASSERT(RawData);
	ExecFile.write(RawData,0x3f0);
	ExecFile.write("SFX",4);
	ExecFile.write(&ResSize,4);
	ExecFile.write(&RawData[0x3f0+8],ResSize-(0x3f0+8));

	return SaveData(ExecFile,DebugFile);
}
//------------------------------------------------------------------------
bool CLinker::SaveData(stream & Stream, stream & DIStream)
{
	Stream.write("GRMS",sizeof("GRMS"));
	Stream.write_dword(grmc_exports::nAssembleVersion);
	Stream.write("DEBUG",sizeof("DEBUG"));
	Stream.write_byte(DIStream.isopen());

	//Записываем таблицу модулей в DebugInfo
	if (DIStream.isopen())
	{
		DIStream.write_dword(m_lstModules.size());
		for (list_module::iterator i=m_lstModules.begin();i!=m_lstModules.end();++i)
		{
			module_data & mdata = (*i);
			DIStream.write_bstr((*i).m_strFileName);
		}
	}


	//Временно. Можно работать только с одним файлом
	_ASSERT(m_lstModules.size()>0);
	const IConstTable & ConstTable	= (*m_lstModules.begin()).m_ConstTable; 
	const IPCodeList & PCodeList	= (*m_lstModules.begin()).m_PCodeList;
	const ITypeTable & TypeTable	= (*m_lstModules.begin()).m_TypeTable;
	const IImportTable & ImportTable= *m_pImportTable;

	//--------------- Store Import Func Data-------------
	try
	{
		Stream<<(size_t)ImportTable.GetCount();			// Количество записей
		for (int i=0;i<ImportTable.GetCount();i++)
		{
			const IImportTable::func_data & fdata=ImportTable.GetAt(i);
			fdata.write_dump(Stream);
		}
	}
	catch (...)  
	{
		errors::internal_error("Exception raised while writing a file(an import section)\n");
		return false;
	}

	//--------------- Store Const Array -------------
	try
	{
		Stream<<(size_t)ConstTable.GetConstantCount();				// Количество записей
		for (int i=0;i<ConstTable.GetConstantCount();i++)
		{
			const variant & value =ConstTable.GetConstant(i);

			Stream<<(size_t)value.get_type();		// Тип константы
			if (value.isordinal())
				Stream<<value.as_int();
			else if (value.isdouble())
				Stream<<value.as_double();
			else if (value.isstring()){
				Stream.write_bstr(ConvertCompilerString(value.as_cstr()));
			}
			else
				_ASSERT(false);
		}
	}
	catch (...)
	{
		errors::internal_error("Exception raised while writing a file(Const Section)\n");
		return false;
	}
//-------------------- Store Types Table --------------------
	// первым идет количество типов, затем все типы
	// тип включает: количество членов типа, 
	//	затем все члены (член типа содержит только индекс на другой тип) и их смещения в типе
	//	затем класс типа	(4 байта)
	//	затем тег			(4 байта)
	//	затем размер занимаемой типом памяти (4байта)
	try
	{
		Stream<<TypeTable.GetCount();
		for (int i=0;i<TypeTable.GetCount();i++)
		{
			const structs::type& aType=TypeTable.GetAt(i);
			Stream<<(size_t)aType.GetFieldCount();	// количество членов
			for (int j=0;j<aType.GetFieldCount();j++)
			{
				const SSntxID & aField=aType.GetFieldAt(j);
				_ASSERT(aField.pType!=NULL);
				Stream<<(size_t) aField.pType->GetTypeID();	// член типа - индекс своего типа в таблице
				Stream<<(size_t) aField.GetSize();			// для членов - массивов
				//Stream<<(size_t) TypeArray->GetAt(i).MembersOffset[j];		// смещение члена типа 
			}
			Stream<<(size_t)aType.GetTypeClass();// класс
			Stream<<(size_t)aType.GetType();	// для простых типов
			Stream<<(size_t)aType.GetMemSize();	// Размер памяти
		}
	}
	catch (...)
	{
		errors::internal_error("Exception raised while writing a file(Type Section)\n");
		return false;
	}
//-------------------- StoreGrammatics --------------------
	try
	{ 
		(pIGrmKrnl->StoreRules)(Stream);
	}
	catch (...)
	{
		errors::internal_error("Exception raised while writing a file(Rule Section)\n");
		return false;
	}
//-------------------- StartPoint--------------------
	try
	{
		Stream<<(byte)(m_nStart!=UINT_MAX);
		Stream<<(size_t)m_nStart;
	}
	catch (...)
	{
		errors::internal_error("Exception raised while writing a file(Type Section)\n");
		return false;
	}
//-------------------- Store lst. --------------------
	// первым идет количество команд, затем команды
	// команда включает: операнд1,2,3 и операцию [4байта]
	//	операнд состоит из: класса (1байт), индекса (4байта)
	try
	{
		Stream.write_dword(m_lstCommands.size());
		for (list_commands::iterator i=m_lstCommands.begin();i!=m_lstCommands.end();++i)
		{
			const code_cmd & cmd = *i;
			Stream<<(size_t)cmd.Op1.structs::lexeme::GetClass();	
			Stream<<(size_t)cmd.Op1.structs::lexeme::Index;	
			Stream<<(size_t)cmd.Op2.structs::lexeme::GetClass();	
			Stream<<(size_t)cmd.Op2.structs::lexeme::Index;	
			Stream<<(size_t)cmd.Op3.structs::lexeme::GetClass();	
			Stream<<(size_t)cmd.Op3.structs::lexeme::Index;	
			Stream<<(size_t)cmd.Op4.structs::lexeme::GetClass();	
			Stream<<(size_t)cmd.Op4.structs::lexeme::Index;	
			Stream<<(size_t)cmd.Operation;
			if (DIStream.isopen())
				SaveDebugInfo(DIStream,cmd);
		}
	}
	catch (...)
	{
		errors::internal_error("Exception raised while writing a file(Type Section)\n");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------
bool CLinker::SaveDebugInfo(stream & Stream, const code_cmd & cmd)
{
	try
	{
		size_t ModuleIndex=0;
		
		//Запись имени модуля
		string_std s;
		s=cmd.Op1.GetLocation().m_strFileName;
		if (s.isempty())
			s=cmd.Op2.GetLocation().m_strFileName;
		if (s.isempty())
			s=cmd.Op3.GetLocation().m_strFileName;
		if (s.isempty())
			s=cmd.Op4.GetLocation().m_strFileName;

		if (s.isempty())
			ModuleIndex=m_lstModules.size();
		else
			for (list_module::iterator i=m_lstModules.begin();i!=m_lstModules.end();++i,ModuleIndex++)
			{
				module_data & mdata = (*i);
				if (mdata.m_strFileName.cmpi(s.c_str()))
					break;
			}

		if (ModuleIndex<m_lstModules.size())
			Stream.write_dword(ModuleIndex);
		else
			Stream.write_dword((long)-1);

		//Запись строки
		
		Stream.write_dword(cmd.GetSourceLine());
		_ASSERT(cmd.GetSourceLine()>=0);

		//Запись данных
		Stream.write_bstr(cmd.Op1.Get_Str_Text());
		Stream.write_bstr(cmd.Op2.Get_Str_Text());
		Stream.write_bstr(cmd.Op3.Get_Str_Text());
		Stream.write_bstr(cmd.Op4.Get_Str_Text());	}
	catch (...)
	{
		errors::internal_error("Exception in writing Debug Info file\n");
		return false;
	}
	return true;
}
//------------------------------------------------------------------------
bool CLinker::Link(cstr szOutputFileName,bool bMakeSfx, const IImportTable & ImportTable)
{
	list_commands & lst=m_lstCommands;
	m_pImportTable = &ImportTable;

	//Для начала собираем все модули в один большой список
	//Примечание. Сейчас компилятор генерирует наименования меток уникальные 
	//во всем пространстве модулей, поэтому делать переадресацию нет меток
	//нет необходимости
	for (list_module::iterator i=m_lstModules.begin();i!=m_lstModules.end();++i)
	{
		module_data & mdata = (*i);
		//Добавляем текущий модуль
		for (size_t i=0;i<mdata.m_PCodeList.GetCount();++i){
			lst.push_back(mdata.m_PCodeList.GetAt(i));
		}
	}
	
	// Переносим всю глобальную часть в начало программы - в инициализационный сегмент
	list_commands lst_funcs;
	for (list_commands::iterator i=lst.begin();i!=lst.end();++i)
	{
		//Если попалась функция или процедура, пропускаем ее
		if ((*i).Operation==FUNC)	{
			list_commands::iterator sp=i,ep=i;	//Отмечаем начало и конец интересующего нас диапазона
			for (;(*ep).Operation!=ENDF;++ep)	//
				_ASSERT(ep!=lst.end());			//
			i--;	//Надо отойти на шаг назад, так как эта позиция сейчас перенесется
			lst_funcs.splice(lst_funcs.end(),lst,sp,++ep);//Переносим весь кусок в друной список
		}
		else if ((*i).Operation==SUBROUTINE){
			list_commands::iterator sp=i,ep=i;	//Отмечаем начало и конец интересующего нас диапазона
			for (;(*ep).Operation!=ENDSUBR;++ep)//
				_ASSERT(ep!=lst.end());			//
			i--;	//Надо отойти на шаг назад, так как эта позиция сейчас перенесется
			lst_funcs.splice(lst_funcs.end(),lst,sp,++ep);//Переносим весь кусок в друной список
		}

		_ASSERT(!oneof((*i).Operation,FUNC,ENDF,SUBROUTINE,ENDSUBR));
	}

	lst.push_back(code_cmd(END_ISEGM)); //  без этой команды можно было бы и обойтись, но с ней понятнее
	lst.push_back(code_cmd(HALT)); 
	lst.splice(lst.end(),lst_funcs,lst_funcs.begin(),lst_funcs.end());
	//Переносим дополнительно инициализационную часть
	lst.splice(lst.begin(),m_lstInitCommands,m_lstInitCommands.begin(), m_lstInitCommands.end());

	typedef map<size_t,size_t> map_addr;
	map_addr mapFuncs,mapLabels;

	//Сбор всех объявлений меток и функций
	size_t addr=0;		//Целочисленный вид итератора
	m_nStart=UINT_MAX;	//Точка входа в программу
	for (list_commands::iterator i=lst.begin();i!=lst.end();++i,++addr)
	{
		code_cmd & cmd = (*i);

		if (cmd.Operation==FUNC) // Наткнулись на объявление функции
		{
			_ASSERT(cmd.Op1.GetClass()==FUNC_CLASS_ID);
			mapFuncs[cmd.Op1.Index]=addr+1;// CCылаемся на 1-ую команду после объявления 
			if (_tcscmp(cmd.Op1.GetSzText(),"main")==0)
				m_nStart=addr+1;
		}
		else if (cmd.Operation==LABEL)	//Наткнелись на метку
		{
			_ASSERT(cmd.Op1.GetClass()==LABEL_CLASS_ID);
			mapLabels[cmd.Op1.Index]=addr+1; // CCылаемся на 1-ую команду после объявления 
		}
	}
	// Замена меток на фактические адреса 
	for (list_commands::iterator i=lst.begin();i!=lst.end();++i)
	{
		code_cmd & cmd = (*i);
		if (cmd.Operation==CALL) // Наткнулись на вызов функции
		{
			_ASSERT(cmd.Op1.GetClass()==FUNC_CLASS_ID);
			map_addr::iterator it=mapFuncs.find(cmd.Op1.Index);
			_ASSERT(it!=mapFuncs.end());
			cmd.Op1.SetClass(DIRECT_NUMBER_CLASS_ID);
			cmd.Op1.Index=(*it).second;
		}
		else  if ((cmd.Operation==GET_JZ) || // Наткнулись на переход
				  (cmd.Operation==JMP))
		{
			_ASSERT(cmd.Op1.GetClass()==LABEL_CLASS_ID);
			map_addr::iterator it=mapLabels.find(cmd.Op1.Index);
			_ASSERT(it!=mapFuncs.end());

			cmd.Op1.SetClass(DIRECT_NUMBER_CLASS_ID);
			cmd.Op1.Index=(*it).second;
		}
	}

	// Формирование грамматик
	addr=0;
	for (list_commands::iterator i=lst.begin();i!=lst.end();++i,++addr)
	{
		code_cmd & cmd = (*i);
		if (cmd.Operation==SUBROUTINE) 
		{
			_ASSERT(cmd.Op1.GetClass()==SUBROUTINE_CLASS_ID);
			bool bRes=(pIGrmKrnl->ReplaceFunctionNameToAddr)(cmd.Op1.GetSzPureText(),addr+1);
			_ASSERT(bRes);
		}
		else if (cmd.Operation==PARSE) 
		{
			DWORD nIndex;	//Библиотечные функции работают только c WinApi-типами
			if (!(pIGrmKrnl->GetRuleIndex)(cmd.Op1.GetSzPureText(),nIndex))
			{
				errors::e_unresolved_rule(cmd.Op1.GetSzPureText());
				return false;
			}
			cmd.Op1.Index=nIndex;
			cmd.Op1.SetClass(DIRECT_NUMBER_CLASS_ID);
		}
	}
	_ASSERT((pIGrmKrnl->CheckOnReplacement)()); // Проверяет, чтобы все функции были заменены
	(pIGrmKrnl->BuildTree)();

	str szUnresolvedRule;
	if (!pIGrmKrnl->CheckOnUnresolvedRules(szUnresolvedRule))
	{
		errors::e_unresolved_rule((cstr)szUnresolvedRule);
		return false;
	}

	cstr szLeftSideRecRule;
	if (!pIGrmKrnl->CheckOnLeftSidedRecursion(szLeftSideRecRule))
	{
		errors::w_left_sided_recursion(szLeftSideRecRule,0);
		//return false;
	}

	// Проверка на наличие входной точки
	if (m_nStart==UINT_MAX)
		errors::w_no_entry_point();

	#ifdef __DIAGNOSTIC__
	{
		CPCodeList lnk_lst(m_BuildContext,change_fileext(szOutputFileName,".lnk.").c_str());
		for (list_commands::iterator i=lst.begin();i!=lst.end();++i)
			lnk_lst.Add(*i);
	}
	#endif

	return (bMakeSfx) ? Save2SFXFile(szOutputFileName,true) : Save2BinFile(szOutputFileName,true);
}
