/********************************************************************
	created:	2004/07/06
	created:	6:7:2004   23:40
	filename: 	X:\MultiTranslator\sources\mtdev\storage.cpp
	file path:	X:\MultiTranslator\sources\mtdev
	file base:	storage
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	Реализация сохранения настроек программы
*********************************************************************/

#include "stdafx.h"
#include "storage.h"

//Создание переходника для объекта
static i_objects::object_thunk<CStorage> ProjectProps(__MSG_PREFIX__,i_objects::support_interface<IStorageLink>());


//------------------------------------------------------------------------
CStorage::CStorage(void)
{

}
//------------------------------------------------------------------------
CStorage::~CStorage(void)
{
}
//------------------------------------------------------------------------
void CStorage::GetRegistryPath(cstr szSection, cstr szFunctionName, OUT string_smart & strRegSection, OUT string_smart & strRegEntry)
{
	string_split s2(szFunctionName,true,"(");
	string_smart strParamName = s2.get_left_();
	string_split s(strParamName.c_str(),true,":");
	
	while (!s.get_right_().isempty()){
		strParamName=s.get_right_();
		s=s.get_right_();
	}

	if ((strnicmp(strParamName.c_str(),"Get",3)==0) || 
		((strnicmp(strParamName.c_str(),"Set",3)==0)) ||
		((strnicmp(strParamName.c_str(),"Put",3)==0))
		)
		strParamName.del(0,3);
	if (strParamName.c_str()[0]=='_')
		strParamName.del(0);
	strParamName.replace("_","\\");

	//Формируем путь
	string_smart strFullPath(include_backslash(szSection),strParamName);
	strRegSection = extract_filedir(strFullPath);
	strRegEntry = extract_filename(strFullPath);
}
//------------------------------------------------------------------------
variant CStorage::GetProperty(cstr szSection, cstr szFunctionName, const variant & default_value)
{
	string_smart strSection, strEntry;
	GetRegistryPath(szSection,szFunctionName,strSection,strEntry);

	if (default_value.isstring()) 
		return AfxGetApp()->GetProfileString(strSection.c_str(),strEntry.c_str(),default_value.as_cstr());
	else if (default_value.isnumerical())
		return (int)AfxGetApp()->GetProfileInt(strSection.c_str(),strEntry.c_str(),default_value.as_int());
	else
		ASSERT_RETURN(false,default_value);
}
//------------------------------------------------------------------------
void CStorage::SetProperty(cstr szSection,cstr szFunctionName, const variant & _value,IStorageLink::EParam nParam)
{
	variant value = _value;

	//Исправляем -1 для TRUE в OLE на 1
	if (value.isshort() && (value.as_int()==-1)) //COM-true всегда есть -1
		value=1;

	string_smart strSection, strEntry;
	GetRegistryPath(szSection,szFunctionName,strSection,strEntry);

	bool bChanged=true;

	//Проверяем, а есть ли вообще хоть какое-то значение в реестре. Если нет
	//bChanged=true;
	HKEY hSecKey = AfxGetApp()->GetSectionKey(strSection.c_str());
	if (hSecKey != NULL){
		BYTE tmp[4096]; DWORD nLen = sizeof(tmp); DWORD nRegType;
		bChanged = (RegQueryValueEx(hSecKey,strEntry.c_str(),NULL,&nRegType,tmp,&nLen)!=ERROR_SUCCESS);
		RegCloseKey(hSecKey);
	};
	
	//Если в реестре что-то записано, то нужно проверить, не то же самое там хранится
	if (!bChanged)
		bChanged = (GetProperty(szSection,szFunctionName,value)!=value);

	if (value.isstring())
		AfxGetApp()->WriteProfileString(strSection.c_str(),strEntry.c_str(),value.as_cstr());
	else
		AfxGetApp()->WriteProfileInt(strSection.c_str(),strEntry.c_str(),value.as_int());

	if (bChanged)
		i_objects::event<ARGS_1(IStorageLink::EParam)>(IStorageLink::IEventSink::evOnChangeData,nParam);
}


		
