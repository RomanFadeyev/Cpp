/********************************************************************
	created:	2002/03/03
	created:	3:3:2002   12:36
	filename: 	$(Projects)\Sources\MTDEv\Dialogs\NewProject.h
	file path:	$(Projects)\Sources\MTDEv\Dialogs

	file base:	NewProject
	file ext:	h
	author:		Fadeyev R.V.
	
	purpose:	Display a dialog to allow the user to select a file path.
*********************************************************************/
#pragma once

#include "CObjectEx.h" //Один из родителей
#include "BaseStructs.h"


/////////////////////////////////////////////////////////////////////////////
// CdlgNewProject dialog
class CdlgNewProject : public CCObjectEx, public INewProjectLink
{ 
public:
	//Класс - мастер создания проекта. Базовый
	class CProjectWizard: public INewProjectWizardLink
	{
	protected:
		HICON		m_hIcon;
		string_std	m_strTitle;
	public:
		//Возвращает иконку для отображения в списке
		virtual HICON GetImage() {return m_hIcon;};
		//Возвращает свое название
		virtual cstr  GetTitle(){return m_strTitle.c_str();};
		//Виртуальный деструктор
		virtual ~CProjectWizard(){};
	};
	
	//Класс - мастер создания проекта по умолчанию
	class CProjectWizardDefault: public CProjectWizard
	{
	public:
		CProjectWizardDefault();
		override ~CProjectWizardDefault();
		override bool Run(IProjectDocLink* pIface){return true;}
	};
	
	//Класс - мастер создания минимального проекта 
	class CProjectWizardSimple: public CProjectWizard
	{
	public:
		CProjectWizardSimple();
		override ~CProjectWizardSimple();
		override bool Run(IProjectDocLink* pIface);
	};


// Construction
public:
	CdlgNewProject();   // standard constructor

private:
	map<OLE_HANDLE,CProjectWizard*> m_mapRegisteredWizards;

protected:
	override bool Run(
		string_adapter szProjectNameDef, //Имя проекта по умолчанию
		string_adapter szProjectDirDef,	//Родительская директория по умолчанию
		OUT string_smart & out_strProjectName,	//Имя проекта
		OUT string_smart & out_strProjectDir,	//Родительская директория, где будет размещен проект
		INewProjectWizardLink* & pWizardLink 
		);

	override void OnRunApp();
	override void OnStopApp();
};
