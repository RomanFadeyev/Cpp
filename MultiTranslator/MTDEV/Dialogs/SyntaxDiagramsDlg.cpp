/********************************************************************
	created:	2002/09/21
	created:	21:9:2002   18:09
	filename: 	$(Projects)\sources\mtdev\dialogs\syntaxdiagramsdlg.cpp
	file path:	$(Projects)\sources\mtdev\dialogs
	file base:	syntaxdiagramsdlg
	file ext:	cpp
	author:		Fadeyev R.V.
	
	purpose:	
*********************************************************************/

#include "stdafx.h"

#include <GUIService.h>
#include <ModuleNames.h> 

#include "SyntaxDiagramsDlg.h"

//Создание переходника для объекта
static i_objects::object_thunk< CdlgSyntaxDiagrams > dlgSyntaxDiagrams(__MSG_PREFIX__);


//------------------------------------------------------------------------
// CdlgSyntaxDiagrams
//------------------------------------------------------------------------
CdlgSyntaxDiagrams::CdlgSyntaxDiagrams()
{
}
//------------------------------------------------------------------------
CdlgSyntaxDiagrams::~CdlgSyntaxDiagrams()
{
}
//------------------------------------------------------------------------
bool CdlgSyntaxDiagrams::LoadLibrary()
{
	if (m_spSDLibraryInfo.GetInterfacePtr())	//Уже заинициализирована
		return true;

	if (!CreateInterfaceInstance(m_spSDLibraryInfo,__uuidof(mtdev_sd::LibraryInfo),module_names::szSDLibName))
		return false;

	//Теперь инициализируем библиотеку. Это можно делать только после вызова SetRegistryKey
	m_spSDLibraryInfo->RegistryKey=ApplicationData::APP_REG_KEY;
	m_spSDLibraryInfo->SupportMail = ApplicationData::APP_EMAIL;
	m_spSDLibraryInfo->CompanyName = ApplicationData::APP_COMPANY;
	m_spSDLibraryInfo->ProductName = ApplicationData::APP_NAME;
	m_spSDLibraryInfo->AppIcon = (OLE_HANDLE)AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	return true;
}
//------------------------------------------------------------------------
bool CdlgSyntaxDiagrams::Init()
{
	if (!LoadLibrary())
		return false;

	if (!CreateInterfaceInstance(m_spSD,__uuidof(mtdev_sd::SyntaxDiagram),module_names::szSDLibName))
		return false;

	//Запуск парсера
	IParserLink * pParserIface = i_objects::GetInterface<IParserLink>();
	ASSERT_RETURN_FALSE(pParserIface);

	IParserLink::syntax_info SyntaxInfo;
	//Разбор
	pParserIface->ParseWholeProject(*m_pProjectLink,SyntaxInfo);

	CWaitCursor wait_cursor;
	m_spSD->BeginUpdate();
	LOG_PROTECT_CODE_BEGIN
	 
	//Правила
	for (IParserLink::si_iterator i=SyntaxInfo.m_Rules.GetFirstUnitPos();!SyntaxInfo.m_Rules.IsEndUnitPos(i);) 
	{
		IParserLink::syntax_item & SRule = SyntaxInfo.m_Rules.GetNextUnit(i);
		mtdev_sd::ISDRule * pSDRule = m_spSD->AddRule();
		pSDRule->Name = SRule.strName.c_str();
		//Варианты
		for (IParserLink::si_iterator j=SRule.GetFirstUnitPos();!SRule.IsEndUnitPos(j);) 
		{
			IParserLink::syntax_item & SVariant = SRule.GetNextUnit(j);
			mtdev_sd::ISDVariant * pSDVariant = pSDRule->AddVariant();
			pSDVariant->Name = SVariant.strName.c_str();
			//Символы
			for (IParserLink::si_iterator k=SVariant.GetFirstUnitPos();!SVariant.IsEndUnitPos(k);) 
			{
				IParserLink::syntax_item & STerm = SVariant.GetNextUnit(k);
				mtdev_sd::ISDSymbol * pSDSymbol = pSDVariant->AddSymbol();
				pSDSymbol->Name = STerm.strName.c_str();
				//Здесь должно быть точное соответствие констант. Надо следить.
				pSDSymbol->SymbolClass = (mtdev_sd::TSymbolClass)STerm.nSymbolClass;
			}
		}
	}
	LOG_PROTECT_CODE_END

	LOG_PROTECT_CODE_BEGIN
	m_spSD->EndUpdate();
	
	m_spSD->LoadPlacement(GetPlacementDocPath().c_str());

	return true;
	LOG_PROTECT_CODE_END
	
	return false;  
}
//------------------------------------------------------------------------
bool CdlgSyntaxDiagrams::Run(IProjectDocLink * pProjectLink, show_type nShowType)
{
	m_nShowType = nShowType;
	m_pProjectLink=pProjectLink;
	if (!Init())
		return false;
	ASSERT_RETURN_FALSE(m_spSD.GetInterfacePtr());
	m_spSD->Show();
	m_spSD.Release();	//Освободим окно, чтобы не болталось
	return true;
}
