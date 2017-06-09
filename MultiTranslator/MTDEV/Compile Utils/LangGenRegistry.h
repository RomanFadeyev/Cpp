/********************************************************************
	created:	2003/12/27
	created:	27:12:2003   10:50
	filename: 	$(Projects)\sources\mtdev\compile utils\LangGenregistry.h
	file path:	$(Projects)\sources\mtdev\compile utils
	file base:	LangGenregistry
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/
#pragma once

#include <BaseUtils.h>
#include "CObjectEx.h" //���� �� ���������
#include <GrmLangGenExports.h>
#include <BaseStructs.h>

#include "Names.h"

using namespace base_utils;

class CLangGenRegistry :public CCObjectEx, public ILangGenRegistryLink
{
private:
	//���������� lang_gen_item
	class lang_gen_item_impl : public lang_gen_item
	{
	public:
		lang_gen_item_impl(cstr szPath, cstr szName, cstr szDescription){
			m_strPath=szPath;
			m_strName=szName;
			m_strDescription=szDescription;
		}
	};
	//-----------------------

	bool			m_Scanned;
	lst_lang_gen	m_LangGenLst;

	void Scan();
	void ScanDirectory(cstr szDirectory);
	void ReadProperties(cstr szFilePath);
protected:
	//������ ILangGenRegistry
	override void GetList(lst_lang_gen & aList) const;	//��������� ������ ���� ����������� ����
	//�� ����� ���������� ����������� ������ ���� � DLL
	override bool FindByName(const string_adapter strName, OUT lang_gen_item & Item) const;
public:
	CLangGenRegistry();
	virtual ~CLangGenRegistry();
};
